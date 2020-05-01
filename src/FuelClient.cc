/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <google/protobuf/text_format.h>
#include <ignition/msgs/fuel_metadata.pb.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <regex>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include <ignition/msgs/Utility.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/JSONParser.hh"
#include "ignition/fuel_tools/LocalCache.hh"
#include "ignition/fuel_tools/ModelIdentifier.hh"
#include "ignition/fuel_tools/ModelIterPrivate.hh"
#include "ignition/fuel_tools/RestClient.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"
#include "ignition/fuel_tools/WorldIterPrivate.hh"

using namespace ignition;
using namespace fuel_tools;

/// \brief Private Implementation
class ignition::fuel_tools::FuelClientPrivate
{
  /// \brief A model URL,
  /// E.g.: https://fuel.ignitionrobotics.org/1.0/caguero/models/Beer/2
  /// Where the API version and the model version are optional.
  public: const std::string kModelUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "models"
    "models\\/+"
    // Name
    "([^\\/]+)\\/*"
    // Version
    "([0-9]*|tip)"};

  /// \brief A world URL,
  /// E.g.: https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty/1
  /// Where the API version and the world version are optional.
  public: const std::string kWorldUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "worlds"
    "worlds\\/+"
    // Name
    "([^\\/]+)\\/*"
    // Version
    "([0-9]*|tip)"};

  /// \brief A model file URL,
  /// E.g.: https://server.org/1.0/owner/models/modelname/files/meshes/mesh.dae
  /// Where the API version is optional, but the model version is required.
  public: const std::string kModelFileUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "models"
    "models\\/+"
    // Model
    "([^\\/]+)\\/+"
    // Version
    "([0-9]*|tip)\\/+"
    // "files"
    "files\\/+"
    // File path
    "(.*)"};

  /// \brief A world file URL,
  /// E.g.: https://server.org/1.0/owner/worlds/worldname/files/meshes/mesh.dae
  /// Where the API version is optional, but the world version is required.
  public: const std::string kWorldFileUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "worlds"
    "worlds\\/+"
    // World
    "([^\\/]+)\\/+"
    // Version
    "([0-9]*|tip)\\/+"
    // "files"
    "files\\/+"
    // File path
    "(.*)"};

  /// \brief Recursively get all the files in the given path.
  /// \param[in] _path Path to process.
  /// \param[out] _files All the files in the given _path.
  public: void AllFiles(const std::string &_path,
              std::vector<std::string> &_files) const;

  /// \brief Client configuration
  public: ClientConfig config;

  /// \brief RESTful client
  public: Rest rest;

  /// \brief Local Cache
  public: std::shared_ptr<LocalCache> cache;

  /// \brief Regex to parse Ignition Fuel model URLs.
  public: std::unique_ptr<std::regex> urlModelRegex;

  /// \brief Regex to parse Ignition Fuel world URLs.
  public: std::unique_ptr<std::regex> urlWorldRegex;

  /// \brief Regex to parse Ignition Fuel model file URLs.
  public: std::unique_ptr<std::regex> urlModelFileRegex;

  /// \brief Regex to parse Ignition Fuel world file URLs.
  public: std::unique_ptr<std::regex> urlWorldFileRegex;

  /// \brief The set of licenses where the key is the name of the license
  /// and the value is the license ID on a Fuel server. See the
  /// PopulateLicenses function.
  public: std::map<std::string, unsigned int> licenses;
};

//////////////////////////////////////////////////
FuelClient::FuelClient()
  : FuelClient(ClientConfig(), Rest(), nullptr)
{
}

//////////////////////////////////////////////////
FuelClient::FuelClient(const ClientConfig &_config, const Rest &_rest,
    LocalCache *_cache)
  : dataPtr(new FuelClientPrivate)
{
  this->dataPtr->config = _config;
  this->dataPtr->rest = _rest;
  this->dataPtr->rest.SetUserAgent(this->dataPtr->config.UserAgent());

  if (nullptr == _cache)
    this->dataPtr->cache.reset(new LocalCache(&(this->dataPtr->config)));
  else
    this->dataPtr->cache.reset(_cache);

  this->dataPtr->urlModelRegex.reset(new std::regex(
    this->dataPtr->kModelUrlRegexStr));
  this->dataPtr->urlWorldRegex.reset(new std::regex(
    this->dataPtr->kWorldUrlRegexStr));
  this->dataPtr->urlModelFileRegex.reset(new std::regex(
    this->dataPtr->kModelFileUrlRegexStr));
  this->dataPtr->urlWorldFileRegex.reset(new std::regex(
    this->dataPtr->kWorldFileUrlRegexStr));
}

//////////////////////////////////////////////////
FuelClient::~FuelClient()
{
}

//////////////////////////////////////////////////
ClientConfig &FuelClient::Config()
{
  return this->dataPtr->config;
}

//////////////////////////////////////////////////
Result FuelClient::ModelDetails(const ModelIdentifier &_id,
    ModelIdentifier &_model) const
{
  ignition::fuel_tools::Rest rest;
  RestResponse resp;

  auto serverUrl = _id.Server().Url().Str();
  auto version = _id.Server().Version();
  common::URIPath path;
  path = path / _id.Owner() / "models" / _id.Name();

  resp = rest.Request(HttpMethod::GET, serverUrl, version,
      path.Str(), {}, {}, "");
  if (resp.statusCode != 200)
    return Result(ResultType::FETCH_ERROR);

  _model = JSONParser::ParseModel(resp.data, _id.Server());

  return Result(ResultType::FETCH);
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ServerConfig &_server)
{
  return const_cast<const FuelClient*>(this)->Models(_server);
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ServerConfig &_server) const
{
  ModelIter iter = ModelIterFactory::Create(this->dataPtr->rest,
      _server, "models");

  if (!iter)
  {
    // Return just the cached models
    ignwarn << "Failed to fetch models from server, returning cached models."
            << std::endl << _server.AsString() << std::endl;

    ModelIdentifier id;
    id.SetServer(_server);

    return this->dataPtr->cache->MatchingModels(id);
  }
  return iter;
}

//////////////////////////////////////////////////
Result FuelClient::WorldDetails(const WorldIdentifier &_id,
    WorldIdentifier &_world) const
{
  auto serverUrl = _id.Server().Url().Str();

  if (serverUrl.empty() || _id.Owner().empty() || _id.Name().empty())
    return Result(ResultType::FETCH_ERROR);

  ignition::fuel_tools::Rest rest;
  RestResponse resp;

  auto version = _id.Server().Version();
  common::URIPath path;
  path = path / _id.Owner() / "worlds" / _id.Name();

  resp = rest.Request(HttpMethod::GET, serverUrl, version,
      path.Str(), {}, {}, "");
  if (resp.statusCode != 200)
    return Result(ResultType::FETCH_ERROR);

  _world = JSONParser::ParseWorld(resp.data, _id.Server());

  return Result(ResultType::FETCH);
}

//////////////////////////////////////////////////
WorldIter FuelClient::Worlds(const ServerConfig &_server) const
{
  Rest rest(this->dataPtr->rest);
  WorldIter iter = WorldIterFactory::Create(rest, _server, "worlds");

  if (!iter)
  {
    // Return just the cached worlds
    ignwarn << "Failed to fetch worlds from server, returning cached worlds."
            << std::endl << _server.AsString() << std::endl;

    WorldIdentifier id;
    id.SetServer(_server);

    return this->dataPtr->cache->MatchingWorlds(id);
  }
  return iter;
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ModelIdentifier &_id)
{
  return const_cast<const FuelClient*>(this)->Models(_id);
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ModelIdentifier &_id) const
{
  // Check local cache first
  ModelIter localIter = this->dataPtr->cache->MatchingModels(_id);
  if (localIter)
    return localIter;

  // TODO(nkoenig) try to fetch model directly from a server
  // Note: ign-fuel-server doesn't like URLs ending in /
  common::URIPath path;
  if (!_id.Name().empty() && !_id.Owner().empty())
    path = path / _id.Owner() / "models" / _id.Name();
  else if (!_id.Owner().empty())
    path = path / _id.Owner() / "models";

  if (path.Str().empty())
    return localIter;

  ignmsg << _id.UniqueName() << " not found in cache, attempting download\n";

  return ModelIterFactory::Create(this->dataPtr->rest, _id.Server(),
      path.Str());
}

//////////////////////////////////////////////////
WorldIter FuelClient::Worlds(const WorldIdentifier &_id) const
{
  // Check local cache first
  WorldIter localIter = this->dataPtr->cache->MatchingWorlds(_id);
  if (localIter)
    return localIter;

  ignmsg << _id.UniqueName() << " not found in cache, attempting download\n";

  // Note: ign-fuel-server doesn't like URLs ending in /
  common::URIPath path;
  if (!_id.Name().empty())
    path = path / _id.Owner() / "worlds" / _id.Name();
  else
    path = path / _id.Owner() / "worlds";

  Rest rest(this->dataPtr->rest);
  return WorldIterFactory::Create(rest, _id.Server(), path.Str());
}

//////////////////////////////////////////////////
Result FuelClient::UploadModel(const std::string &_pathToModelDir,
    const ModelIdentifier &_id, const std::vector<std::string> &_headers,
    bool _private)
{
  ignition::fuel_tools::Rest rest;
  RestResponse resp;

  if (!common::exists(_pathToModelDir))
  {
    ignerr << "The model path[" << _pathToModelDir << "] doesn't exist.\n";
    return Result(ResultType::UPLOAD_ERROR);
  }

  ignition::msgs::FuelMetadata meta;

  // Try the `metadata.pbtxt` file first since it contains more information
  // than `model.config`.
  if (common::exists(common::joinPaths(_pathToModelDir, "metadata.pbtxt")))
  {
    std::string filePath = common::joinPaths(_pathToModelDir, "metadata.pbtxt");

    ignmsg << "Parsing " << filePath  << std::endl;

    // Read the pbtxt file.
    std::ifstream inputFile(filePath);
    std::string inputStr((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());

    // Parse the file into the fuel metadata message
    google::protobuf::TextFormat::ParseFromString(inputStr, &meta);
  }
  else if (common::exists(common::joinPaths(_pathToModelDir, "model.config")))
  {
    std::string filePath = common::joinPaths(_pathToModelDir, "model.config");

    ignmsg << "Parsing " << filePath << std::endl;

    std::ifstream inputFile(filePath);
    std::string inputStr((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());

    if (!ignition::msgs::ConvertFuelMetadata(inputStr, meta))
    {
      ignerr << "Unable to convert model config[" << _pathToModelDir << "].\n";
      return Result(ResultType::UPLOAD_ERROR);
    }
  }
  else
  {
    ignerr << "Provided model directory[" <<  _pathToModelDir
      << "] needs a metadata.pbtxt or a model.confg file.";
    return Result(ResultType::UPLOAD_ERROR);
  }

  std::multimap<std::string, std::string> form =
  {
    {"name", meta.name()},
    {"description", meta.description()},
    {"private", _private ? "1" : "0"},
  };

  // \todo(nkoenig) The ign-fuelserver expects an integer number for the
  // license information. The fuelserver should be modified to accept
  // a string. Otherwise, we have to bake into each client a mapping of
  // license name to integer.
  //
  // If there is no license information, then default to
  // "Creative Commons - Public Domain"
  if (this->dataPtr->licenses.empty() || !meta.has_legal())
  {
    form.emplace("license", "1");
  }
  // Otherwise, we have license information (see PopulateLicenses) and
  // legal information
  else
  {
    // Find the license by name.
    std::map<std::string, unsigned int>::const_iterator licenseIt =
      this->dataPtr->licenses.find(meta.legal().license());
    if (licenseIt != this->dataPtr->licenses.end())
    {
      form.emplace("license", std::to_string(licenseIt->second));
    }
    // No license found, print an error and return.
    else
    {
      std::string validLicenseNames;
      auto end = this->dataPtr->licenses.end();
      std::advance(end, -1);
      //for (size_t i = 0; i < this->dataPtr->licenses.size(); ++i)
      for (licenseIt = this->dataPtr->licenses.begin();
           licenseIt != end; ++licenseIt)
      {
        validLicenseNames += "    " + licenseIt->first + "\n";
      }
      validLicenseNames += "    " + licenseIt->first;

      ignerr << "Invalid license[" << meta.legal().license() << "].\n"
             << "  Valid licenses include:\n"
             << validLicenseNames << std::endl;

      return Result(ResultType::UPLOAD_ERROR);
    }
  }

  // Add tags
  std::string tags;
  for (int i = 0; i < meta.tags_size(); ++i)
    tags += meta.tags(i) + ",";
  if (!tags.empty())
    form.emplace("tags", tags);

  // Recursively get all the files.
  std::vector<std::string> files;
  this->dataPtr->AllFiles(_pathToModelDir, files);
  for (const std::string &file : files)
  {
    form.emplace("file", std::string("@") + file + ";"
        + file.substr(_pathToModelDir.size()+1));
  }

  // Send the request.
  resp = rest.Request(HttpMethod::POST_FORM, _id.Server().Url().Str(),
      _id.Server().Version(), "models", {}, _headers, "", form);

  if (resp.statusCode != 200)
  {
    ignerr << "Failed to upload model." << std::endl
           << "  Server: " << _id.Server().Url().Str() << std::endl
           << "  Route: /models\n"
           << "  REST response code: " << resp.statusCode << std::endl;
    return Result(ResultType::FETCH_ERROR);
  }

  return Result(ResultType::UPLOAD);
}

//////////////////////////////////////////////////
Result FuelClient::DeleteModel(const ModelIdentifier &)
{
  ignerr << "Model deletion requires a private-token or JWT to be specified"
    << " in a header. No action is performed.\n";

  return Result(ResultType::DELETE_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::DeleteUrl(const ignition::common::URI &_uri,
    const std::vector<std::string> &_headers)
{
  ignition::fuel_tools::Rest rest;
  RestResponse resp;

  std::string server;
  std::string version;
  common::URIPath path;
  std::string type;
  std::string name;

  ModelIdentifier modelId;
  WorldIdentifier worldId;
  if (this->ParseModelUrl(_uri, modelId))
  {
    type = "model";
    name = modelId.UniqueName();
    server = modelId.Server().Url().Str();
    version = modelId.Server().Version();
    path = path / modelId.Owner() / "models" / modelId.Name();
  }
  else if (this->ParseWorldUrl(_uri, worldId))
  {
    type = "world";
    name = worldId.UniqueName();
    server = worldId.Server().Url().Str();
    version = worldId.Server().Version();
    path = path / worldId.Owner() / "worlds" / worldId.Name();
  }
  else
  {
    ignerr << "Unable to parse URI[" << _uri.Str() << "]\n";
    return Result(ResultType::DELETE_ERROR);
  }

  // Send the request.
  resp = rest.Request(HttpMethod::DELETE, server, version, path.Str(), {},
      _headers, "", {});

  if (resp.statusCode != 200)
  {
    ignerr << "Failed to delete resource." << std::endl
           << "  Server: " << server << std::endl
           << "  API Version: " << version << std::endl
           << "  Route: " << path.Str() << std::endl
           << "  REST response code: " << resp.statusCode << std::endl;
    return Result(ResultType::DELETE_ERROR);
  }
  else
  {
    ignmsg << "Deleted " << type << " [" << name << "]" << std::endl;
  }

  return Result(ResultType::DELETE);
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const ModelIdentifier &_id)
{
  return this->DownloadModel(_id, {});
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const ModelIdentifier &_id,
    const std::vector<std::string> &_headers)
{
  // Server config
  if (!_id.Server().Url().Valid() || _id.Server().Version().empty())
  {
    ignerr << "Can't download model, server configuration incomplete: "
          << std::endl << _id.Server().AsString() << std::endl;
    return Result(ResultType::FETCH_ERROR);
  }

  // Route
  common::URIPath route;
  route = route / _id.Owner() / "models" / _id.Name() / _id.VersionStr() /
        (_id.Name() + ".zip");

  ignmsg << "Downloading model [" << _id.UniqueName() << "]" << std::endl;

  // Request
  ignition::fuel_tools::Rest rest;
  RestResponse resp;
  resp = rest.Request(HttpMethod::GET, _id.Server().Url().Str(),
      _id.Server().Version(), route.Str(), {}, _headers, "");
  if (resp.statusCode != 200)
  {
    ignerr << "Failed to download model." << std::endl
           << "  Server: " << _id.Server().Url().Str() << std::endl
           << "  Route: " << route.Str() << std::endl
           << "  REST response code: " << resp.statusCode << std::endl;
    return Result(ResultType::FETCH_ERROR);
  }

  // Get version from header
  ModelIdentifier newId = _id;
  unsigned int version = 1;
  if (resp.headers.find("X-Ign-Resource-Version") != resp.headers.end())
  {
    try
    {
      version = std::stoi(resp.headers["X-Ign-Resource-Version"]);
    }
    catch(std::invalid_argument &)
    {
      ignwarn << "Failed to convert X-Ign-Resource-Version header value ["
              << resp.headers["X-Ign-Resource-Version"]
              << "] to integer. Hardcoding version 1." << std::endl;
    }
  }
  else
  {
    ignwarn << "Missing X-Ign-Resource-Version in REST response headers."
            << " Hardcoding version 1." << std::endl;
  }
  newId.SetVersion(version);

  // Save
  // Note that the save function doesn't return the path
  if (!this->dataPtr->cache->SaveModel(newId, resp.data, true))
    return Result(ResultType::FETCH_ERROR);

  return Result(ResultType::FETCH);
}

//////////////////////////////////////////////////
Result FuelClient::DownloadWorld(WorldIdentifier &_id)
{
  // Server config
  if (!_id.Server().Url().Valid() || _id.Server().Version().empty())
  {
    ignerr << "Can't download world, server configuration incomplete: "
          << std::endl << _id.Server().AsString() << std::endl;
    return Result(ResultType::FETCH_ERROR);
  }

  // Route
  common::URIPath route;
  route = route / _id.Owner() / "worlds" / _id.Name() / _id.VersionStr() /
        (_id.Name() + ".zip");

  ignmsg << "Downloading world [" << _id.UniqueName() << "]" << std::endl;

  // Request
  ignition::fuel_tools::Rest rest;
  RestResponse resp;
  resp = rest.Request(HttpMethod::GET, _id.Server().Url().Str(),
      _id.Server().Version(), route.Str(), {}, {}, "");
  if (resp.statusCode != 200)
  {
    ignerr << "Failed to download world." << std::endl
           << "  Server: " << _id.Server().Url().Str() << std::endl
           << "  Route: " << route.Str() << std::endl
           << "  REST response code: " << resp.statusCode << std::endl;
    return Result(ResultType::FETCH_ERROR);
  }

  // Get version from header
  unsigned int version = 1;

  if (resp.headers.find("X-Ign-Resource-Version") != resp.headers.end())
  {
    try
    {
      version = std::stoi(resp.headers["X-Ign-Resource-Version"]);
    }
    catch(std::invalid_argument &)
    {
      ignwarn << "Failed to convert X-Ign-Resource-Version header value ["
              << resp.headers["X-Ign-Resource-Version"]
              << "] to integer. Hardcoding version 1." << std::endl;
    }
  }
  else
  {
    ignwarn << "Missing X-Ign-Resource-Version in REST response headers."
            << " Hardcoding version 1." << std::endl;
  }
  _id.SetVersion(version);

  // Save
  if (!this->dataPtr->cache->SaveWorld(_id, resp.data, true))
    return Result(ResultType::FETCH_ERROR);

  return Result(ResultType::FETCH);
}

//////////////////////////////////////////////////
bool FuelClient::ParseModelUrl(const common::URI &_modelUrl,
    ModelIdentifier &_id)
{
  if (!_modelUrl.Valid())
    return false;

  auto urlStr = _modelUrl.Str();

  std::smatch match;
  std::string scheme;
  std::string server;
  std::string apiVersion;
  std::string owner;
  std::string modelName;
  std::string modelVersion;

  std::regex_match(urlStr, match, *this->dataPtr->urlModelRegex);

  if (std::regex_match(urlStr, match, *this->dataPtr->urlModelRegex) &&
      match.size() >= 5u)
  {
    unsigned int i{1};

    scheme = match[i++];
    server = match[i++];
    apiVersion = match[i++];
    owner = match[i++];
    modelName = match[i++];
    modelVersion = match[i++];
  }
  else
  {
    return false;
  }

  // Get remaining server information from config
  _id.Server().SetUrl(common::URI(scheme + "://" + server));
  _id.Server().SetVersion(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.Url().Str() == _id.Server().Url().Str())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.Url().Str() << "], but will use ["
                << s.Version() << "] as given in the config file."
                << std::endl;
      }
      _id.Server() = s;
      break;
    }
  }

  if (_id.Server().Version().empty())
  {
    ignwarn << "Server configuration is incomplete:" << std::endl
            << _id.Server().AsString();
  }

  _id.SetOwner(owner);
  _id.SetName(modelName);
  _id.SetVersionStr(modelVersion);

  return true;
}

//////////////////////////////////////////////////
bool FuelClient::ParseWorldUrl(const common::URI &_worldUrl,
    WorldIdentifier &_id)
{
  if (!_worldUrl.Valid())
    return false;

  auto urlStr = _worldUrl.Str();

  std::smatch match;
  std::string scheme;
  std::string server;
  std::string apiVersion;
  std::string owner;
  std::string worldName;
  std::string worldVersion;

  std::regex_match(urlStr, match, *this->dataPtr->urlWorldRegex);

  if (std::regex_match(urlStr, match, *this->dataPtr->urlWorldRegex) &&
      match.size() >= 5u)
  {
    unsigned int i{1};

    scheme = match[i++];
    server = match[i++];
    apiVersion = match[i++];
    owner = match[i++];
    worldName = match[i++];
    worldVersion = match[i++];
  }
  else
  {
    return false;
  }

  // Get remaining server information from config
  _id.Server().SetUrl(common::URI(scheme + "://" + server));
  _id.Server().SetVersion(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.Url() == _id.Server().Url())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.Url().Str() << "], but will use ["
                << s.Version() << "] as given in the config file."
                << std::endl;
      }
      _id.Server() = s;
      break;
    }
  }

  if (_id.Server().Version().empty())
  {
    ignwarn << "Server configuration is incomplete:" << std::endl
            << _id.Server().AsString();
  }

  _id.SetOwner(owner);
  _id.SetName(worldName);
  _id.SetVersionStr(worldVersion);

  return true;
}

//////////////////////////////////////////////////
bool FuelClient::ParseModelFileUrl(const common::URI &_fileUrl,
    ModelIdentifier &_id, std::string &_filePath)
{
  if (!_fileUrl.Valid())
    return false;

  auto urlStr = _fileUrl.Str();

  std::smatch match;
  std::string scheme;
  std::string server;
  std::string apiVersion;
  std::string owner;
  std::string modelName;
  std::string modelVersion;
  std::string file;

  if (std::regex_match(urlStr, match, *this->dataPtr->urlModelFileRegex) &&
      match.size() == 8u)
  {
    unsigned int i{1};

    scheme = match[i++];
    server = match[i++];
    apiVersion = match[i++];
    owner = match[i++];
    modelName = match[i++];
    modelVersion = match[i++];
    file = match[i++];
  }
  else
  {
    return false;
  }

  // Get remaining server information from config
  _id.Server().SetUrl(common::URI(scheme + "://" + server));
  _id.Server().SetVersion(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.Url().Str() == _id.Server().Url().Str())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.Url().Str() << "], but will use ["
                << s.Version() << "] as given in the config file."
                << std::endl;
      }
      _id.Server() = s;
      break;
    }
  }

  if (_id.Server().Version().empty())
  {
    ignwarn << "Server configuration is incomplete:" << std::endl
            << _id.Server().AsString();
  }

  _id.SetOwner(owner);
  _id.SetName(modelName);
  _id.SetVersionStr(modelVersion);
  _filePath = file;

  return true;
}

//////////////////////////////////////////////////
bool FuelClient::ParseWorldFileUrl(const common::URI &_fileUrl,
    WorldIdentifier &_id, std::string &_filePath)
{
  if (!_fileUrl.Valid())
    return false;

  auto urlStr = _fileUrl.Str();

  std::smatch match;
  std::string scheme;
  std::string server;
  std::string apiVersion;
  std::string owner;
  std::string worldName;
  std::string worldVersion;
  std::string file;

  if (std::regex_match(urlStr, match, *this->dataPtr->urlWorldFileRegex) &&
      match.size() == 8u)
  {
    unsigned int i{1};

    scheme = match[i++];
    server = match[i++];
    apiVersion = match[i++];
    owner = match[i++];
    worldName = match[i++];
    worldVersion = match[i++];
    file = match[i++];
  }
  else
  {
    return false;
  }

  // Get remaining server information from config
  _id.Server().SetUrl(common::URI(scheme + "://" + server));
  _id.Server().SetVersion(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.Url() == _id.Server().Url())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.Url().Str() << "], but will use ["
                << s.Version() << "] as given in the config file."
                << std::endl;
      }
      _id.Server() = s;
      break;
    }
  }

  if (_id.Server().Version().empty())
  {
    ignwarn << "Server configuration is incomplete:" << std::endl
            << _id.Server().AsString();
  }

  _id.SetOwner(owner);
  _id.SetName(worldName);
  _id.SetVersionStr(worldVersion);
  _filePath = file;

  return true;
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const common::URI &_modelUrl,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  if (!this->ParseModelUrl(_modelUrl, id))
  {
    return Result(ResultType::FETCH_ERROR);
  }

  // Download
  Result result = this->DownloadModel(id);
  if (!result)
    return result;

  // TODO(anyone) We shouldn't need to reconstruct the path, SaveModel should
  // be changed to return it

  // We need to figure out the version for the tip
  if (id.Version() == 0 || id.VersionStr() == "tip")
  {
    Model model = this->dataPtr->cache->MatchingModel(id);
    id.SetVersion(model.Identification().Version());
  }

  _path = ignition::common::joinPaths(this->Config().CacheLocation(),
      id.Server().Url().Path().Str(), id.Owner(), "models", id.Name(),
      id.VersionStr());

  return result;
}

//////////////////////////////////////////////////
Result FuelClient::DownloadWorld(const common::URI &_worldUrl,
  std::string &_path)
{
  // Get data from URL
  WorldIdentifier id;
  if (!this->ParseWorldUrl(_worldUrl, id))
  {
    return Result(ResultType::FETCH_ERROR);
  }

  // Download
  auto result = this->DownloadWorld(id);
  if (!result)
    return result;

  _path = id.LocalPath();

  return result;
}

//////////////////////////////////////////////////
bool FuelClient::CachedModel(const common::URI &_modelUrl)
{
  // Get data from URL
  ModelIdentifier id;
  if (!this->ParseModelUrl(_modelUrl, id))
    return Result(ResultType::FETCH_ERROR);

  // Check local cache
  return this->dataPtr->cache->MatchingModel(id) ? true : false;
}

//////////////////////////////////////////////////
Result FuelClient::CachedModel(const common::URI &_modelUrl,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  if (!this->ParseModelUrl(_modelUrl, id))
  {
    return Result(ResultType::FETCH_ERROR);
  }

  // Check local cache
  auto modelIter = this->dataPtr->cache->MatchingModel(id);
  if (modelIter)
  {
    _path = modelIter.PathToModel();
    return Result(ResultType::FETCH_ALREADY_EXISTS);
  }

  return Result(ResultType::FETCH_ERROR);
}

//////////////////////////////////////////////////
bool FuelClient::CachedWorld(const common::URI &_worldUrl)
{
  // Get data from URL
  WorldIdentifier id;
  if (!this->ParseWorldUrl(_worldUrl, id))
    return Result(ResultType::FETCH_ERROR);

  // Check local cache
  return this->dataPtr->cache->MatchingWorld(id);
}

//////////////////////////////////////////////////
Result FuelClient::CachedWorld(const common::URI &_worldUrl,
  std::string &_path)
{
  // Get data from URL
  WorldIdentifier id;
  if (!this->ParseWorldUrl(_worldUrl, id))
  {
    return Result(ResultType::FETCH_ERROR);
  }

  // Check local cache
  auto success = this->dataPtr->cache->MatchingWorld(id);
  if (success)
  {
    _path = id.LocalPath();
    return Result(ResultType::FETCH_ALREADY_EXISTS);
  }

  return Result(ResultType::FETCH_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::CachedModelFile(const common::URI &_fileUrl,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  std::string filePath;
  if (!this->ParseModelFileUrl(_fileUrl, id, filePath))
    return Result(ResultType::FETCH_ERROR);

  if (filePath.empty())
    return Result(ResultType::FETCH_ERROR);

  // Look for model
  auto modelIter = this->dataPtr->cache->MatchingModel(id);

  if (!modelIter)
    return Result(ResultType::FETCH_ERROR);

  auto modelPath = modelIter.PathToModel();

  // Check if file exists
  filePath = common::joinPaths(modelPath, filePath);

  if (common::exists(filePath))
  {
    _path = filePath;
    return Result(ResultType::FETCH_ALREADY_EXISTS);
  }

  return Result(ResultType::FETCH_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::CachedWorldFile(const common::URI &_fileUrl,
  std::string &_path)
{
  // Get data from URL
  WorldIdentifier id;
  std::string filePath;
  if (!this->ParseWorldFileUrl(_fileUrl, id, filePath))
    return Result(ResultType::FETCH_ERROR);

  if (filePath.empty())
    return Result(ResultType::FETCH_ERROR);

  // Look for world
  auto success = this->dataPtr->cache->MatchingWorld(id);

  if (!success)
    return Result(ResultType::FETCH_ERROR);

  auto worldPath = id.LocalPath();

  // Check if file exists
  filePath = common::joinPaths(worldPath, filePath);

  if (common::exists(filePath))
  {
    _path = filePath;
    return Result(ResultType::FETCH_ALREADY_EXISTS);
  }

  return Result(ResultType::FETCH_ERROR);
}

//////////////////////////////////////////////////
void FuelClientPrivate::AllFiles(const std::string &_path,
    std::vector<std::string> &_files) const
{
  common::DirIter dirIter(_path);
  common::DirIter end;
  while (dirIter != end)
  {
    if (common::isDirectory(*dirIter))
      this->AllFiles(*dirIter, _files);
    else
      _files.push_back(*dirIter);

    ++dirIter;
  }
}


//////////////////////////////////////////////////
void FuelClient::PopulateLicenses(const ServerConfig &_server)
{
  ignition::fuel_tools::Rest rest;
  RestResponse resp;

  // Send the request.
  resp = rest.Request(HttpMethod::GET, _server.Url().Str(),
      _server.Version(), "licenses", {}, {}, "");
  if (resp.statusCode != 200)
  {
    ignerr << "Failed to get license information from "
      << _server.Url().Str() << "/" << _server.Version() << std::endl;
  }
  else if (!JSONParser::ParseLicenses(resp.data, this->dataPtr->licenses))
  {
    ignerr << "Failed to parse license information[" << resp.data << "]\n";
  }
}

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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <regex>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/JSONParser.hh"
#include "ignition/fuel_tools/LocalCache.hh"
#include "ignition/fuel_tools/ModelIdentifier.hh"
#include "ignition/fuel_tools/ModelIterPrivate.hh"
#include "ignition/fuel_tools/REST.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"
#include "ignition/fuel_tools/WorldIterPrivate.hh"

using namespace ignition;
using namespace fuel_tools;

/// \brief Private Implementation
class ignition::fuel_tools::FuelClientPrivate
{
  /// \brief A model URL,
  /// E.g.: https://api.ignitionfuel.org/1.0/caguero/models/Beer/2
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
  /// E.g.: https://api.ignitionfuel.org/1.0/openrobotics/worlds/Empty/1
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

  /// \brief Client configuration
  public: ClientConfig config;

  /// \brief RESTful client
  public: REST rest;

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

  /// \brief The path where the configuration file is located.
  public: std::string configPath;
};

//////////////////////////////////////////////////
FuelClient::FuelClient()
  : FuelClient(ClientConfig(), REST(), nullptr)
{
}

//////////////////////////////////////////////////
FuelClient::FuelClient(const ClientConfig &_config, const REST &_rest,
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
Result FuelClient::ModelDetails(const ServerConfig &/*_server*/,
  const ModelIdentifier &_id, ModelIdentifier &_model) const
{
  ignition::fuel_tools::REST rest;
  RESTResponse resp;

  auto serverUrl = _id.Server().URL();
  auto version = _id.Server().Version();
  auto path = ignition::common::joinPaths(_id.Owner(), "models", _id.Name());

  resp = rest.Request(REST::GET, serverUrl, version, path, {}, {}, "");
  if (resp.statusCode != 200)
    return Result(Result::FETCH_ERROR);

  _model = JSONParser::ParseModel(resp.data, _id.Server());

  return Result(Result::FETCH);
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ServerConfig &_server)
{
  ModelIter iter = ModelIterFactory::Create(this->dataPtr->rest,
      _server, "models");

  if (!iter)
  {
    // Return just the cached models
    ignwarn << "Failed to fetch models from server, returning cached models."
            << std::endl << _server.AsString() << std::endl;

    ModelIdentifier id;
    id.Server(_server);

    return this->dataPtr->cache->MatchingModels(id);
  }
  return iter;
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
    id.Server(_server);

    return this->dataPtr->cache->MatchingModels(id);
  }
  return iter;
}

//////////////////////////////////////////////////
WorldIter FuelClient::Worlds(const ServerConfig &_server) const
{
  WorldIter iter = WorldIterFactory::Create(this->dataPtr->rest,
      _server, "worlds");

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
ModelIter FuelClient::Models(const ServerConfig &/*_server*/,
  const ModelIdentifier &_id)
{
  // Check local cache first
  ModelIter localIter = this->dataPtr->cache->MatchingModels(_id);
  if (localIter)
    return localIter;

  ignmsg << _id.UniqueName() << " not found in cache, attempting download\n";

  // Todo try to fetch model directly from a server
  // Note: ign-fuel-server doesn't like URLs ending in /
  std::string path;
  if (!_id.Name().empty())
    path = ignition::common::joinPaths(_id.Owner(), "models", _id.Name());
  else
    path = ignition::common::joinPaths(_id.Owner(), "models");

  return ModelIterFactory::Create(this->dataPtr->rest, _id.Server(), path);
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ServerConfig &/*_server*/,
  const ModelIdentifier &_id) const
{
  // Check local cache first
  ModelIter localIter = this->dataPtr->cache->MatchingModels(_id);
  if (localIter)
    return localIter;

  ignmsg << _id.UniqueName() << " not found in cache, attempting download\n";

  // Todo try to fetch model directly from a server
  // Note: ign-fuel-server doesn't like URLs ending in /
  std::string path;
  if (!_id.Name().empty())
    path = ignition::common::joinPaths(_id.Owner(), "models", _id.Name());
  else
    path = ignition::common::joinPaths(_id.Owner(), "models");

  return ModelIterFactory::Create(this->dataPtr->rest, _id.Server(), path);
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
  std::string path;
  if (!_id.Name().empty())
    path = ignition::common::joinPaths(_id.Owner(), "worlds", _id.Name());
  else
    path = ignition::common::joinPaths(_id.Owner(), "worlds");

  return WorldIterFactory::Create(this->dataPtr->rest, _id.Server(), path);
}

//////////////////////////////////////////////////
Result FuelClient::UploadModel(const ServerConfig &/*_server*/,
  const std::string &/*_pathToModelDir*/, const ModelIdentifier &/*_id*/)
{
  // TODO Upload a model and return an Result
  return Result(Result::UPLOAD_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::DeleteModel(const ServerConfig &/*_server*/,
  const ModelIdentifier &/*_id*/)
{
  // TODO Delete a model and return a Result
  return Result(Result::DELETE_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const ServerConfig &/*_server*/,
  const ModelIdentifier &_id)
{
  // Server config
  if (_id.Server().URL().empty() || _id.Server().Version().empty())
  {
    ignerr << "Can't download model, server configuration incomplete: "
          << std::endl << _id.Server().AsString() << std::endl;
    return Result(Result::FETCH_ERROR);
  }

  // Route
  auto route = ignition::common::joinPaths(_id.Owner(),
        "models", _id.Name(), _id.VersionStr(),
        _id.Name() + ".zip");

  // Request
  ignition::fuel_tools::REST rest;
  RESTResponse resp;
  resp = rest.Request(REST::GET, _id.Server().URL(), _id.Server().Version(),
      route, {}, {}, "");
  if (resp.statusCode != 200)
  {
    ignerr << "Failed to download model." << std::endl
           << "  Server: " << _id.Server().URL() << std::endl
           << "  Route: " << route << std::endl
           << "  REST response code: " << resp.statusCode << std::endl;
    return Result(Result::FETCH_ERROR);
  }

  // Get version from header
  auto newId = _id;
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
    return Result(Result::FETCH_ERROR);

  return Result(Result::FETCH);
}

//////////////////////////////////////////////////
Result FuelClient::DownloadWorld(WorldIdentifier &_id)
{
  // Server config
  if (_id.Server().URL().empty() || _id.Server().Version().empty())
  {
    ignerr << "Can't download world, server configuration incomplete: "
          << std::endl << _id.Server().AsString() << std::endl;
    return Result(Result::FETCH_ERROR);
  }

  // Route
  auto route = ignition::common::joinPaths(_id.Owner(),
        "worlds", _id.Name(), _id.VersionStr(),
        _id.Name() + ".zip");

  // Request
  ignition::fuel_tools::REST rest;
  RESTResponse resp;
  resp = rest.Request(REST::GET, _id.Server().URL(), _id.Server().Version(),
      route, {}, {}, "");
  if (resp.statusCode != 200)
  {
    ignerr << "Failed to download world." << std::endl
           << "  Server: " << _id.Server().URL() << std::endl
           << "  Route: " << route << std::endl
           << "  REST response code: " << resp.statusCode << std::endl;
    return Result(Result::FETCH_ERROR);
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
  // Note that the save function doesn't return the path
  if (!this->dataPtr->cache->SaveWorld(_id, resp.data, true))
    return Result(Result::FETCH_ERROR);

  return Result(Result::FETCH);
}

//////////////////////////////////////////////////
bool FuelClient::ParseModelURL(const std::string &_modelUrl,
    ServerConfig &/*_server*/, ModelIdentifier &_id)
{
  return this->ParseModelUrl(common::URI(_modelUrl), _id);
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
  _id.Server().URL(scheme + "://" + server);
  _id.Server().Version(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.URL() == _id.Server().URL())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.URL() << "], but will use ["
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

  _id.Owner(owner);
  _id.Name(modelName);
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
  _id.Server().URL(scheme + "://" + server);
  _id.Server().Version(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.URL() == _id.Server().URL())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.URL() << "], but will use ["
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
  _id.Server().URL(scheme + "://" + server);
  _id.Server().Version(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.URL() == _id.Server().URL())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.URL() << "], but will use ["
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

  _id.Owner(owner);
  _id.Name(modelName);
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
  _id.Server().URL(scheme + "://" + server);
  _id.Server().Version(apiVersion);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.URL() == _id.Server().URL())
    {
      if (!apiVersion.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << apiVersion
                << "] for server [" << s.URL() << "], but will use ["
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
Result FuelClient::DownloadModel(const std::string &_modelUrl,
  std::string &_path)
{
  return this->DownloadModel(common::URI(_modelUrl), _path);
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const common::URI &_modelUrl,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  if (!this->ParseModelUrl(_modelUrl, id))
  {
    return Result(Result::FETCH_ERROR);
  }

  // Download
  ServerConfig srv;
  auto result = this->DownloadModel(srv, id);
  if (!result)
    return result;

  // TODO(anyone) We shouldn't need to reconstruct the path, SaveModel should
  // be changed to return it

  // We need to figure out the version for the tip
  if (id.Version() == 0)
  {
    auto model = this->dataPtr->cache->MatchingModel(id);
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
    return Result(Result::FETCH_ERROR);
  }

  // Download
  auto result = this->DownloadWorld(id);
  if (!result)
    return result;

  _path = id.LocalPath();

  return result;
}

//////////////////////////////////////////////////
Result FuelClient::CachedModel(const common::URI &_modelUrl,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  ServerConfig srv;
  if (!this->ParseModelURL(_modelUrl.Str(), srv, id))
  {
    return Result(Result::FETCH_ERROR);
  }

  // Check local cache
  auto modelIter = this->dataPtr->cache->MatchingModel(id);
  if (modelIter)
  {
    _path = modelIter.PathToModel();
    return Result(Result::FETCH_ALREADY_EXISTS);
  }

  return Result(Result::FETCH_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::CachedWorld(const common::URI &_worldUrl,
  std::string &_path)
{
  // Get data from URL
  WorldIdentifier id;
  if (!this->ParseWorldUrl(_worldUrl, id))
  {
    return Result(Result::FETCH_ERROR);
  }

  // Check local cache
  auto success = this->dataPtr->cache->MatchingWorld(id);
  if (success)
  {
    _path = id.LocalPath();
    return Result(Result::FETCH_ALREADY_EXISTS);
  }

  return Result(Result::FETCH_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::CachedModelFile(const common::URI &_fileUrl,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  std::string filePath;
  if (!this->ParseModelFileUrl(_fileUrl, id, filePath))
    return Result(Result::FETCH_ERROR);

  if (filePath.empty())
    return Result(Result::FETCH_ERROR);

  // Look for model
  auto modelIter = this->dataPtr->cache->MatchingModel(id);

  if (!modelIter)
    return Result(Result::FETCH_ERROR);

  auto modelPath = modelIter.PathToModel();

  // Check if file exists
  filePath = common::joinPaths(modelPath, filePath);

  if (common::exists(filePath))
  {
    _path = filePath;
    return Result(Result::FETCH_ALREADY_EXISTS);
  }

  return Result(Result::FETCH_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::CachedWorldFile(const common::URI &_fileUrl,
  std::string &_path)
{
  // Get data from URL
  WorldIdentifier id;
  std::string filePath;
  if (!this->ParseWorldFileUrl(_fileUrl, id, filePath))
    return Result(Result::FETCH_ERROR);

  if (filePath.empty())
    return Result(Result::FETCH_ERROR);

  // Look for world
  auto success = this->dataPtr->cache->MatchingWorld(id);

  if (!success)
    return Result(Result::FETCH_ERROR);

  auto worldPath = id.LocalPath();

  // Check if file exists
  filePath = common::joinPaths(worldPath, filePath);

  if (common::exists(filePath))
  {
    _path = filePath;
    return Result(Result::FETCH_ALREADY_EXISTS);
  }

  return Result(Result::FETCH_ERROR);
}


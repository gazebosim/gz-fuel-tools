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

using namespace ignition;
using namespace fuel_tools;

/// \brief Private Implementation
class ignition::fuel_tools::FuelClientPrivate
{
  /// \brief A model URL.
  /// E.g.: https://api.ignitionfuel.org/1.0/caguero/models/Beer
  public: const std::string kModelURLRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // Version
    "([^\\/\\s]+)\\/+"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "models"
    "models\\/+"
    // Name
    "([^\\/]+)\\/*"};

  /// \brief A model unique name, which is the same as the URL but without the
  /// API version.
  /// E.g.: https://api.ignitionfuel.org/caguero/models/Beer
  public: const std::string kModelUniqueNameRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "models"
    "models\\/+"
    // Name
    "([^\\/]+)\\/*"};

  /// \brief Client configuration
  public: ClientConfig config;

  /// \brief RESTful client
  public: REST rest;

  /// \brief Local Cache
  public: std::shared_ptr<LocalCache> cache;

  /// \brief Regex to parse Ignition Fuel model URLs.
  public: std::unique_ptr<std::regex> urlModelRegex;

  /// \brief Regex to parse Ignition Fuel model unique names.
  public: std::unique_ptr<std::regex> uniqueNameModelRegex;

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
    this->dataPtr->kModelURLRegexStr));
  this->dataPtr->uniqueNameModelRegex.reset(new std::regex(
    this->dataPtr->kModelUniqueNameRegexStr));
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

  auto serverURL = _id.Server().URL();
  auto version = _id.Server().Version();
  auto path = ignition::common::joinPaths(_id.Owner(), "models", _id.Name());

  resp = rest.Request(REST::GET, serverURL, version, path, {}, {}, "");
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
    ignwarn << "Failed to fetch models from server, returning cached models\n";
    return this->dataPtr->cache->AllModels();
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
    ignwarn << "Failed to fetch models from server, returning cached models\n";
    return this->dataPtr->cache->AllModels();
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

  // Local path
  auto path = ignition::common::joinPaths(_id.Owner(),
      "models", _id.Name() + ".zip");

  // Request
  ignition::fuel_tools::REST rest;
  RESTResponse resp;
  resp = rest.Request(REST::GET, _id.Server().URL(), _id.Server().Version(),
      path, {}, {}, "");
  if (resp.statusCode != 200)
    return Result(Result::FETCH_ERROR);

  // Save
  if (!this->dataPtr->cache->SaveModel(_id, resp.data, true))
    return Result(Result::FETCH_ERROR);

  return Result(Result::FETCH);
}

//////////////////////////////////////////////////
bool FuelClient::ParseModelURL(const std::string &_modelURL,
  ServerConfig &/*_server*/, ModelIdentifier &_id)
{
  std::smatch match;
  std::string scheme;
  std::string server;
  std::string version;
  std::string owner;
  std::string name;

  // Try URL first
  if (std::regex_match(_modelURL, match, *this->dataPtr->urlModelRegex) &&
      match.size() == 6u)
  {
    scheme = match[1];
    server = match[2];
    version = match[3];
    owner = match[4];
    name = match[5];
  }
  // Then try unique name
  else if (std::regex_match(_modelURL, match,
      *this->dataPtr->uniqueNameModelRegex) && match.size() == 5u)
  {
    scheme = match[1];
    server = match[2];
    owner = match[3];
    name = match[4];
  }
  else
  {
    ignerr << "Invalid URL [" << _modelURL << "]" << std::endl;
    return false;
  }

  // Get remaining server information from config
  _id.Server().URL(scheme + "://" + server);
  _id.Server().Version(version);
  for (const auto &s : this->dataPtr->config.Servers())
  {
    if (s.URL() == _id.Server().URL())
    {
      if (!version.empty() && s.Version() != _id.Server().Version())
      {
        ignwarn << "Requested server API version [" << version
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
  _id.Name(name);

  return true;
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const std::string &_modelURL,
  std::string &_path)
{
  // Get data from URL
  ModelIdentifier id;
  ServerConfig srv;
  if (!this->ParseModelURL(_modelURL, srv, id))
  {
    return Result(Result::FETCH_ERROR);
  }

  // Download
  auto result = this->DownloadModel(srv, id);
  if (result)
  {
    _path = ignition::common::joinPaths(this->Config().CacheLocation(),
        id.Server().Url().Path().Str(), id.Owner(), "models", id.Name());
  }

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


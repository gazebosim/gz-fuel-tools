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
  public: std::string kModelUrlRegexStr =
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
    "([^\\/]+)\\/*";

  /// \brief Client configuration
  public: ClientConfig config;

  /// \brief RESTful client
  public: REST rest;

  /// \brief Local Cache
  public: std::shared_ptr<LocalCache> cache;

  /// \brief Regex to parse Ignition Fuel URLs.
  public: std::unique_ptr<std::regex> urlModelRegex;

  /// \brief The path where the configuration file is located.
  public: std::string configPath;
};

//////////////////////////////////////////////////
FuelClient::FuelClient()
  : dataPtr(new FuelClientPrivate)
{
  this->dataPtr->cache.reset(new LocalCache(&(this->dataPtr->config)));
  this->dataPtr->urlModelRegex.reset(new std::regex(
    this->dataPtr->kModelUrlRegexStr));
  this->dataPtr->rest.SetUserAgent(this->dataPtr->config.UserAgent());
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
Result FuelClient::ModelDetails(const ServerConfig &_server,
  const ModelIdentifier &_id, ModelIdentifier &_model) const
{
  ignition::fuel_tools::REST rest;
  RESTResponse resp;

  auto serverUrl = _server.Url();
  auto version = _server.Version();
  auto path = ignition::common::joinPaths(_id.Owner(), "models", _id.Name());

  resp = rest.Request(REST::GET, serverUrl, version, path, {}, {}, "");
  if (resp.statusCode != 200)
    return Result(Result::FETCH_ERROR);

  _model = JSONParser::ParseModel(resp.data, _server);

  return Result(Result::FETCH);
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
ModelIter FuelClient::Models(const ServerConfig &_server,
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

  return ModelIterFactory::Create(this->dataPtr->rest, _server, path);
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
Result FuelClient::DownloadModel(const ServerConfig &_server,
  const ModelIdentifier &_id)
{
  ignition::fuel_tools::REST rest;
  RESTResponse resp;

  auto serverUrl = _server.Url();
  auto version = _server.Version();
  auto path = ignition::common::joinPaths(_id.Owner(), "models",
    _id.Name() + ".zip");

  resp = rest.Request(REST::GET, serverUrl, version, path, {}, {}, "");
  if (resp.statusCode != 200)
    return Result(Result::FETCH_ERROR);

  if (!this->dataPtr->cache->SaveModel(_id, resp.data, true))
    return Result(Result::FETCH_ERROR);

  return Result(Result::FETCH);
}

//////////////////////////////////////////////////
bool FuelClient::ParseModelURL(const std::string &_modelUrl,
  ServerConfig &_srv, ModelIdentifier &_id)
{
  std::smatch match;
  if (!std::regex_match(_modelUrl, match, *this->dataPtr->urlModelRegex))
    return false;

  if (match.size() != 6u)
    return false;

  std::string method = match[1];
  std::string server = match[2];
  std::string owner = match[4];
  std::string name = match[5];

  _id.Owner(owner);
  _id.Name(name);

  _srv.SetUrl(method + "://" + server);

  return true;
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const std::string &_modelUrl,
  std::string &_path)
{
  ModelIdentifier id;
  ServerConfig srv;
  if (!this->ParseModelURL(_modelUrl, srv, id))
  {
    return Result(Result::FETCH_ERROR);
  }

  auto result = this->DownloadModel(srv, id);
  if (result)
  {
    // Convert name to lowercase.
    std::string name = id.Name();
    std::string owner = id.Owner();
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    name = common::replaceAll(name, " ", "_");
    _path = ignition::common::joinPaths(this->Config().CacheLocation(),
      "models", owner, name);
  }

  return result;
}

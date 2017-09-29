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

#include <iostream>
#include <memory>
#include <ignition/common/Console.hh>

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/FuelClient.hh"
#include "ignition/fuel-tools/JSONParser.hh"
#include "ignition/fuel-tools/LocalCache.hh"
#include "ignition/fuel-tools/ModelIdentifier.hh"
#include "ignition/fuel-tools/REST.hh"
#include "ignition/fuel-tools/ModelIterPrivate.hh"


namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


/// \brief Private Implementation
class ignft::FuelClientPrivate
{
  /// \brief Client configuration
  public: ClientConfig config;

  /// \brief RESTful client
  public: REST rest;

  /// \brief Local Cache
  public: std::shared_ptr<LocalCache> cache;
};


//////////////////////////////////////////////////
FuelClient::FuelClient(const ClientConfig &_config, const REST &_rest,
    LocalCache *_cache)
  : dataPtr(new FuelClientPrivate)
{
  this->dataPtr->config = _config;
  this->dataPtr->rest = _rest;

  if (nullptr == _cache)
    this->dataPtr->cache.reset(new LocalCache(&(this->dataPtr->config)));
  else
    this->dataPtr->cache.reset(_cache);
}

//////////////////////////////////////////////////
FuelClient::~FuelClient()
{
}

//////////////////////////////////////////////////
Result FuelClient::ModelDetails(const ModelIdentifier &_id,
  ModelIdentifier &_model) const
{
  ignition::fuel_tools::REST rest;
  RESTResponse resp;

  // ToDo: Check all servers.
  auto servers = this->dataPtr->config.Servers();
  if (servers.empty())
  {
    ignerr << "No servers found" << std::endl;
    return Result(Result::FETCH_ERROR);
  }

  auto serverURL = servers.front().URL();
  auto path = "/1.0/" + _id.Owner() + "/models/" + _id.Name();

  resp = rest.Request("GET", serverURL, path, {}, {}, "");
  if (resp.statusCode != 200)
    return Result(Result::FETCH_ERROR);

  _model = JSONParser::ParseModel(resp.data);

  return Result(Result::FETCH);
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models()
{
  ModelIter iter = ModelIterFactory::Create(this->dataPtr->rest,
      this->dataPtr->config, "/1.0/models");

  if (!iter)
  {
    // Return just the cached models
    ignwarn << "Failed to fetch models from server, returning cached models\n";
    return this->dataPtr->cache->AllModels();
  }
  return iter;
}

//////////////////////////////////////////////////
//ModelIter FuelClient::Models(const ModelIdentifier &_id)
//{
//  // Check local cache first
//  ModelIter localIter = this->dataPtr->cache->MatchingModels(_id);
//  if (localIter)
//    return localIter;
//
//  ignmsg << _id.UniqueName() << " not found in cache, attempting download\n";
//
//  // Todo try to fetch model directly from a server
//  auto path = "/1.0/" + _id.Owner() + "/models/" + _id.Name();
//
//  return ModelIterFactory::Create(this->dataPtr->rest,
//      this->dataPtr->config, path);
//}

//////////////////////////////////////////////////
Result FuelClient::UploadModel(const std::string &_pathToModelDir,
    const ModelIdentifier &_id)
{
  // TODO Upload a model and return an Result
  return Result(Result::UPLOAD_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::DeleteModel(const ModelIdentifier &_id)
{
  // TODO Delete a model and return a Result
  return Result(Result::DELETE_ERROR);
}

//////////////////////////////////////////////////
Result FuelClient::DownloadModel(const ModelIdentifier &_id)
{
  ignition::fuel_tools::REST rest;
  RESTResponse resp;

  // ToDo: Check all servers.
  auto servers = this->dataPtr->config.Servers();
  if (servers.empty())
  {
    ignerr << "No servers found" << std::endl;
    return Result(Result::FETCH_ERROR);
  }

  auto serverURL = servers.front().URL();
  auto path = "/1.0/" + _id.Owner() + "/models/" + _id.Name() + ".zip";

  resp = rest.Request("GET", serverURL, path, {}, {}, "");
  if (resp.statusCode != 200)
    return Result(Result::FETCH_ERROR);

  if (!this->dataPtr->cache->SaveModel(_id, resp.data, true))
    return Result(Result::FETCH_ERROR);

  return Result(Result::FETCH);
}

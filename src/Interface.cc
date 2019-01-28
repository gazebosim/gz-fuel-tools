/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include "ignition/fuel_tools/Interface.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"

namespace ignition
{
  namespace fuel_tools
  {
    //////////////////////////////////////////////
    std::string fetchResource(const std::string &_uri)
    {
      ignition::fuel_tools::FuelClient client;
      return fetchResourceWithClient(_uri, client);
    }

    //////////////////////////////////////////////
    std::string fetchResourceWithClient(const std::string &_uri,
        ignition::fuel_tools::FuelClient &_client)
    {
      std::string result;

      ignition::fuel_tools::ModelIdentifier model;
      ignition::fuel_tools::WorldIdentifier world;
      ignition::common::URI uri(_uri);
      // Download the model, if it is a model URI
      if (_client.ParseModelUrl(uri, model) &&
          !_client.CachedModel(uri, result))
      {
        _client.DownloadModel(uri, result);
      }
      // Download the world, if it is a world URI
      else if (_client.ParseWorldUrl(uri, world) &&
               !_client.CachedWorld(uri, result))
      {
        _client.DownloadWorld(uri, result);
      }

      return result;
    }
  }
}

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

#include "gz/common/Console.hh"
#include "gz/fuel_tools/Interface.hh"
#include "gz/fuel_tools/WorldIdentifier.hh"

namespace ignition
{
  namespace fuel_tools
  {
    //////////////////////////////////////////////
    std::string fetchResource(const std::string &_uri)
    {
      gz::fuel_tools::FuelClient client;
      return fetchResourceWithClient(_uri, client);
    }

    //////////////////////////////////////////////
    std::string fetchResourceWithClient(const std::string &_uri,
        gz::fuel_tools::FuelClient &_client)
    {
      std::string result;

      gz::fuel_tools::ModelIdentifier model;
      gz::fuel_tools::WorldIdentifier world;
      std::string fileUrl;
      gz::common::URI uri(_uri);
      // Download the model, if it is a model URI
      if (_client.ParseModelUrl(uri, model) &&
          !_client.CachedModel(uri, result))
      {
        _client.DownloadModel(uri, result);
      }
      // Download the model, if it's a model file URI
      else if (_client.ParseModelFileUrl(uri, model, fileUrl) &&
          !_client.CachedModelFile(uri, result))
      {
        auto modelUri = _uri.substr(0,
            _uri.find("files", model.UniqueName().size())-1);
        _client.DownloadModel(common::URI(modelUri), result);
        result = common::joinPaths(result, fileUrl);
      }
      // Download the world, if it is a world URI
      else if (_client.ParseWorldUrl(uri, world) &&
               !_client.CachedWorld(uri, result))
      {
        _client.DownloadWorld(uri, result);
      }
      // Download the world, if it's a world file URI
      else if (_client.ParseWorldFileUrl(uri, world, fileUrl) &&
          !_client.CachedWorldFile(uri, result))
      {
        auto worldUri = _uri.substr(0,
            _uri.find("files", world.UniqueName().size())-1);
        _client.DownloadWorld(common::URI(worldUri), result);
        result = common::joinPaths(result, fileUrl);

      }

      return result;
    }
  }
}

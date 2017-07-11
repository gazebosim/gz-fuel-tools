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


#include <ignition/fuel-tools/FuelClient.hh>
#include <ignition/fuel-tools/REST.hh>
#include <ignition/fuel-tools/ModelIterPrivate.hh>


namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


/// \brief Private Implementation
class ignft::FuelClientPrivate
{
  /// \brief Client configuration
  public: ClientConfig config;
};


//////////////////////////////////////////////////
FuelClient::FuelClient(const ClientConfig &_config)
  : dataPtr(new FuelClientPrivate)
{
  this->dataPtr->config = _config;
}

//////////////////////////////////////////////////
FuelClient::~FuelClient()
{
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models()
{
  // TODO fetch from all servers and combine result, not just one server
  auto servers = this->dataPtr->config.Servers();
  if (!servers.empty())
  {
    std::string protocol = "GET";
    auto serverURL = servers.front();
    std::string path = "/1.0/models";
    std::vector<std::string> headers =  {"Accept: application/json"};

    RESTResponse resp = REST::Request("GET", serverURL, path, {}, headers, "");

    if (resp.statusCode != 200)
    {
      // TODO throw Result complaining of bad response from server?
      std::unique_ptr<ModelIterPrivate> noResults(new ModelIterPrivate);
      return std::move(ModelIter(std::move(noResults)));
    }

    std::cerr << "Got response [" << resp.data << "]\n";
  }

  std::unique_ptr<ModelIterPrivate> results(new ModelIterPrivate);
  return std::move(ModelIter(std::move(results)));
}

//////////////////////////////////////////////////
ModelIter FuelClient::Models(const ModelIdentifier &_id)
{
  // Todo Return ModelIter set to fetch models matching ModelIdentifier
}

//////////////////////////////////////////////////
Result FuelClient::UploadModel(const std::string &_pathToModelDir,
    const ModelIdentifier &_id)
{
  // TODO Upload a model and return an Result
}

//////////////////////////////////////////////////
Result FuelClient::DeleteModel(const ModelIdentifier &_id)
{
  // TODO Delete a model and return a Result
}

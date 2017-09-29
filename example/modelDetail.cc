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

#include <ignition/fuel-tools.hh>

int main(int argc, char **argv)
{
  // Create a ClientConfig, TODO create this from a yaml file
  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL("https://localhost:4430");
  srv.LocalName("ignitionfuel");
  conf.AddServer(srv);

  // Remember to set your IGN_FUEL_PATH.
  // E.g.: export IGN_FUEL_PATH=~/.ignition/fuel/models
  ignition::fuel_tools::FuelClient client(conf);

  // Set the properties of the model that we want to download.
  ignition::fuel_tools::ModelIdentifier modelIdentifier;
  modelIdentifier.Owner("anonymous");
  modelIdentifier.Name("Cabinet");

  // Fetch the model.
  ignition::fuel_tools::ModelIdentifier model;
  auto result = client.ModelDetails(modelIdentifier, model);

  // Show model details.
  if (result)
  {
    std::cout << "Name: " << model.Name() << std::endl;
    std::cout << "Owner: " << model.Owner() << std::endl;
  }
}

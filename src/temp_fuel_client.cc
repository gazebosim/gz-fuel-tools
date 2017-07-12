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

#include <ignition/common/Console.hh>
#include <ignition/fuel-tools/FuelClient.hh>
#include <ignition/fuel-tools/LocalCache.hh>

int main()
{
  ignition::common::Console::SetVerbosity(4);

  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8001/");
  srv.LocalName("local");
  conf.AddServer(srv);

  conf.CacheLocation("/build/ign-fuel-tools");

  ignition::fuel_tools::FuelClient client(conf);
  auto iter = client.Models();

  // TODO LocalCache should be used inside FuelClient
  ignition::fuel_tools::LocalCache cache(&conf);

  auto localIter = cache.AllModels();
  while (localIter)
  {
    ignmsg << "Model: " << localIter->Identification().Name() << "\n";
    ++localIter;
  }
  ignmsg << "exiting\n";
  return 0;
}

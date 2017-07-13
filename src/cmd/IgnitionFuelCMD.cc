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

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <ignition/fuel-tools/Helpers.hh>
#include <ignition/fuel-tools/FuelClient.hh>


/// \brief Read a config file from disk
ignition::fuel_tools::ClientConfig IGNITION_FUEL_TOOLS_HIDDEN getConfig()
{
  // TODO READ config file, don't hardcode this
  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8001/");
  srv.LocalName("local");
  conf.AddServer(srv);

  if(const char* env_p = std::getenv("IGN_FUEL_CACHE"))
    conf.CacheLocation(env_p);
  else
  {
    // TODO Get home directory
    conf.CacheLocation("~/.ignition/fuel/");
  }
  return conf;
}

/// \brief Print models from a ModelIter to stdout
void IGNITION_FUEL_TOOLS_HIDDEN printModels(
    ignition::fuel_tools::ModelIter _iter)
{
  // print out the models
  while (_iter)
  {
    std::cout << _iter->Identification().UniqueName() << "\n";
    ++_iter;
  }
}

extern "C"
{
  /// \brief Prints "Hello World!\n" to STDOUT
  void IGNITION_FUEL_TOOLS_VISIBLE helloWorld()
  {
    std::cout << "Hello world!\n";
  }

  /// \brief Lists Models
  /// \param[in] _name only list models with this name
  /// \param[in] _owner only list models from this owner
  /// \param[in] _url only list models from this server
  void IGNITION_FUEL_TOOLS_VISIBLE listModels(char *_name, char *_owner,
      char *_url)
  {
    auto conf = getConfig();
    ignition::fuel_tools::FuelClient client(conf);

    if (!std::strlen(_name) && !std::strlen(_owner) && !std::strlen(_url))
    {
      // All models
      printModels(client.Models());
    }
    else
    {
      // Get some models
      ignition::fuel_tools::ModelIdentifier id;
      if (std::strlen(_name))
        id.Name(_name);
      if (std::strlen(_owner))
        id.Owner(_owner);
      if (std::strlen(_url))
        id.SourceURL(_url);
      printModels(client.Models(id));
    }
  }
}

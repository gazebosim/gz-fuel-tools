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

#include <chrono>
#include <iostream>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/config.hh"
#include "ignition/fuel_tools/Helpers.hh"
#include "ignition/fuel_tools/ign.hh"
#include "ignition/fuel_tools/FuelClient.hh"

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE char *ignitionVersion()
{
  return strdup(IGNITION_FUEL_TOOLS_VERSION_FULL);
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE void listModels(const char *_url)
{
  std::string url{_url};

  // Client
  ignition::fuel_tools::ClientConfig conf;
  if (!url.empty())
  {
    ignition::fuel_tools::ServerConfig serverConf;
    serverConf.URL(url);
    conf.AddServer(serverConf);
  }
  else
  {
    conf.LoadConfig();
  }

  ignition::fuel_tools::FuelClient client(conf);

  // Get models
  for (auto server : conf.Servers())
  {
    std::cout << "Fetching model list from " << server.URL() << "..."
              << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    auto iter = client.Models(server);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);

    std::cout << "Received model list (took " << duration.count() << "ms)."
              << std::endl;

    // Rearrange by user
    // key: user name
    // value: vector of model names
    std::map<std::string, std::vector<std::string>> modelsMap;
    for (; iter; ++iter)
    {
      modelsMap[iter->Identification().Owner()].push_back(
          iter->Identification().Name());
    }

    // Print all models
    unsigned int ownerCount{0};
    unsigned int modelCount{0};
    std::cout << "\033[92m\033[1m" << server.URL() << "\033[39m\033[0m"
              << std::endl;
    for (auto owner = modelsMap.begin(); owner != modelsMap.end(); ++owner)
    {
      ownerCount++;

      // Owner indentation
      bool lastOwner = owner == std::prev(modelsMap.end());
      if (lastOwner)
        std::cout << "└──";
      else
        std::cout << "├──";

      // Owner name
      std::cout << " \033[93m\033[1m" << owner->first << "\033[39m\033[0m"
                << std::endl;

      // Models
      for (auto model = owner->second.begin(); model != owner->second.end();
          ++model)
      {
        modelCount++;

        // Model indentation
        if (lastOwner)
          std::cout << "    ";
        else
          std::cout << "│   ";

        if (model == std::prev(owner->second.end()))
          std::cout << "└── ";
        else
          std::cout << "├── ";

        // Model name
        std::cout << *model << std::endl;
      }
    }

    std::cout << "\033[36m" << ownerCount << " owners, " << modelCount
              << " models\033[39m" << std::endl;
  }
}


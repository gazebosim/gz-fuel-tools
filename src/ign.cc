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

#include <curl/curl.h>
#include <string.h>
#ifdef _WIN32
// DELETE is defined in winnt.h and causes a problem with REST::DELETE
#undef DELETE
#endif

#include <algorithm>
#include <chrono>
#include <iostream>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/config.hh"
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/Helpers.hh"
#include "ignition/fuel_tools/ign.hh"

//////////////////////////////////////////////////
/// \brief Print resources in a human readable manner
/// \param[in] _serverConfig Server configuration
/// \param[in] _resourceMap Key is owner name, value is vector of resources
/// \param[in] _resourceType Type, such as "models"
extern "C" void prettyPrint(
    const ignition::fuel_tools::ServerConfig &_serverConfig,
    std::map<std::string, std::vector<std::string>> _resourceMap,
    const std::string &_resourceType)
{
  unsigned int ownerCount{0};
  unsigned int resourceCount{0};
  std::cout << "\033[92m\033[1m" << _serverConfig.Url().Str() << "\033[39m\033[0m"
            << std::endl;
  for (auto owner = _resourceMap.begin(); owner != _resourceMap.end(); ++owner)
  {
    ownerCount++;

    // Owner indentation
    bool lastOwner = owner == std::prev(_resourceMap.end());
    if (lastOwner)
      std::cout << "└──";
    else
      std::cout << "├──";

    // Owner name
    std::cout << " \033[93m\033[1m" << owner->first << "\033[39m\033[0m"
              << std::endl;

    // Resources
    for (auto resource = owner->second.begin(); resource != owner->second.end();
        ++resource)
    {
      resourceCount++;

      // Resource indentation
      if (lastOwner)
        std::cout << "    ";
      else
        std::cout << "│   ";

      if (resource == std::prev(owner->second.end()))
        std::cout << "└── ";
      else
        std::cout << "├── ";

      // Resource name
      std::cout << *resource << std::endl;
    }
  }

  std::cout << "\033[36m" << ownerCount << " owners, " << resourceCount
            << " " << _resourceType << "\033[39m" << std::endl;
}

//////////////////////////////////////////////////
/// \brief Print resources in a machine readable manner
/// \param[in] _serverConfig Server configuration
/// \param[in] _resourceMap Key is owner name, value is vector of resources
/// \param[in] _resourceType Type, such as "models"
extern "C" void uglyPrint(
    const ignition::fuel_tools::ServerConfig &_serverConfig,
    std::map<std::string, std::vector<std::string>> _resourceMap,
    const std::string &_resourceType)
{
  CURL *curl = curl_easy_init();
  for (auto owner = _resourceMap.begin(); owner != _resourceMap.end(); ++owner)
  {
    for (auto resource : owner->second)
    {
      auto encodedRes =
          curl_easy_escape(curl, resource.c_str(), resource.size());

      std::cout << _serverConfig.Url().Str() << "/" << _serverConfig.Version()
                << "/" << owner->first << "/" << _resourceType << "/"
                << std::string(encodedRes) << std::endl;
    }
  }
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE char *ignitionVersion()
{
  return strdup(IGNITION_FUEL_TOOLS_VERSION_FULL);
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int listModels(const char *_url,
    const char *_raw)
{
  std::string url{_url};
  std::string rawStr{_raw};
  std::transform(rawStr.begin(), rawStr.end(),
                 rawStr.begin(), ::tolower);
  bool pretty = rawStr != "true";

  // Client
  ignition::fuel_tools::ClientConfig conf;
  if (!url.empty())
  {
    ignition::fuel_tools::ServerConfig serverConf;
    serverConf.SetUrl(ignition::common::URI(url));
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
    if (pretty)
    {
      std::cout << "Fetching model list from " << server.Url().Str() << "..."
                << std::endl;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    auto iter = client.Models(server);

    if (!iter)
    {
      std::cout <<
          "Either failed to fetch model list, or server has no models yet."
          << std::endl;
      return false;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);

    if (pretty)
    {
      std::cout << "Received model list (took " << duration.count() << "ms)."
                << std::endl;
    }

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
    if (pretty)
      prettyPrint(server, modelsMap, "models");
    else
      uglyPrint(server, modelsMap, "models");
  }

  return true;
}


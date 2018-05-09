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
#ifdef _WIN32
// DELETE is defined in winnt.h and causes a problem with REST::DELETE
#undef DELETE
#endif

#include <algorithm>
#include <chrono>
#include <iostream>

#include <ignition/common/Console.hh>
#include <ignition/common/URI.hh>

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
    const ignition::fuel_tools::ServerConfig _serverConfig,
    std::map<std::string, std::vector<std::string>> _resourceMap,
    const std::string &_resourceType)
{
  unsigned int ownerCount{0};
  unsigned int resourceCount{0};
  std::cout << "\033[92m\033[1m" << _serverConfig.URL() << "\033[39m\033[0m"
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
    const ignition::fuel_tools::ServerConfig _serverConfig,
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

      std::cout << _serverConfig.URL() << "/" << _serverConfig.Version() << "/"
                << owner->first << "/" << _resourceType << "/"
                << std::string(encodedRes) << std::endl;
    }
  }
}

//////////////////////////////////////////////////
/// \brief Fill a map with all models from a server
/// \param[in] _client Fuel client
/// \param[in] _server Server configuration
/// \param[out] _resourceMap Key is owner name, value is vector of resources
/// \return True if successful, will fail if there's a server error or if the
/// server has no models yet.
extern "C" bool getAllModels(
    const ignition::fuel_tools::FuelClient &_client,
    const ignition::fuel_tools::ServerConfig &_server,
    std::map<std::string, std::vector<std::string>> &_resourceMap)
{
  auto iter = _client.Models(_server);

  if (!iter)
  {
    std::cout <<
        "Either failed to fetch model list, or server has no models yet."
        << std::endl;
    return false;
  }

  // Rearrange by user
  // key: user name
  // value: vector of model names
  for (; iter; ++iter)
  {
    _resourceMap[iter->Identification().Owner()].push_back(
        iter->Identification().Name());
  }

  return true;
}

//////////////////////////////////////////////////
/// \brief Fill a map with all models from an owner
/// \param[in] _client Fuel client
/// \param[in] _modelId Identifier for models to be returned
/// \param[out] _resourceMap Key is owner name, value is vector of resources
/// \return True if successful, will fail if there's a server error or if the
/// server has no models yet.
extern "C" bool getOwnerModels(
    const ignition::fuel_tools::FuelClient &_client,
    const ignition::fuel_tools::ModelIdentifier &_modelId,
    std::map<std::string, std::vector<std::string>> &_resourceMap)
{
  // Dummy server config not used, will be deprecated on version 2
  ignition::fuel_tools::ServerConfig server;
  auto iter = _client.Models(server, _modelId);

  if (!iter)
  {
    std::cout <<
        "Either failed to fetch model list, or server has no models yet."
        << std::endl;
    return false;
  }

  // Rearrange by user
  // key: user name
  // value: vector of model names
  for (; iter; ++iter)
  {
    _resourceMap[iter->Identification().Owner()].push_back(
        iter->Identification().Name());
  }

  return true;
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE char *ignitionVersion()
{
  return strdup(IGNITION_FUEL_TOOLS_VERSION_FULL);
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int listModels(const char *_url,
    const char *_owner, const char *_raw)
{
  std::string url{_url};
  std::string owner{_owner};
  std::string rawStr{_raw};
  std::transform(rawStr.begin(), rawStr.end(),
                 rawStr.begin(), ::tolower);
  bool pretty = rawStr != "true";

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

  conf.SetUserAgent("FuelTools " IGNITION_FUEL_TOOLS_VERSION_FULL);

  // Filter
  ignition::fuel_tools::ModelIdentifier modelId;
  if (!owner.empty())
    modelId.Owner(owner);

  ignition::fuel_tools::FuelClient client(conf);

  // Get models
  for (auto server : conf.Servers())
  {
    modelId.Server(server);

    if (pretty)
    {
      std::cout << "Fetching model list from " << server.URL() << "..."
                << std::endl;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    std::map<std::string, std::vector<std::string>> modelsMap;

    // All models
    if (owner.empty())
    {
      if (!getAllModels(client, server, modelsMap))
        return false;
    }
    else
    {
      if (!getOwnerModels(client, modelId, modelsMap))
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

    // Print all models
    if (pretty)
      prettyPrint(server, modelsMap, "models");
    else
      uglyPrint(server, modelsMap, "models");
  }

  return true;
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int downloadUrl(const char *_url)
{
  std::string urlStr{_url};
  ignition::common::URI url(urlStr);
  if (!url.Valid())
  {
    std::cout << "Download failed: Malformed URL" << std::endl;
    return false;
  }

  // Client
  ignition::fuel_tools::ClientConfig conf;
  conf.LoadConfig();
  conf.SetUserAgent("FuelTools " IGNITION_FUEL_TOOLS_VERSION_FULL);

  ignition::fuel_tools::FuelClient client(conf);

  // Model?
  ignition::fuel_tools::ModelIdentifier model;
  if (client.ParseModelUrl(url, model))
  {
    // Download
    std::cout << "Downloading model: " << "\033[36m" << std::endl
              << model.AsString("  ") << "\033[39m" << std::endl;

    ignition::fuel_tools::ServerConfig srv;
    auto result = client.DownloadModel(srv, model);

    if (!result)
    {
      std::cout << "Download failed." << std::endl;
      return false;
    }
  }
  else
  {
    std::cout << "Invalid URL: only models can be downloaded so far."
              << std::endl;
    return false;
  }

  std::cout << "Download succeeded." << std::endl;
  return true;
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE void cmdVerbose(const char *_verbosity)
{
  ignition::common::Console::SetVerbosity(std::atoi(_verbosity));
}


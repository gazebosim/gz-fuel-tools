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
#include <csignal>
#include <exception>
#ifdef _WIN32
// DELETE is defined in winnt.h and causes a problem with REST::DELETE
#undef DELETE
#endif

#include <algorithm>
#include <chrono>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/SignalHandler.hh>
#include <ignition/common/URI.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/CollectionIdentifier.hh"
#include "ignition/fuel_tools/config.hh"
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/Helpers.hh"
#include "ign.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"

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
  std::cout << "\033[92m\033[1m" << _serverConfig.Url().Str()
            << "\033[39m\033[0m" << std::endl;
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

      std::cout << _serverConfig.Url().Str() << "/" << _serverConfig.Version()
                << "/" << owner->first << "/" << _resourceType << "/"
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
/// \brief Fill a map with all worlds from a server
/// \param[in] _client Fuel client
/// \param[in] _server Server configuration
/// \param[out] _resourceMap Key is owner name, value is vector of resources
/// \return True if successful, will fail if there's a server error or if the
/// server has no worlds yet.
extern "C" bool getAllWorlds(
    const ignition::fuel_tools::FuelClient &_client,
    const ignition::fuel_tools::ServerConfig &_server,
    std::map<std::string, std::vector<std::string>> &_resourceMap)
{
  auto iter = _client.Worlds(_server);

  if (!iter)
  {
    std::cout <<
        "Either failed to fetch world list, or server has no worlds yet."
        << std::endl;
    return false;
  }

  // Rearrange by user
  // key: user name
  // value: vector of world names
  for (; iter; ++iter)
  {
    _resourceMap[iter->Owner()].push_back(iter->Name());
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
  ignition::fuel_tools::ModelIter iter = _client.Models(_modelId);

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
/// \brief Fill a map with all worlds from an owner
/// \param[in] _client Fuel client
/// \param[in] _worldId Identifier for worlds to be returned
/// \param[out] _resourceMap Key is owner name, value is vector of resources
/// \return True if successful, will fail if there's a server error or if the
/// server has no worlds yet.
extern "C" bool getOwnerWorlds(
    const ignition::fuel_tools::FuelClient &_client,
    const ignition::fuel_tools::WorldIdentifier &_worldId,
    std::map<std::string, std::vector<std::string>> &_resourceMap)
{
  auto iter = _client.Worlds(_worldId);

  if (!iter)
  {
    std::cout <<
        "Either failed to fetch world list, or server has no worlds yet."
        << std::endl;
    return false;
  }

  // Rearrange by user
  // key: user name
  // value: vector of world names
  for (; iter; ++iter)
  {
    _resourceMap[iter->Owner()].push_back(iter->Name());
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
    const char *_owner, const char *_raw, const char *_configFile)
{
  std::string urlStr{_url};
  if (!urlStr.empty() && !ignition::common::URI::Valid(_url))
  {
    std::cout << "Invalid URL [" << _url << "]" << std::endl;
    return 0;
  }

  ignition::common::URI url(urlStr);
  std::string owner{_owner};
  std::string rawStr{_raw};
  std::transform(rawStr.begin(), rawStr.end(),
                 rawStr.begin(), ::tolower);
  bool pretty = rawStr != "true";

  // Client
  ignition::fuel_tools::ClientConfig conf;
  if (_configFile && strlen(_configFile) > 0)
  {
    conf.Clear();
    conf.LoadConfig(_configFile);
  }

  if (url.Valid())
  {
    conf.Clear();
    ignition::fuel_tools::ServerConfig serverConf;
    serverConf.SetUrl(ignition::common::URI(url));
    conf.AddServer(serverConf);
  }

  conf.SetUserAgent("FuelTools " IGNITION_FUEL_TOOLS_VERSION_FULL);

  // Filter
  ignition::fuel_tools::ModelIdentifier modelId;
  if (!owner.empty())
    modelId.SetOwner(owner);

  ignition::fuel_tools::FuelClient client(conf);

  // Get models
  for (auto server : conf.Servers())
  {
    modelId.SetServer(server);

    if (pretty)
    {
      std::cout << "Fetching model list from " << server.Url().Str() << "..."
                << std::endl;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    std::map<std::string, std::vector<std::string>> modelsMap;

    // All models
    if (owner.empty())
    {
      if (!getAllModels(client, server, modelsMap))
        continue;
    }
    else
    {
      if (!getOwnerModels(client, modelId, modelsMap))
        continue;
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
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int listWorlds(const char *_url,
    const char *_owner, const char *_raw, const char *_configFile)
{
  std::string urlStr{_url};
  if (!urlStr.empty() && !ignition::common::URI::Valid(_url))
  {
    std::cout << "Invalid URL [" << _url << "]" << std::endl;
    return 0;
  }

  ignition::common::URI url(urlStr);
  std::string owner{_owner};
  std::string rawStr{_raw};
  std::transform(rawStr.begin(), rawStr.end(),
                 rawStr.begin(), ::tolower);
  bool pretty = rawStr != "true";

  // Client
  ignition::fuel_tools::ClientConfig conf;
  if (_configFile && strlen(_configFile) > 0)
  {
    conf.Clear();
    conf.LoadConfig(_configFile);
  }

  if (url.Valid())
  {
    conf.Clear();
    ignition::fuel_tools::ServerConfig serverConf;
    serverConf.SetUrl(url);
    conf.AddServer(serverConf);
  }

  conf.SetUserAgent("FuelTools " IGNITION_FUEL_TOOLS_VERSION_FULL);

  // Filter
  ignition::fuel_tools::WorldIdentifier worldId;
  if (!owner.empty())
    worldId.SetOwner(owner);

  ignition::fuel_tools::FuelClient client(conf);

  // Get worlds
  for (auto server : conf.Servers())
  {
    worldId.SetServer(server);

    if (pretty)
    {
      std::cout << "Fetching world list from " << server.Url().Str() << "..."
                << std::endl;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    std::map<std::string, std::vector<std::string>> worldsMap;

    // All worlds
    if (owner.empty())
    {
      if (!getAllWorlds(client, server, worldsMap))
        continue;
    }
    else
    {
      if (!getOwnerWorlds(client, worldId, worldsMap))
        continue;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);

    if (pretty)
    {
      std::cout << "Received world list (took " << duration.count() << "ms)."
                << std::endl;
    }

    // Print all worlds
    if (pretty)
      prettyPrint(server, worldsMap, "worlds");
    else
      uglyPrint(server, worldsMap, "worlds");
  }

  return true;
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int downloadUrl(const char *_url,
    const char *_configFile, const char *_header, const char *_type, int _jobs)
{
  // Add signal handler for SIGTERM and SIGINT. Ctrl-C doesn't work without this
  // handler.
  ignition::common::SignalHandler sigHandler;
  sigHandler.AddCallback([&](int _sig) {
      if (SIGTERM == _sig || SIGINT == _sig)
      {
        std::exit(1);
      }
  });
  std::string urlStr{_url};
  ignition::common::URI url(urlStr);
  if (!url.Valid())
  {
    std::cout << "Download failed: Malformed URL" << std::endl;
    return false;
  }

  // Client
  ignition::fuel_tools::ClientConfig conf;
  if (_configFile && strlen(_configFile) > 0)
  {
    conf.Clear();
    conf.LoadConfig(_configFile);
  }

  conf.SetUserAgent("FuelTools " IGNITION_FUEL_TOOLS_VERSION_FULL);

  ignition::fuel_tools::FuelClient client(conf);
  ignition::fuel_tools::ModelIdentifier model;
  ignition::fuel_tools::WorldIdentifier world;
  ignition::fuel_tools::CollectionIdentifier collection;

  // Model?
  if (client.ParseModelUrl(url, model))
  {
    // Download
    if (ignition::common::Console::Verbosity() >= 3)
    {
      std::cout << "Downloading model: " << "\033[36m" << std::endl
                << model.AsPrettyString("  ") << "\033[39m" << std::endl;
    }

    if (model.Version() != 0)
    {
      ignwarn << "Requested version [" << model.VersionStr()  << "], but "
              << "currently only the model's latest (tip) version is supported."
              << std::endl;
    }

    int result = 0;
    if (_header && strlen(_header) > 0)
    {
      std::vector<std::string> headers;
      headers.push_back(_header);
      result = client.DownloadModel(model, headers);
    }
    else
    {
      result = client.DownloadModel(model);
    }

    if (!result)
    {
      std::cout << "Download failed." << std::endl;
      return false;
    }
  }
  // World?
  else if (client.ParseWorldUrl(url, world))
  {
    // Download
    if (ignition::common::Console::Verbosity() >= 3)
    {
      std::cout << "Downloading world: " << "\033[36m" << std::endl
                << world.AsPrettyString("  ") << "\033[39m" << std::endl;
    }

    if (world.Version() != 0)
    {
      ignwarn << "Requested version [" << world.VersionStr()  << "], but "
              << "currently only the world's latest (tip) version is supported."
              << std::endl;
    }

    ignition::fuel_tools::Result result = client.DownloadWorld(world);

    if (!result)
    {
      std::cout << "Download failed because " << result.ReadableResult()
        << std::endl;
      return false;
    }
  }
  // Collection?
  else if (client.ParseCollectionUrl(url, collection))
  {
    if (ignition::common::Console::Verbosity() >= 3)
    {
      std::cout << "Downloading collection: " << "\033[36m" << std::endl
                << collection.AsPrettyString("  ") << "\033[39m" << std::endl;
    }

    bool downloadModels = true;
    bool downloadWorlds = true;
    if (nullptr !=_type)
    {
      if (strcmp(_type, "model") == 0)
      {
        downloadWorlds = false;
      }
      else if (strcmp(_type, "world") == 0)
      {
        downloadModels = false;
      }
      else
      {
        ignerr << "Unknown resource type [" << _type << "] sepcified.\n";
        return false;
      }
    }

    std::vector<ignition::fuel_tools::ModelIdentifier> modelIds;
    std::vector<ignition::fuel_tools::WorldIdentifier> worldIds;

    if (downloadModels)
    {
      // Get list of model identifiers in collection
      auto modelsIter = client.Models(collection);
      for (; modelsIter; ++modelsIter)
      {
        modelIds.push_back(modelsIter->Identification());
      }
      ignmsg << "Found " << modelIds.size() << " models in collection ["
        << collection.Name() << "]" << std::endl;
    }

    if (downloadWorlds)
    {
      // Get list of world identifiers in collection
      auto worldIter = client.Worlds(collection);
      for (; worldIter; ++worldIter)
      {
        worldIds.push_back(worldIter);
      }
      ignmsg << "Found " << worldIds.size() << " worlds in collection ["
        << collection.Name() << "]" << std::endl;
    }

    const std::size_t totalItemCount = modelIds.size() + worldIds.size();
    if (totalItemCount == 0)
    {
      std::cout << "There are no items in collection [" << collection.Name()
        << "]" << std::endl;
      return false;
    }

    size_t jobs = std::max(1, _jobs);

    ignmsg << "Using " << jobs << " jobs to download collection of "
           << totalItemCount << " items" << std::endl;

    std::deque<std::future<ignition::fuel_tools::Result>> tasks;

    // Check for finished tasks by checking if the status of their futures is
    // "ready". If a task is finished, check if it succeeded and print out an
    // error message if it failed. When a task is finished, it gets erased from
    // the tasks list to make room for other tasks to be added.
    size_t itemCount = 0;
    auto checkForFinishedTasks = [&itemCount, &totalItemCount, &tasks] {
      auto finishedIt =
          std::partition(tasks.begin(), tasks.end(), [](const auto &_task)
              {
                return std::future_status::ready !=
                _task.wait_for(std::chrono::milliseconds(100));
              });

      if (finishedIt != tasks.end())
      {
        for (auto taskIt = finishedIt; taskIt != tasks.end(); ++taskIt)
        {
          ignition::fuel_tools::Result result = taskIt->get();
          if (result)
          {
            ++itemCount;
          }
          else
          {
            ignerr << result.ReadableResult() << std::endl;
          }
        }

        tasks.erase(finishedIt, tasks.end());
        ignmsg << "Downloaded: " << itemCount << " / " << totalItemCount
               << std::endl;
      }
    };

    // Here we use std::async to download items in parallel. The download task
    // is started asynchronously and gets added to the task list which is
    // monitored for completion.
    if (downloadModels)
    {
      for (const auto &modelId : modelIds)
      {
        // Check if any of the tasks are done. Don't start a new task until the
        // number of tasks in the tasks lists is below the number of jobs
        // specified by the user.
        while (tasks.size() >= jobs)
        {
          checkForFinishedTasks();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto handle = std::async(std::launch::async, [&modelId, &client]
            {
              return client.DownloadModel(modelId);
            });
        tasks.push_back(std::move(handle));
      }
    }

    if (downloadWorlds)
    {
      // We need a mutable worldId because DownloadWorld modifies it
      for (auto &worldId : worldIds)
      {
        // Check if any of the tasks are done
        while (tasks.size() >= jobs)
        {
          checkForFinishedTasks();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto handle = std::async(std::launch::async, [&worldId, &client]
            {
            return client.DownloadWorld(worldId);
            });
        tasks.push_back(std::move(handle));
      }
    }

    // All the tasks have been queued. Now wait for them to finish
    while (!tasks.empty())
    {
      checkForFinishedTasks();
    }
  }
  else
  {
    std::cout << "Invalid URL: only models and worlds or collections can be "
              << "downloaded so far." << std::endl;
    return false;
  }

  if (ignition::common::Console::Verbosity() >= 3)
  {
    std::cout << "Download succeeded." << std::endl;
  }
  return true;
}

//////////////////////////////////////////////////
extern "C" IGNITION_FUEL_TOOLS_VISIBLE void cmdVerbosity(const char *_verbosity)
{
  ignition::common::Console::SetVerbosity(std::atoi(_verbosity));
}

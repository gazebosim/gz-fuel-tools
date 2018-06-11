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

#ifndef _WIN32
  #include <unistd.h>
#endif

#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/StringUtils.hh>
#include <ignition/common/Util.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/LocalCache.hh"
#include "ignition/fuel_tools/ModelIterPrivate.hh"
#include "ignition/fuel_tools/ModelPrivate.hh"
#include "ignition/fuel_tools/Zip.hh"
#include "ignition/fuel_tools/WorldIterPrivate.hh"
#include "ignition/fuel_tools/WorldPrivate.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

class ignft::LocalCachePrivate
{
  /// \brief return all models in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<Model> ModelsInServer(const std::string &_path) const;

  /// \brief return all worlds in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<World> WorldsInServer(const std::string &_path) const;

  /// \brief return all models in a given Owner/models directory
  public: std::vector<Model> ModelsInPath(const std::string &_path);

  /// \brief client configuration
  public: const ClientConfig *config = nullptr;
};

//////////////////////////////////////////////////
std::vector<Model> LocalCachePrivate::ModelsInServer(
    const std::string &_path) const
{
  std::vector<Model> models;
  if (!common::isDirectory(_path))
  {
    ignwarn << "Server directory does not exist [" << _path << "]\n";
    return models;
  }

  common::DirIter end;
  common::DirIter ownIter(_path);
  while (ownIter != end)
  {
    if (!common::isDirectory(*ownIter))
    {
      ++ownIter;
      continue;
    }

    // This is an owner directory, look for models
    common::DirIter modIter(common::joinPaths(*ownIter, "models"));
    while (modIter != end)
    {
      if (!common::isDirectory(*modIter))
      {
        ++modIter;
        continue;
      }

      // Go through all versions
      common::DirIter versionIter(common::absPath(*modIter));
      while (versionIter != end)
      {
        if (!common::isDirectory(*versionIter))
        {
          ++versionIter;
          continue;
        }

        if (common::exists(common::joinPaths(*versionIter, "model.config")))
        {
          std::shared_ptr<ModelPrivate> modPriv(new ModelPrivate);
          modPriv->id.Name(common::basename(*modIter));
          modPriv->id.Owner(common::basename(*ownIter));
          modPriv->id.SetVersionStr(common::basename(*versionIter));
          modPriv->pathOnDisk = common::absPath(*versionIter);
          Model model(modPriv);
          models.push_back(model);
        }
        ++versionIter;
      }
      ++modIter;
    }
    ++ownIter;
  }
  return models;
}

//////////////////////////////////////////////////
std::vector<World> LocalCachePrivate::WorldsInServer(
    const std::string &_path) const
{
  std::vector<World> worlds;
  if (!common::isDirectory(_path))
  {
    ignwarn << "Server directory does not exist [" << _path << "]\n";
    return worlds;
  }

  common::DirIter end;
  common::DirIter ownIter(_path);
  while (ownIter != end)
  {
    if (!common::isDirectory(*ownIter))
    {
      ++ownIter;
      continue;
    }

    // This is an owner directory, look for worlds
    common::DirIter modIter(common::joinPaths(*ownIter, "worlds"));
    while (modIter != end)
    {
      if (!common::isDirectory(*modIter))
      {
        ++modIter;
        continue;
      }

      // Go through all versions
      common::DirIter versionIter(common::absPath(*modIter));
      while (versionIter != end)
      {
        if (!common::isDirectory(*versionIter))
        {
          ++versionIter;
          continue;
        }

        if (common::exists(common::joinPaths(*versionIter, "world.config")))
        {
          std::shared_ptr<WorldPrivate> modPriv(new WorldPrivate);
          modPriv->id.SetName(common::basename(*modIter));
          modPriv->id.SetOwner(common::basename(*ownIter));
          modPriv->id.SetVersionStr(common::basename(*versionIter));
          modPriv->pathOnDisk = common::absPath(*versionIter);
          World world(modPriv);
          worlds.push_back(world);
        }
        ++versionIter;
      }
      ++modIter;
    }
    ++ownIter;
  }
  return worlds;
}

//////////////////////////////////////////////////
LocalCache::LocalCache(const ClientConfig *_config)
  : dataPtr(new LocalCachePrivate)
{
  this->dataPtr->config = _config;
}

//////////////////////////////////////////////////
LocalCache::~LocalCache()
{
}

//////////////////////////////////////////////////
ModelIter LocalCache::AllModels()
{
  std::vector<Model> models;
  if (this->dataPtr->config)
  {
    for (auto &server : this->dataPtr->config->Servers())
    {
      std::string path = common::joinPaths(
          this->dataPtr->config->CacheLocation(), server.Url().Path().Str());
      auto srvModels = this->dataPtr->ModelsInServer(path);
      for (auto &mod : srvModels)
      {
        mod.dataPtr->id.Server(server);
      }
      models.insert(models.end(), srvModels.begin(), srvModels.end());
    }
  }

  return ModelIterFactory::Create(models);
}

//////////////////////////////////////////////////
WorldIter LocalCache::AllWorlds()
{
  std::vector<WorldIdentifier> worldIds;
  if (this->dataPtr->config)
  {
    // Iterate over servers
    for (auto &server : this->dataPtr->config->Servers())
    {
      std::string path = common::joinPaths(
          this->dataPtr->config->CacheLocation(), server.Url().Path().Str());

      // Make sure the server info is correct
      auto srvWorlds = this->dataPtr->WorldsInServer(path);
      for (auto &world : srvWorlds)
      {
        WorldIdentifier id = world.dataPtr->id;
        id.SetServer(server);

        worldIds.push_back(id);
      }
    }
  }

  return WorldIterFactory::Create(worldIds);
}

//////////////////////////////////////////////////
Model LocalCache::MatchingModel(const ModelIdentifier &_id)
{
  // For the tip, we have to find the highest version
  bool tip = (_id.Version() == 0);
  Model tipModel;

  for (auto iter = this->AllModels(); iter; ++iter)
  {
    auto id = iter->Identification();
    if (_id == id)
    {
      if (_id.Version() == id.Version())
        return *iter;
      else if (tip && id.Version() > tipModel.Identification().Version())
        tipModel = *iter;
    }
  }

  return tipModel;
}

//////////////////////////////////////////////////
bool LocalCache::MatchingWorld(WorldIdentifier &_id)
{
  // For the tip, we have to find the highest version
  bool tip = (_id.Version() == 0);
  WorldIdentifier tipWorld;

  for (auto iter = this->AllWorlds(); iter; ++iter)
  {
    auto id = iter->Identification();
    if (_id == id)
    {
      if (_id.Version() == id.Version())
      {
        _id = id;
        return true;
      }
      else if (tip && id.Version() > tipWorld.Version())
      {
        tipWorld = id;
      }
    }
  }

  auto foundTip = !(tipWorld == WorldIdentifier());
  if (foundTip)
  {
    _id = tipWorld;
  }

  return foundTip;
}

//////////////////////////////////////////////////
ModelIter LocalCache::MatchingModels(const ModelIdentifier &_id)
{
  if (_id.Name().empty() && _id.Server().URL().empty() && _id.Owner().empty())
    return ModelIterFactory::Create();

  std::vector<Model> models;
  for (auto iter = this->AllModels(); iter; ++iter)
  {
    bool matches = true;
    if (!_id.Name().empty() && _id.Name() != iter->Identification().Name())
      matches = false;
    if (!_id.Owner().empty() && _id.Owner() != iter->Identification().Owner())
      matches = false;
    if (!_id.Server().URL().empty() &&
        _id.Server().URL() != iter->Identification().Server().URL())
      matches = false;
    if (matches)
      models.push_back(*iter);
  }
  return ModelIterFactory::Create(models);
}

//////////////////////////////////////////////////
WorldIter LocalCache::MatchingWorlds(const WorldIdentifier &_id)
{
  if (_id.Name().empty() && _id.Server().URL().empty() && _id.Owner().empty())
    return WorldIterFactory::Create();

  std::vector<WorldIdentifier> worldIds;
  for (auto iter = this->AllWorlds(); iter; ++iter)
  {
    bool matches = true;
    if (!_id.Name().empty() && _id.Name() != iter->Identification().Name())
      matches = false;
    if (!_id.Owner().empty() && _id.Owner() != iter->Identification().Owner())
      matches = false;
    if (!_id.Server().URL().empty() &&
        _id.Server().URL() != iter->Identification().Server().URL())
      matches = false;
    if (matches)
      worldIds.push_back(iter->Identification());
  }
  return WorldIterFactory::Create(worldIds);
}

//////////////////////////////////////////////////
bool LocalCache::SaveModel(
  const ModelIdentifier &_id, const std::string &_data, const bool _overwrite)
{
  if (_id.Server().URL().empty() || _id.Owner().empty() ||
      _id.Name().empty() || _id.Version() == 0)
  {
    ignerr << "Incomplete model identifier, failed to save model." << std::endl
           << _id.AsString();
    return false;
  }

  auto cacheLocation = this->dataPtr->config->CacheLocation();

  auto modelRootDir = common::joinPaths(cacheLocation,
      _id.Server().Url().Path().Str(), _id.Owner(), "models", _id.Name());
  auto modelVersionedDir = common::joinPaths(modelRootDir, _id.VersionStr());

  // Is it already in the cache?
  if (common::isDirectory(modelVersionedDir) && !_overwrite)
  {
    ignerr << "Directory [" << modelVersionedDir << "] already exists"
           << std::endl;
    return false;
  }

  // Create the model directory.
  if (!common::createDirectories(modelVersionedDir))
  {
    ignerr << "Unable to create directory [" << modelVersionedDir << "]"
           << std::endl;
  }

  auto zipFile = common::joinPaths(modelVersionedDir, _id.Name() + ".zip");
  std::ofstream ofs(zipFile, std::ofstream::out);
  ofs << _data;
  ofs.close();

  if (!Zip::Extract(zipFile, modelVersionedDir))
  {
    ignerr << "Unable to unzip [" << zipFile << "]" << std::endl;
    return false;
  }

  if (!common::removeDirectoryOrFile(zipFile))
  {
    ignwarn << "Unable to remove [" << zipFile << "]" << std::endl;
  }

  ignmsg << "Saved model at:" << std::endl
         << "  " << modelVersionedDir << std::endl;

  return true;
}


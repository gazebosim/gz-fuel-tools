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

#include <stdio.h>
#include <unistd.h>
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

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

class ignft::LocalCachePrivate
{
  /// \brief return all models in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<Model> ModelsInServer(const std::string &_path) const;

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

      // Model should at a minimum have the tip version
      std::string modelPath = common::absPath(*modIter);
      if (common::exists(common::joinPaths(modelPath, "tip", "model.config")))
      {
        // Found a model!!!
        std::shared_ptr<ModelPrivate> modPriv(new ModelPrivate);
        modPriv->id.Name(common::basename(*modIter));
        modPriv->id.Owner(common::basename(*ownIter));
        modPriv->pathOnDisk = modelPath;
        Model model(modPriv);
        models.push_back(model);
      }
      ++modIter;
    }
    ++ownIter;
  }
  return models;
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
Model LocalCache::MatchingModel(const ModelIdentifier &_id)
{
  for (auto iter = this->AllModels(); iter; ++iter)
  {
    if (_id == iter->Identification())
    {
      return *iter;
    }
  }
  return Model();
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
  auto modelVersionedDir = common::joinPaths(modelRootDir,
      std::to_string(_id.Version()));

  // Is it already in the cache?
  if (common::isDirectory(modelVersionedDir) && !_overwrite)
  {
    ignerr << "Directory [" << modelVersionedDir << "] already exists" << std::endl;
    return false;
  }

  // Create the model directory.
  if (!common::createDirectories(modelVersionedDir))
  {
    ignerr << "Unable to create directory [" << modelVersionedDir << "]" << std::endl;
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
         << "  " << modelVersionedDir << "]" << std::endl;

  this->UpdateTipSymLink(modelRootDir);

  return true;
}

//////////////////////////////////////////////////
bool LocalCache::UpdateTipSymLink(const std::string &_path)
{
  unsigned int highestVersion{0};

  // Iterate over all subdirectories of _path and get the highest version
  common::DirIter endIter;
  common::DirIter versionIter(_path);
  while (versionIter != endIter)
  {
    if (!common::isDirectory(*versionIter))
    {
      ++versionIter;
      continue;
    }

    auto parts = common::Split(*versionIter, *common::separator("").c_str());

    unsigned int version{0};
    try
    {
      version = std::stoi(parts.back());
    }
    catch(std::invalid_argument &_e)
    {
      ++versionIter;
      continue;
    }

    highestVersion = version > highestVersion ? version : highestVersion;

    ++versionIter;
  }

  if (highestVersion == 0)
  {
    ignerr << "Failed to locate a numbered directory under [" << _path
           << "], symbolic link not created." << std::endl;
    return false;
  }

  // Remove tip dir if it already exists
  auto tipDir = common::joinPaths(_path, "tip");
  if (common::isDirectory(tipDir))
  {
    // TODO: Move this to ign-common, removeAll / removeDirectoryOrFile are
    // both failing to remove the symlink
    if (remove(tipDir.c_str()) == -1)
    {
      ignerr << "Unable to remove directory [" << tipDir << "]" << std::endl;
      return false;
    }
  }

  // Create sym link
  auto highestDir = common::joinPaths(_path, std::to_string(highestVersion));

  if (!symlink(highestDir.c_str(), tipDir.c_str()))
  {
    ignmsg << "Failed to create a symbolic link from:" << std::endl
           << "  " << tipDir << std::endl
           <<"to:" << std::endl
           << "  " << highestDir << std::endl;
    return false;
  }

  ignmsg << "Created symbolic link from:" << std::endl
         << "  " << tipDir << std::endl
         <<"to:" << std::endl
         << "  " << highestDir << std::endl;

  return true;
}

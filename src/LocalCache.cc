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

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/LocalCache.hh"
#include "ignition/fuel-tools/ModelIterPrivate.hh"
#include "ignition/fuel-tools/ModelPrivate.hh"
#include "ignition/fuel-tools/Zip.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

class ignft::LocalCachePrivate
{
  /// \brief return all models in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<Model> ModelsInServer(const std::string &_path) const;

  /// \brief return all models in a given Owner directory
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
  }
  else
  {
    common::DirIter end;
    common::DirIter ownIter(_path);
    while (ownIter != end)
    {
      if (common::isDirectory(*ownIter))
      {
        // This is an owner directory, look for models
        common::DirIter modIter(*ownIter);
        while (modIter != end)
        {
          if (common::isDirectory(*modIter))
          {
            std::string modelPath = common::absPath(*modIter);
            if (common::exists(common::joinPaths(modelPath, "model.config")))
            {
              // Found a model!!!
              std::shared_ptr<ModelPrivate> modPriv(new ModelPrivate);
              modPriv->id.Name(common::basename(*modIter));
              modPriv->id.Owner(common::basename(*ownIter));
              modPriv->pathOnDisk = modelPath;
              Model model(modPriv);
              models.push_back(model);
              // ignmsg << "Found model [" << modelPath << "]\n";
            }
          }
          ++modIter;
        }
      }
      ++ownIter;
    }
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
          this->dataPtr->config->CacheLocation(), server.LocalName());
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
  auto cacheLocation = this->dataPtr->config->CacheLocation();
  std::string name = _id.Name();
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);
  auto modelDir = common::joinPaths(
    cacheLocation, "models", _id.Owner(), name);

  // Is it already in the cache?
  if (common::isDirectory(modelDir) && !_overwrite)
  {
    ignerr << "Directory [" << modelDir << "] already exists" << std::endl;
    return false;
  }

  // Create the model directory.
  if (!common::createDirectories(modelDir))
  {
    ignerr << "Unable to create directory [" << modelDir << "]" << std::endl;
  }

  auto zipFile = common::joinPaths(modelDir, _id.Name() + ".zip");
  std::ofstream ofs(zipFile, std::ofstream::out);
  ofs << _data;
  ofs.close();

  if (!Zip::Extract(zipFile, modelDir))
  {
    ignerr << "Unable to unzip [" << zipFile << "]" << std::endl;
    return false;
  }

  // if (!common::removeDirectoryOrFile(zipFile))
  // {
  //   ignwarn << "Unable to remove [" << zipFile << "]" << std::endl;
  // }

  return true;
}

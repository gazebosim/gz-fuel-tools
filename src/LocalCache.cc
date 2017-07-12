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

#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/fuel-tools/LocalCache.hh>
#include <ignition/fuel-tools/ModelIterPrivate.hh>
#include <ignition/fuel-tools/ModelPrivate.hh>

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


class ignft::LocalCachePrivate
{
  /// \brief return all models in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<Model> ModelsInServer(
              const std::string &_path) const;

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
            std::string config = common::joinPaths(modelPath, "model.config");
            if (common::exists(config))
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
            else
            {
              igndbg << "No model in [" << config << "]\n";
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
  if (common::isDirectory(_config->CacheLocation()))
  {
    this->dataPtr->config = _config;
  }
  else
  {
    ignerr << "Cache location does not exist [" << _config->CacheLocation()
      << "]\n";
  }
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
        mod.dataPtr->id.SourceURL(server.URL());
      }
      models.insert(models.end(), srvModels.begin(), srvModels.end());
    }
  }

  return ModelIterFactory::Create(models);
}

//////////////////////////////////////////////////
Model LocalCache::MatchingModel(const ModelIdentifier &_id)
{
  // TODO
  // Find an exact model
}

//////////////////////////////////////////////////
bool LocalCache::SaveModel(
    const ModelIdentifier &_id, const std::string &_data)
{
  // TODO
  // Does this model exist?
  // Is it already in the cache?
}

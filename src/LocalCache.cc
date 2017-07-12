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

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


class ignft::LocalCachePrivate
{
  /// \brief client configuration
  public: const ClientConfig *config = nullptr;
};


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
  // TODO
  // Look through all servers
  // Look through all owners
  // Make a giant list of all models
}

//////////////////////////////////////////////////
ModelIter LocalCache::MatchingModels(const ModelIdentifier &_id)
{
  // TODO
  // If servers is unset, look through all models
  // if owners is unset, look through all owners
  // if models is unset, look through all models
}

//////////////////////////////////////////////////
Model LocalCache::MatchingModel(const ModelIdentifier &_id)
{
  // TODO
  // Find an exact model
}

//////////////////////////////////////////////////
bool LocalCache::AddModel(
    const ModelIdentifier &_id, const std::string &_data)
{
  // TODO
  // Does this model exist?
  // Is it already in the cache?
}

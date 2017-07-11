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

#ifndef IGNITION_FUEL_TOOLS_LOCALCACHE_HH_
#define IGNITION_FUEL_TOOLS_LOCALCACHE_HH_

#include <memory>
#include <ignition/fuel-tools/ClientConfig.hh>
#include <ignition/fuel-tools/Helpers.hh>
#include <ignition/fuel-tools/ModelIter.hh>

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Forward declaration
    class LocalCachePrivate;

    /// \brief Class for managing stuff in the local cache
    class IGNITION_FUEL_TOOLS_VISIBLE LocalCache
    {
      /// \brief Constructor
      /// \param[in] _config The configuration for the client
      public: LocalCache(const ClientConfig *_config);

      /// \brief destructor
      public: virtual ~LocalCache();

      /// \brief Get all models in offline cache
      public: ModelIter AllModels();

      /// \brief Get all models which partially match an ID
      /// \param[in] _id An id with at least one of name, owner, source
      public: ModelIter MatchingModels(const ModelIdentifier &_id);

      /// \brief Get the first model matching an ID
      /// \param[in] _id An id with at least one of name, owner, source
      public: Model MatchingModel(const ModelIdentifier &_id);

      /// \brief Add a model from packed data to the local cache
      /// \param[in] _id A completely populated ID
      /// \returns true if the model was successfully added to the local cache
      public: bool AddModel(
                  const ModelIdentifier &_id, const std::string &_data);

      private: std::shared_ptr<LocalCachePrivate> dataPtr;
    };
  }
}

#endif

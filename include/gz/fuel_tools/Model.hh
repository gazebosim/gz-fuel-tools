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

#ifndef GZ_FUEL_TOOLS_MODEL_HH_
#define GZ_FUEL_TOOLS_MODEL_HH_

#include <memory>
#include <string>

#include "gz/fuel_tools/Result.hh"
#include "gz/fuel_tools/ModelIdentifier.hh"

namespace gz
{
  namespace fuel_tools
  {
    /// \brief Forward Declaration
    class ModelPrivate;

    /// \brief Forward declaration
    class ModelIter;

    /// \brief Forward declaration
    class ModelIterPrivate;

    /// \brief forward declaration
    class LocalCache;

    /// \brief forward declaration
    class LocalCachePrivate;

    /// \brief Forward declaration
    class IterIds;

    /// \brief Forward declaration
    class IterRESTIds;

    /// \brief Forward declaration
    class IterRestIds;

    /// \brief Forward declaration
    class ModelIterTest;

    /// \brief Defines how to identify a model.
    class GZ_FUEL_TOOLS_VISIBLE Model
    {
      friend IterIds;
      friend IterRESTIds;
      friend IterRestIds;
      friend ModelIter;
      friend ModelIterPrivate;
      friend ModelIterTest;
      friend LocalCache;
      friend LocalCachePrivate;

      /// \brief Default constructor
      public: Model();

      public: explicit Model(const gz::fuel_tools::ModelIdentifier &_id,
                             const std::string &_pathOnDisk = "");

      public: Model(const std::string &_name, const std::string &_owner,
                    const gz::fuel_tools::ServerConfig &_server);

      /// \brief Returns false if model was constructed via Model()
      public: operator bool() const;

      /// \brief Returns false if model was constructed via Model().
      /// \return False if model was constructed via Model().
      public: operator bool();

      /// \brief Returns information identifying the model.
      /// \return Information that can identify the model.
      public: ModelIdentifier Identification() const;

      /// \brief Make sure this model is in the local cache
      /// \remarks this downloads the model and saves it locally if necessary
      /// \returns The result of fetching
      public: Result Fetch() const;

      /// \brief Returns a path to the model on disk if it is already
      /// cached.
      /// \returns path, or empty string if the model is not cached.
      public: std::string PathToModel() const;

      /// \brief PIMPL
      GZ_UTILS_IMPL_PTR_FWD(ModelPrivate, dataPtr)
    };
  }
}
#endif

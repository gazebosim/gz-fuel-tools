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

#ifndef IGNITION_FUEL_TOOLS_WORLD_HH_
#define IGNITION_FUEL_TOOLS_WORLD_HH_

#include <memory>
#include <string>

#include "ignition/fuel_tools/Helpers.hh"
#include "ignition/fuel_tools/Result.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Forward Declaration
    class WorldPrivate;

    /// \brief Forward declaration
    class WorldIter;

    /// \brief Forward declaration
    class WorldIterPrivate;

    /// \brief forward declaration
    class LocalCache;

    /// \brief forward declaration
    class LocalCachePrivate;

    /// \brief Forward declaration
    class WorldIterIds;

    /// \brief Forward declaration
    class WorldIterRESTIds;

    /// \brief Forward declaration
    class WorldIterTest;

    /// \brief Defines how to identify a world
    class IGNITION_FUEL_TOOLS_VISIBLE World
    {
      friend WorldIterIds;
      friend WorldIterRESTIds;
      friend WorldIter;
      friend WorldIterPrivate;
      friend WorldIterTest;
      friend LocalCache;
      friend LocalCachePrivate;

      /// \brief Default constructor
      public: World();

      /// \brief Protected constructor
      protected: explicit World(std::shared_ptr<WorldPrivate> _dptr);

      /// \brief Copy constructor
      public: World(const World &_orig) = default;

      /// \brief Returns false if world was constructed via World()
      public: operator bool() const;

      /// \brief Returns information identifying the world
      public: WorldIdentifier Identification() const;

      /// \brief Returns a path to the world on disk if it is already cached
      /// \returns path, or empty string if the world is not cached.
      public: std::string PathToWorld() const;

      /// \brief Private data pointer
      private: std::shared_ptr<WorldPrivate> dataPtr;
    };
  }
}

#endif

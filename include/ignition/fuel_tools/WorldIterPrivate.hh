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

#ifndef IGNITION_FUEL_TOOLS_WORLDITERPRIVATE_HH_
#define IGNITION_FUEL_TOOLS_WORLDITERPRIVATE_HH_

#include <string>
#include <vector>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/World.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"
#include "ignition/fuel_tools/REST.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief forward declaration
    class WorldIter;

    /// \brief Private class, do not include or instantiate
    class IGNITION_FUEL_TOOLS_VISIBLE WorldIterFactory
    {
      /// \brief Create a world iterator from a vector of world identifiers
      /// \param[in] _ids World identifiers
      public: static WorldIter Create(const std::vector<WorldIdentifier> &_ids);

      /// \brief Create a world iter that will make REST api calls
      /// \param[in] _rest a REST request
      /// \param[in] _server The server to request the operation
      /// \param[in] _api The path to request
      public: static WorldIter Create(const REST &_rest,
                                      const ServerConfig &_server,
                                      const std::string &_api);

      /// \brief Create a world iterator that is empty
      public: static WorldIter Create();
    };

    /// \brief Private class, do not include or instantiate
    class IGNITION_FUEL_TOOLS_VISIBLE WorldIterPrivate
    {
      /// \brief destructor
      public: virtual ~WorldIterPrivate();

      /// \brief Advance iterator to next world
      public: virtual void Next() = 0;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() = 0;

      /// \brief current world for returning references
      public: World world;
    };

    /// \brief class for iterating through world ids where all are known
    ///        in advance
    class IGNITION_FUEL_TOOLS_VISIBLE WorldIterIds : public WorldIterPrivate
    {
      /// \brief constructor
      public: explicit WorldIterIds(std::vector<WorldIdentifier> _ids);

      /// \brief destructor
      public: virtual ~WorldIterIds();

      /// \brief Advance iterator to next world
      public: virtual void Next() override;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() override;

      /// \brief World identifiers that have been requested
      protected: std::vector<WorldIdentifier> ids;

      /// \brief where the current iterator is in the list of ids
      protected: std::vector<WorldIdentifier>::iterator idIter;
    };

    /// \brief class for iterating through world ids where all are known
    ///        in advance
    class IGNITION_FUEL_TOOLS_VISIBLE IterWorlds: public WorldIterPrivate
    {
      /// \brief constructor
      public: explicit IterWorlds(std::vector<World> _worlds);

      /// \brief destructor
      public: virtual ~IterWorlds();

      /// \brief Advance iterator to next world
      public: virtual void Next() override;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() override;

      /// \brief Worlds to iterator through
      protected: std::vector<World> worlds;

      /// \brief where the current iterator is in the list of worlds
      protected: std::vector<World>::iterator worldIter;
    };


    /// \brief class for iterating through world ids from a rest API
    class IGNITION_FUEL_TOOLS_VISIBLE WorldIterRESTIds: public WorldIterPrivate
    {
      /// \brief constructor
      public: WorldIterRESTIds(const REST &_rest,
                               const ServerConfig &_server,
                               const std::string &_api);

      /// \brief destructor
      public: virtual ~WorldIterRESTIds();

      /// \brief Advance iterator to next world
      public: virtual void Next() override;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() override;

      /// \brief Client configuration
      public: ServerConfig config;

      /// \brief RESTful client
      public: REST rest;

      /// \brief World identifiers in the current page
      protected: std::vector<WorldIdentifier> ids;

      /// \brief where the current iterator is in the list of ids
      protected: std::vector<WorldIdentifier>::iterator idIter;
    };
  }
}

#endif

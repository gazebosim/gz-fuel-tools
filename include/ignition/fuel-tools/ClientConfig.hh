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

#ifndef IGNITION_FUEL_TOOLS_CLIENTCONFIG_HH_
#define IGNITION_FUEL_TOOLS_CLIENTCONFIG_HH_

#include <memory>
#include <vector>

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Forward Declaration
    class ClientConfigPrivate;

    /// \brief High level interface to ignition fuel
    ///
    class ClientConfig
    {
      /// \brief constructor
      public: ClientConfig();

      /// \brief destructor
      public: ~ClientConfig();

      /// \brief List URI of servers the client will connect to
      public: std::vector<std::string> Servers() const;

      /// \brief Add a server to the list
      public: void AddServer();

      /// \brief PIMPL
      private: std::unique_ptr<ClientConfigPrivate> dataPtr;
    };
  }
}

#endif

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

#ifndef GZ_FUEL_TOOLS_INTERFACE_HH_
#define GZ_FUEL_TOOLS_INTERFACE_HH_

#include <string>
#include "gz/fuel_tools/Export.hh"
#include "gz/fuel_tools/FuelClient.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Download the specified resource into the default configuration of
    /// fuel tools. This will place the asset in ~/.ignition/fuel.
    /// \param[in] _uri URI to the asset.
    /// \return Path to the downloaded asset. Empty on error.
    IGNITION_FUEL_TOOLS_VISIBLE std::string fetchResource(
        const std::string &_uri);

    /// \brief Download the specified resource using the ClientConfig
    /// contained in the FuelClient parameter.
    /// \param[in] _uri URI to the asset.
    /// \param[in] _client Custom FuelClient configuration.
    /// \return Path to the downloaded asset. Empty on error.
    IGNITION_FUEL_TOOLS_VISIBLE std::string fetchResourceWithClient(
        const std::string &_uri, gz::fuel_tools::FuelClient &_client);
  }
}

#endif

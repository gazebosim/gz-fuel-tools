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

#ifndef IGNITION_FUELTOOLS_IGN_HH_
#define IGNITION_FUELTOOLS_IGN_HH_

#include "ignition/fuel_tools/Export.hh"

/// \brief External hook to read the library version.
/// \return C-string representing the version. Ex.: 0.1.2
extern "C" IGNITION_FUEL_TOOLS_VISIBLE char *ignitionVersion();

/// \brief Set verbosity level
/// \param[in] _verbosity 0 to 4
extern "C" IGNITION_FUEL_TOOLS_VISIBLE void cmdVerbosity(
    const char *_verbosity);

/// \brief External hook to execute 'ign fuel list -t model' from the command
/// line.
/// \param[in] _url Optional server URL.
/// \param[in] _owner Optional owner name
/// \param[in] _raw 'true' for machine readable output.
/// \param[in] _configFile Path to a YAML configuration file.
/// \return 1 if successful, 0 if not.
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int listModels(
    const char *_url = nullptr, const char *_owner = "",
    const char *_raw = "false", const char *_configFile = nullptr);

/// \brief External hook to execute 'ign fuel list -t world' from the command
/// line.
/// \param[in] _url Optional server URL.
/// \param[in] _owner Optional owner name
/// \param[in] _raw 'true' for machine readable output.
/// \param[in] _configFile Path to a YAML configuration file.
/// \return 1 if successful, 0 if not.
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int listWorlds(
    const char *_url = nullptr, const char *_owner = "",
    const char *_raw = "false", const char *_configFile = nullptr);

/// \brief External hook to execute 'ign fuel download -u URL' from the command
/// line.
/// \param[in] _url Resource URL.
/// \param[in] _configFile Path to a YAML configuration file.
/// \param[in] _header An HTTP header.
/// \return 1 if successful, 0 if not.
extern "C" IGNITION_FUEL_TOOLS_VISIBLE int downloadUrl(
    const char *_url = nullptr, const char *_configFile = nullptr,
    const char *_header = nullptr);

#endif

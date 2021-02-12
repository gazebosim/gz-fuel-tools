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

#ifndef IGNITION_FUEL_TOOLS_HELPERS_HH_
#define IGNITION_FUEL_TOOLS_HELPERS_HH_

#include <string>
#include <ignition/common/URI.hh>
#include <ignition/fuel_tools/Export.hh>

// Use safer functions on Windows
#ifdef _MSC_VER
  #define ign_strcat strcat_s
  #define ign_strcpy strcpy_s
  #define ign_sprintf sprintf_s
  #define ign_strdup _strdup
#else
  #define ign_strcat std::strcat
  #define ign_strcpy std::strcpy
  #define ign_sprintf std::sprintf
  #define ign_strdup strdup
#endif

namespace ignition
{
namespace fuel_tools
{
/// \brief Convert a URI to a string suitable to use as a path on disk.
/// It strips the scheme and authority's `//` prefix.
/// \param[in] _uri URI to convert.
/// \return String suitable to use in file paths
IGNITION_FUEL_TOOLS_VISIBLE
std::string uriToPath(const ignition::common::URI &_uri);
}
}

// IGNITION_FUEL_TOOLS_HELPERS_HH_
#endif


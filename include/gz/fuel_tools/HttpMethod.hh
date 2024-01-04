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
#ifndef GZ_FUEL_TOOLS_HTTPMETHOD_HH_
#define GZ_FUEL_TOOLS_HTTPMETHOD_HH_

#include <map>
#include <memory>
#include <string>
#include <vector>

#if defined(_WIN32) && defined(DELETE)
#undef DELETE
#endif

namespace gz::fuel_tools
{
  /// \brief the types of HTTP methods
  enum class HttpMethod
  {
    /// \brief Get method.
    GET,

    /// \brief Post method.
    POST,

    /// \brief Delete method.
    DELETE,

    /// \brief Put method.
    PUT,

    /// \brief Patch method.
    PATCH,

    /// \brief Post form method.
    POST_FORM,

    /// \brief Patch form method.
    PATCH_FORM
  };
}  // namespace gz::fuel_tools

#endif  // GZ_FUEL_TOOLS_HTTPMETHOD_HH_

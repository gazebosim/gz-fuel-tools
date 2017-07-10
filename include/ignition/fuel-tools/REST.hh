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
#ifndef IGNITION_FUEL_TOOLS_REST_HH_
#define IGNITION_FUEL_TOOLS_REST_HH_

#include <memory>
#include <string>
#include <vector>

#include "ignition/fuel-tools/Helpers.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Stores the common configuration for all REST requests given
    /// a specific Girder server.
    struct IGNITION_FUEL_TOOLS_VISIBLE RESTResponse
    {
      /// \brief The returned status code. E.g.: 200
      public: int statusCode = 0;

      /// \brief The data received.
      public: std::string data;
    };

    /// \brief A helper class for making REST requests.
    class IGNITION_FUEL_TOOLS_VISIBLE REST
    {
      /// \brief Trigger a REST request.
      /// \param[in] _httpMethod The HTTP method. Use all uppercase letters.
      ///            E.g.: "GET"
      /// \param[in] _url The url to request.
      ///            E.g.: "http://localhost:8000/"
      /// \param[in] _path The path to request.
      ///            E.g.: "collection"
      /// \param[in] _queryStrings All the query strings to be requested.
      ///            E.g.: {"sort=name", "sortdir=1"}
      /// \param[in] _headers All the headers to be included in the HTTP request
      ///            E.g.: {"Accept: application/json"}
      /// \param[in] _data Data to be included in the HTTP request.
      public: static RESTResponse Request(const std::string &_httpMethod,
          const std::string &_url, const std::string &_path,
          const std::vector<std::string> &_queryStrings,
          const std::vector<std::string> &_headers,
          const std::string &_data);
    };
  }
}

#endif

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
#ifndef IGNITION_FUEL_TOOLS_RESTCLIENT_HH_
#define IGNITION_FUEL_TOOLS_RESTCLIENT_HH_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ignition/fuel_tools/Export.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Stores a response to a RESTful request
    struct IGNITION_FUEL_TOOLS_VISIBLE RestResponse
    {
      /// \brief The returned status code. E.g.: 200
      // cppcheck-suppress unusedStructMember
      public: int statusCode = 0;

      /// \brief The data received.
      public: std::string data = "";

      /// \brief Map of headers where the key is the header type.
      /// For example, a raw header of the form "Content-Type: json" would
      /// use "Content-Type" as a key and "json" as the key's data.
      public: std::map<std::string, std::string> headers;
    };

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
      POST_FORM
    };

    /// \brief A helper class for making REST requests.
    class IGNITION_FUEL_TOOLS_VISIBLE Rest
    {
      /// \brief Default constructor.
      public: Rest() = default;

      /// \brief Trigger a REST request.
      /// \param[in] _method The HTTP method. Use all uppercase letters.
      ///            E.g.: "GET"
      /// \param[in] _url The url to request.
      ///            E.g.: "http://localhost:8000/"
      /// \param[in] _version The protocol version.
      ///            E.g.: "1.0"
      /// \param[in] _path The path to request.
      ///            E.g.: "collection"
      /// \param[in] _queryStrings All the query strings to be requested.
      ///            E.g.: {"sort=name", "sortdir=1"}
      /// \param[in] _headers All the headers to be included in the HTTP request
      ///            E.g.: {"Accept: application/json"}
      /// \param[in] _data Data to be included in the HTTP request.
      /// \param[in] _form multi-part / form data to be used with
      ///            Method::POST_FORM
      public: virtual RestResponse Request(const HttpMethod _method,
          const std::string &_url,
          const std::string &_version,
          const std::string &_path,
          const std::vector<std::string> &_queryStrings,
          const std::vector<std::string> &_headers,
          const std::string &_data,
          const std::map<std::string, std::string> &_form =
          std::map<std::string, std::string>()) const;

      /// \brief Set the user agent name.
      /// \param[in] _agent User agent name.
      public: void SetUserAgent(const std::string &_agent);

      /// \brief Get the user agent name.
      /// \return Name of the user agent.
      public: const std::string &UserAgent() const;

      /// \brief The user agent name.
      private: std::string userAgent;
    };
  }
}

#endif

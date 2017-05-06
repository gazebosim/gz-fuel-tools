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

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>

#include "ignition/fuel-tools/GirderClient.hh"

using namespace ignition;
using namespace fuel_tools;

namespace ignition
{
  namespace fuel_tools
  {
    /// \internal
    /// \brief Private data for GirderClient class.
    class GirderClientPrivate
    {
      /// \brief Constructor.
      /// \param[in] _conf The common configuration for this Girder server.
      public: explicit GirderClientPrivate(const CommonConfiguration &_conf)
        : commonConf(_conf)
      {
      }

      /// \brief Destructor.
      public: virtual ~GirderClientPrivate() = default;

      /// \brief Stores the common configuration for all REST requests given
      /// a specific Girder server.
      public: CommonConfiguration commonConf;
    };
  }
}

/////////////////////////////////////////////////
size_t get_assets_cb(void *_buffer, size_t _size, size_t _nmemb, void *_userp)
{
  std::string *str = static_cast<std::string*>(_userp);
  _size *= _nmemb;

  // Append the new character data to the string
  str->append(static_cast<const char*>(_buffer), _size);
  return _size;
}

/////////////////////////////////////////////////
GirderClient::GirderClient(const CommonConfiguration &_conf)
{
  this->dataPtr.reset(new GirderClientPrivate(_conf));
}

/////////////////////////////////////////////////
GirderClient::~GirderClient()
{
}

/////////////////////////////////////////////////
GirderResponse GirderClient::Request(const std::string &_httpMethod,
  const std::string &_path, const std::vector<std::string> &_queryStrings,
  const std::vector<std::string> &_headers, const std::string &_data) const
{
  std::string url = this->dataPtr->commonConf.url + "/" +
    this->dataPtr->commonConf.apiVersion + "/" + _path;

  // Process query strings.
  if (!_queryStrings.empty())
  {
    url += "?";
    for (auto const &queryString : _queryStrings)
      url += queryString + "&";
    url.pop_back();
  }

  // Process headers.
  GirderResponse res;
  CURL *curl = curl_easy_init();
  struct curl_slist *headers = NULL;
  for (auto const &header : _headers)
  {
    headers = curl_slist_append(headers, header.c_str());
    if (!headers)
    {
      std::cerr << "[GirderClient::Request()]: Error processing header.\n  ["
                << header.c_str() << "]" << std::endl;
      return res;
    }
  }
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Process data.
  if (!_data.empty())
  {
    // Not supported yet.
  }

  std::string responseData;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_assets_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

  // Send the request.
  if (_httpMethod == "GET")
  {
    CURLcode success = curl_easy_perform(curl);
    if (success != CURLE_OK)
      std::cerr << "Error in REST request" << std::endl;
  }
  else
  {
    std::cerr << "Unsupported method" << std::endl;
    return res;
  }

  // Update the status code.
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.statusCode);

  // Update the data.
  res.data = responseData;

  // Cleaning.
  curl_easy_cleanup(curl);

  return res;
}

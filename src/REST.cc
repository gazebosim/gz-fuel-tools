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

#include "ignition/fuel-tools/REST.hh"

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
size_t WriteMemoryCallback(void *_buffer, size_t _size, size_t _nmemb,
    void *_userp)
{
  std::string *str = static_cast<std::string*>(_userp);
  _size *= _nmemb;

  // Append the new character data to the string
  str->append(static_cast<const char*>(_buffer), _size);
  return _size;
}

/////////////////////////////////////////////////
RESTResponse REST::Request(const std::string &_httpMethod,
    const std::string &_url, const std::string &_path,
    const std::vector<std::string> &_queryStrings,
    const std::vector<std::string> &_headers, const std::string &_data)
{
  RESTResponse res;

  if (_url.empty())
    return res;

  std::string url = _url;

  // Process query strings.
  if (!_queryStrings.empty())
  {
    url += "?";
    for (auto const &queryString : _queryStrings)
      url += queryString + "&";
    url.pop_back();
  }

  // Process headers.

  CURL *curl = curl_easy_init();
  struct curl_slist *headers = NULL;
  for (auto const &header : _headers)
  {
    headers = curl_slist_append(headers, header.c_str());
    if (!headers)
    {
      std::cerr << "[REST::Request()]: Error processing header.\n  ["
                << header.c_str() << "]" << std::endl;

      // cleanup
      curl_easy_cleanup(curl);
      return res;
    }
  }
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  std::string responseData;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

  // Send the request.
  if (_httpMethod == "GET")
  {
    // no need to do anything
  }
  else if (_httpMethod == "POST")
  {
    // enable POST
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _data.c_str());
  }
  else if (_httpMethod == "DELETE")
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _data.c_str());
  }
  else
  {
    std::cerr << "Unsupported method" << std::endl;

    // Cleanup.
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    return res;
  }

  CURLcode success = curl_easy_perform(curl);
  if (success != CURLE_OK)
    std::cerr << "Error in REST request" << std::endl;

  // Update the status code.
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.statusCode);

  // Update the data.
  res.data = responseData;

  // Cleaning.
  curl_easy_cleanup(curl);

  // free the headers
  curl_slist_free_all(headers);

  return res;
}

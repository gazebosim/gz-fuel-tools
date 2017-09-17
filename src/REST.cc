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


//////////////////////////////////////////////////
std::string JoinURL(const std::string &_base,
    const std::string &_more)
{
  if (_base.empty())
    return _more;
  else if (_more.empty())
    return _base;

  if ((_base[_base.size()-1] == '/' && _more[0] != '/')
      || (_base[_base.size()-1] != '/' && _more[0] == '/'))
  {
    // Only one of them has a slash
    return _base + _more;
  }
  else if (_base[_base.size()-1] != '/' && _more[0] != '/')
  {
    // Niether have a slash
    return  _base + '/' + _more;
  }
  else
  {
    // Both have a slash
    return _base + _more.substr(1, _more.size() - 1);
  }
}

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
    const std::vector<std::string> &_headers, const std::string &_data) const
{
  RESTResponse res;

  if (_url.empty())
    return res;

  std::string url = JoinURL(_url, _path);

  // Process query strings.
  if (!_queryStrings.empty())
  {
    url += "?";
    for (auto const &queryString : _queryStrings)
      url += queryString + "&";
    url.pop_back();
  }

  CURL *curl = curl_easy_init();

  // Process headers.
  struct curl_slist *headers = nullptr;
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

  char errbuf[CURL_ERROR_SIZE];
  // provide a buffer to store errors in
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
  // set the error buffer as empty before performing a request
  errbuf[0] = 0;

  // ToDo: Set this option to 0 only when using localhost.
  // Set the default value: do not prove that SSL certificate is authentic
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

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
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
  }

  CURLcode success = curl_easy_perform(curl);
  if (success != CURLE_OK)
  {
    std::cerr << "Error in REST request" << std::endl;
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", success);
    if(len)
      fprintf(stderr, "%s%s", errbuf,
              ((errbuf[len - 1] != '\n') ? "\n" : ""));
    else
      fprintf(stderr, "%s\n", curl_easy_strerror(success));
  }

  // Update the status code.
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.statusCode);

  // Update the data.
  res.data = responseData;

  // free the headers
  curl_slist_free_all(headers);

  // Cleaning.
  curl_easy_cleanup(curl);
  return res;
}

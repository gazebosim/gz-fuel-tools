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
#ifdef _WIN32
// DELETE is defined in winnt.h and causes a problem with HttpMethod::DELETE
#undef DELETE
#endif

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>

#include "ignition/fuel_tools/RestClient.hh"

using namespace ignition;
using namespace fuel_tools;


//////////////////////////////////////////////////
std::string RestJoinUrl(const std::string &_base,
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
size_t RestHeaderCallback(char *_ptr, size_t _size, size_t _nmemb, void *_userp)
{
  std::map<std::string, std::string> *map =
    static_cast<std::map<std::string, std::string> *>(_userp);

  _size *= _nmemb;

  if (map)
  {
    std::string header(_ptr);
    auto colonPos = header.find(":");

    // Only store header information of the form
    //     <type>: <data>
    if (colonPos != std::string::npos)
    {
      map->insert(std::make_pair(header.substr(0, colonPos),
                                 header.substr(colonPos+2)));
    }
  }

  return _size;
}

/////////////////////////////////////////////////
size_t RestWriteMemoryCallback(void *_buffer, size_t _size, size_t _nmemb,
    void *_userp)
{
  std::string *str = static_cast<std::string*>(_userp);
  _size *= _nmemb;

  // Append the new character data to the string
  str->append(static_cast<const char*>(_buffer), _size);
  return _size;
}

/////////////////////////////////////////////////
RestResponse Rest::Request(HttpMethod _method,
    const std::string &_url, const std::string &_version,
    const std::string &_path, const std::vector<std::string> &_queryStrings,
    const std::vector<std::string> &_headers, const std::string &_data,
    const std::map<std::string, std::string> &_form) const
{
  RestResponse res;

  if (_url.empty())
    return res;

  std::string url = RestJoinUrl(_url, _version);

  CURL *curl = curl_easy_init();
  char *encodedPath = curl_easy_escape(curl, _path.c_str(), _path.size());
  url = RestJoinUrl(url, encodedPath);

  // Process query strings.
  if (!_queryStrings.empty())
  {
    std::string fullQuery{"?"};
    for (auto const &queryString : _queryStrings)
      fullQuery += queryString + "&";

    fullQuery.pop_back();

    if (fullQuery != "?")
      url += fullQuery;
  }

  // Process headers.
  struct curl_slist *headers = nullptr;
  for (auto const &header : _headers)
  {
    headers = curl_slist_append(headers, header.c_str());
    if (!headers)
    {
      ignerr << "[Rest::Request()]: Error processing header.\n  ["
                << header.c_str() << "]" << std::endl;

      // cleanup
      curl_easy_cleanup(curl);
      return res;
    }
  }

  curl_easy_setopt(curl, CURLOPT_USERAGENT, this->userAgent.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  std::string responseData;
  std::map<std::string, std::string> headerData;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RestWriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, RestHeaderCallback);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);

  char errbuf[CURL_ERROR_SIZE];
  // provide a buffer to store errors in
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
  // set the error buffer as empty before performing a request
  errbuf[0] = 0;

  // ToDo: Set this option to 0 only when using localhost.
  // Set the default value: do not prove that SSL certificate is authentic
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

  std::ifstream ifs;
  struct curl_httppost *formpost = nullptr;

  // Send the request.
  if (_method == HttpMethod::GET)
  {
    // no need to do anything
  }
  else if (_method == HttpMethod::POST)
  {
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _data.c_str());
  }
  else if (_method == HttpMethod::POST_FORM)
  {
    struct curl_httppost *lastptr = nullptr;
    for (const auto &it : _form)
    {
      std::string key = it.first;
      std::string value = it.second;

      // follow same convention as curl cmdline tool
      // field starting with @ indicates path to file to upload
      // others are standard fields to describe the file
      if (!value.empty() && value[0] == '@')
      {
        // file upload
        std::string path = value.substr(1);
        std::string filename = ignition::common::basename(path);
        ifs.open(path, std::ios::binary);
        std::filebuf* pbuf = ifs.rdbuf();
        std::size_t size = pbuf->pubseekoff(0, ifs.end, ifs.in);
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, key.c_str(),
                     CURLFORM_BUFFER, filename.c_str(),
                     CURLFORM_BUFFERPTR, pbuf,
                     CURLFORM_BUFFERLENGTH, size,
                     CURLFORM_END);
      }
      else
      {
        // standard key:value fields
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, key.c_str(),
                     CURLFORM_COPYCONTENTS, value.c_str(),
                     CURLFORM_END);
       }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
  }
  else if (_method == HttpMethod::DELETE)
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  }
  else
  {
    ignerr << "Unsupported method" << std::endl;

    // Cleanup.
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
  }

  CURLcode success = curl_easy_perform(curl);
  if (success != CURLE_OK)
  {
    ignerr << "Error in REST request" << std::endl;
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", success);
    if (len)
    {
      fprintf(stderr, "%s%s", errbuf,
              ((errbuf[len - 1] != '\n') ? "\n" : ""));
    }
    else
      fprintf(stderr, "%s\n", curl_easy_strerror(success));
  }

  // Update the status code.
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.statusCode);

  // Update the data.
  res.data = responseData;

  // Update the header data.
  res.headers = headerData;

  if (formpost)
    curl_formfree(formpost);

  // free encoded path char*
  curl_free(encodedPath);

  // free the headers
  curl_slist_free_all(headers);

  // Cleaning.
  curl_easy_cleanup(curl);

  if (ifs.is_open())
    ifs.close();
  return res;
}

/////////////////////////////////////////////////
void Rest::SetUserAgent(const std::string &_agent)
{
  this->userAgent = _agent;
}

/////////////////////////////////////////////////
const std::string &Rest::UserAgent() const
{
  return this->userAgent;
}

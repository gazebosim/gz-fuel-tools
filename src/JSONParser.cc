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

#include <json/json.h>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/JSONParser.hh"

using namespace ignition;
using namespace fuel_tools;

#if defined(_WIN32) && !defined(timegm)
  #define timegm _mkgmtime
#endif

/////////////////////////////////////////////////
std::time_t ParseDateTime(const std::string &_datetime)
{
  int y, M, d, h, m;
  float s;
  sscanf(_datetime.c_str(), "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &s);
  std::tm tm;
  // Year since 1900
  tm.tm_year = y - 1900;
  // 0-11
  tm.tm_mon = M - 1;
  // 1-31
  tm.tm_mday = d;
  // 0-23
  tm.tm_hour = h;
  // 0-59
  tm.tm_min = m;
  // 0-61 (0-60 in C++11)
  tm.tm_sec = static_cast<int>(s);
  // 0 - standard time, 1, daylight saving, -1 unknown
  tm.tm_isdst = -1;
  return timegm(&tm);
}

/////////////////////////////////////////////////
std::vector<std::string> JSONParser::ParseTags(const Json::Value &_json)
{
  std::vector<std::string> tags;
  if (!_json.isArray())
  {
    ignerr << "JSON tags are not an array\n";
    return tags;
  }

  try
  {
    for (auto tagIt = _json.begin(); tagIt != _json.end(); ++tagIt)
      tags.push_back(tagIt->asString());
  }
#if JSONCPP_VERSION_MAJOR < 1 && JSONCPP_VERSION_MINOR < 10
  catch (...)
  {
    std::string what;
#else
  catch (const Json::LogicError &error)
  {
    std::string what = ": [" + std::string(error.what()) + "]";
#endif
    ignerr << "Exception parsing tags" << what << "\n";
    return std::vector<std::string>();
  }

  return tags;
}

/////////////////////////////////////////////////
ModelIdentifier JSONParser::ParseModel(const std::string &_json,
  const ServerConfig &_server)
{
  Json::CharReaderBuilder reader;
  Json::Value model;
  ModelIdentifier id;
  std::istringstream iss(_json);
  JSONCPP_STRING errs;

  Json::parseFromStream(reader, iss, &model, &errs);
  ParseModelImpl(model, id);

  // Adding the server used to retrieve the model.
  id.Server(_server);

  return id;
}

/////////////////////////////////////////////////
std::vector<ModelIdentifier> JSONParser::ParseModels(const std::string &_json,
  const ServerConfig &_server)
{
  std::vector<ModelIdentifier> ids;
  Json::CharReaderBuilder reader;
  Json::Value models;
  std::istringstream iss(_json);
  JSONCPP_STRING errs;

  Json::parseFromStream(reader, iss, &models, &errs);

  if (!models.isArray())
  {
    ignerr << "JSON response is not an array\n";
  }
  else
  {
    for (auto modelIt = models.begin(); modelIt != models.end(); ++modelIt)
    {
      Json::Value model = *modelIt;
      ModelIdentifier id;
      if (!ParseModelImpl(model, id))
      {
        ignerr << "Model isn't a json object!\n";
        break;
      }

      // Adding the server used to retrieve the model.
      id.Server(_server);

      ids.push_back(id);
    }
  }

  return ids;
}

/////////////////////////////////////////////////
bool JSONParser::ParseModelImpl(
  const Json::Value &_json, ModelIdentifier &_model)
{
  try
  {
    if (!_json.isObject())
    {
      ignerr << "Model isn't a json object!\n";
      return false;
    }

    if (_json.isMember("name"))
      _model.Name(_json["name"].asString());
    if (_json.isMember("owner"))
      _model.Owner(_json["owner"].asString());
    else
      _model.Owner("anonymous");
    if (_json.isMember("uuid"))
      _model.Uuid(_json["uuid"].asString());
    if (_json.isMember("updatedAt"))
      _model.ModifyDate(ParseDateTime(_json["updatedAt"].asString()));
    if (_json.isMember("createdAt"))
      _model.UploadDate(ParseDateTime(_json["createdAt"].asString()));
    if (_json.isMember("description"))
      _model.Description(_json["description"].asString());
    if (_json.isMember("likes"))
      _model.Likes(_json["likes"].asUInt());
    if (_json.isMember("downloads"))
      _model.Downloads(_json["downloads"].asUInt());
    if (_json.isMember("filesize"))
      _model.FileSize(_json["filesize"].asUInt());
    if (_json.isMember("license_name"))
      _model.LicenseName(_json["license_name"].asString());
    if (_json.isMember("license_url"))
      _model.LicenseURL(_json["license_url"].asString());
    if (_json.isMember("license_image"))
      _model.LicenseImageURL(_json["license_image"].asString());
    if (_json.isMember("tags"))
      _model.Tags(ParseTags(_json["tags"]));
  }
#if JSONCPP_VERSION_MAJOR < 1 && JSONCPP_VERSION_MINOR < 10
  catch (...)
  {
    std::string what;
#else
  catch (const Json::LogicError &error)
  {
    std::string what = ": [" + std::string(error.what()) + "]";
#endif
    ignerr << "Bad response from server" << what << "\n";
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
std::string JSONParser::BuildModel(ModelIter _modelIt)
{
  ModelIdentifier id = _modelIt->Identification();
  Json::Value value;
  value["name"] = id.Name();
  value["description"] = id.Description();
  value["category"] = id.Category();
  value["uuid"] = id.Uuid();

  Json::StreamWriterBuilder builder;
  return Json::writeString(builder, value);
}

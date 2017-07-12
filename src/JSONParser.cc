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

#include <iostream>
#include <string>
#include <vector>

#include <json/json.h>

#include "ignition/fuel-tools/ModelIterPrivate.hh"
#include "ignition/fuel-tools/JSONParser.hh"

using namespace ignition;
using namespace fuel_tools;


/////////////////////////////////////////////////
std::time_t ParseDateTime(const std::string &_datetime)
{
  int y,M,d,h,m;
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
  tm.tm_sec = (int)s;
  return std::mktime(&tm);
}

/////////////////////////////////////////////////
ModelIter JSONParser::ParseModels(const std::string &_json)
{
  Json::Reader reader;
  Json::Value value;
  reader.parse(_json, value);

//  Json::StyledWriter writer;
//  std::string out = writer.write(value);

  std::vector<ModelIdentifier> ids;
  for (auto it = value.begin(); it != value.end(); ++it)
  {
    Json::Value model = *it;
    ModelIdentifier id;
    id.Name(model["name"].asString());
    id.Description(model["description"].asString());
    id.Downloads(model["downloads"].asUInt());
    id.FileSize(model["filesize"].asUInt());
    id.Likes(model["likes"].asUInt());
    id.Uuid(model["uuid"].asString());
    id.Category(model["category"].asString());
    id.ModifyDate(ParseDateTime(model["modify_date"].asString()));
    id.UploadDate(ParseDateTime(model["upload_date"].asString()));
    ids.push_back(id);
  }

  return ModelIterFactory::Create(ids);
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

  Json::StyledWriter writer;
  return writer.write(value);
}

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

#include <gtest/gtest.h>

#include <ctime>
#include <sstream>
#include <string>
#include <vector>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/JSONParser.hh"
#include "ignition/fuel_tools/ModelIter.hh"
#include "ignition/fuel_tools/ModelIterPrivate.hh"
#include "ignition/fuel_tools/WorldIter.hh"
#include "ignition/fuel_tools/WorldIterPrivate.hh"

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
/// \brief Convert JSON string to model iterator
TEST(JSONParser, ParseModels)
{
  std::stringstream tmpJsonStr;
  tmpJsonStr << "["
    << "{\"id\":1,\"createdAt\":\"2012-04-21T19:25:44.511Z\","
    << "\"updatedAt\":\"2012-04-23T18:25:43.511Z\","
    << "\"name\":\"car\","
    << "\"version\":3}]";

  ServerConfig srv;
  srv.SetUrl(common::URI("banana://testServer"));

  auto modelIds = JSONParser::ParseModels(tmpJsonStr.str(), srv);
  EXPECT_EQ(1u, modelIds.size());
  auto model = modelIds.front();
  EXPECT_EQ("car", model.Name());
  EXPECT_EQ(3u, model.Version());
  EXPECT_EQ("banana://testServer", model.Server().Url().Str());
  auto t = model.ModifyDate();
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%F %T", gmtime(&t));
  EXPECT_EQ("2012-04-23 18:25:43", std::string(buffer));
  t = model.UploadDate();
  std::strftime(buffer, sizeof(buffer), "%F %T", gmtime(&t));
  EXPECT_EQ("2012-04-21 19:25:44", std::string(buffer));
}

/////////////////////////////////////////////////
/// \brief Convert model iterator to JSON string
TEST(JSONParser, BuildModel)
{
  std::vector<ModelIdentifier> ids;
  ModelIdentifier id;
  id.SetName("house");
  id.SetVersion(5);
  id.SetDescription("affordable");
  ids.push_back(id);

  std::string jsonStr = JSONParser::BuildModel(ModelIterFactory::Create(ids));

  std::stringstream tmpJsonStr;
  tmpJsonStr
    << "{\n"
    << "\t\"description\" : \"affordable\",\n"
    << "\t\"name\" : \"house\",\n"
    << "\t\"version\" : 5\n"
    << "}";
  EXPECT_EQ(tmpJsonStr.str(), jsonStr);
}

/////////////////////////////////////////////////
TEST(JSONParser, ParseModel)
{
  std::stringstream tmpJsonStr;
  tmpJsonStr << "{\"id\":1,\"createdAt\":\"2012-04-21T19:25:44.511Z\","
    << "\"updatedAt\":\"2012-04-23T18:25:43.511Z\","
    << "\"name\":\"car\","
    << "\"version\":1,"
    << "\"tags\":[\"tag1\"]}";

  ServerConfig srv;
  srv.SetUrl(common::URI("http://testServer"));

  ModelIdentifier model = JSONParser::ParseModel(tmpJsonStr.str(), srv);
  EXPECT_EQ("car", model.Name());
  EXPECT_EQ(1u, model.Version());
  EXPECT_EQ("http://testServer", model.Server().Url().Str());
  auto t = model.ModifyDate();
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%F %T", gmtime(&t));
  EXPECT_EQ("2012-04-23 18:25:43", std::string(buffer));
  t = model.UploadDate();
  std::strftime(buffer, sizeof(buffer), "%F %T", gmtime(&t));
  EXPECT_EQ("2012-04-21 19:25:44", std::string(buffer));

  ASSERT_FALSE(model.Tags().empty());
  EXPECT_EQ("tag1", model.Tags()[0]);
}

/////////////////////////////////////////////////
/// \brief Convert JSON string to world iterator
TEST(JSONParser, ParseWorlds)
{
  std::stringstream tmpJsonStr;
  tmpJsonStr << "["
    << "{\"name\":\"car\","
    << "\"version\":3}]";

  ServerConfig srv;
  srv.SetUrl(common::URI("banana://testServer"));

  auto worldIds = JSONParser::ParseWorlds(tmpJsonStr.str(), srv);
  EXPECT_EQ(1u, worldIds.size());
  auto world = worldIds.front();
  EXPECT_EQ("car", world.Name());
  EXPECT_EQ(3u, world.Version());
  EXPECT_EQ("banana://testServer", world.Server().Url().Str());
}

/////////////////////////////////////////////////
/// \brief Convert world iterator to JSON string
TEST(JSONParser, BuildWorld)
{
  std::vector<WorldIdentifier> ids;
  WorldIdentifier id;
  id.SetName("house");
  id.SetVersion(5);
  ids.push_back(id);

  std::string jsonStr = JSONParser::BuildWorld(WorldIterFactory::Create(ids));

  std::stringstream tmpJsonStr;
  tmpJsonStr
    << "{\n"
    << "\t\"name\" : \"house\",\n"
    << "\t\"version\" : 5\n"
    << "}";
  EXPECT_EQ(tmpJsonStr.str(), jsonStr);
}

/////////////////////////////////////////////////
TEST(JSONParser, ParseWorld)
{
  std::stringstream tmpJsonStr;
  tmpJsonStr << "{\"name\":\"car\","
    << "\"version\":1]}";

  ServerConfig srv;
  srv.SetUrl(common::URI("banana://testServer"));

  WorldIdentifier world = JSONParser::ParseWorld(tmpJsonStr.str(), srv);
  EXPECT_EQ("car", world.Name());
  EXPECT_EQ(1u, world.Version());
  EXPECT_EQ("banana://testServer", world.Server().Url().Str());
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

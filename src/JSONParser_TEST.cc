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

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/JSONParser.hh"
#include "ignition/fuel-tools/ModelIter.hh"
#include "ignition/fuel-tools/ModelIterPrivate.hh"

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
    << "\"uuid\":\"3d3112d9-02b2-4b28-8d2f-f03be00a5a26\"}]";

  ServerConfig srv;
  srv.URL("testServer");

  auto modelIds = JSONParser::ParseModels(tmpJsonStr.str(), srv);
  EXPECT_EQ(1u, modelIds.size());
  auto model = modelIds.front();
  EXPECT_EQ("car", model.Name());
  EXPECT_EQ("testServer", model.Server().URL());
  auto t = model.ModifyDate();
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%F %T", gmtime(&t));
  EXPECT_EQ("2012-04-23 18:25:43", std::string(buffer));
  t = model.UploadDate();
  std::strftime(buffer, sizeof(buffer), "%F %T", gmtime(&t));
  EXPECT_EQ("2012-04-21 19:25:44", std::string(buffer));
  EXPECT_EQ("3d3112d9-02b2-4b28-8d2f-f03be00a5a26", model.Uuid());
}

/////////////////////////////////////////////////
/// \brief Convert model iterator to JSON string
TEST(JSONParser, BuildModel)
{
  std::vector<ModelIdentifier> ids;
  ModelIdentifier id;
  id.Name("house");
  id.Category("building");
  id.Description("affordable");
  id.Uuid("1234-0093asdf");
  ids.push_back(id);

  std::string jsonStr = JSONParser::BuildModel(ModelIterFactory::Create(ids));

  std::stringstream tmpJsonStr;
  tmpJsonStr
    << "{\n"
    << "   \"category\" : \"building\",\n"
    << "   \"description\" : \"affordable\",\n"
    << "   \"name\" : \"house\",\n"
    << "   \"uuid\" : \"1234-0093asdf\"\n"
    << "}\n";
  EXPECT_EQ(tmpJsonStr.str(), jsonStr);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

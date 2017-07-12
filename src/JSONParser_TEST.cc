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

#include "ignition/fuel-tools/JSONParser.hh"
#include "ignition/fuel-tools/ModelIter.hh"
#include "ignition/fuel-tools/ModelIterPrivate.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

/////////////////////////////////////////////////
/// \brief Convert JSON string to model iterator
TEST(JSONParser, ParseModels)
{
  std::stringstream tmpJsonStr;
  tmpJsonStr
  << "["
  << "  {"
  << "    \"category\" : \"transportation\","
  << "    \"description\" : \"I am a car\","
  << "    \"downloads\" : 5,"
  << "    \"filesize\" : 1024,"
  << "    \"images\" : ["
  << "      {"
  << "       \"id\" : \"222\","
  << "       \"url\" : \"car_image_url\""
  << "      }"
  << "    ],"
  << "    \"likes\" : 3,"
  << "    \"modify_date\" : \"2012-04-23T18:25:43.511Z\","
  << "    \"name\" : \"car\","
  << "    \"tags\" : ["
  << "      {"
  << "        \"id\" : \"111\","
  << "        \"name\" : \"car_tag_name\""
  << "      }"
  << "    ],"
  << "    \"upload_date\" : \"2012-04-21T19:25:44.511Z\","
  << "    \"uuid\" : \"1234-abcd\""
  << "  }"
  << "]";

  auto modelIt = JSONParser::ParseModels(tmpJsonStr.str());
  EXPECT_TRUE(modelIt);
  EXPECT_EQ("car", modelIt->Identification().Name());
  EXPECT_EQ("I am a car", modelIt->Identification().Description());
  EXPECT_EQ(1024, modelIt->Identification().FileSize());
  auto t = modelIt->Identification().ModifyDate();
  std::string str = std::asctime(gmtime(&t));
  EXPECT_EQ(str, "Mon Apr 23 18:25:43 2012\n");
  t = modelIt->Identification().UploadDate();
  str = std::asctime(gmtime(&t));
  EXPECT_EQ(str, "Sat Apr 21 19:25:44 2012\n");
  EXPECT_EQ("1234-abcd", modelIt->Identification().Uuid());
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

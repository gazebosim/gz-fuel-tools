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
#include "ignition/fuel-tools/ModelIdentifier.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


/////////////////////////////////////////////////
/// \brief Names can be set
TEST(ModelIdentifier, SetNames)
{
  ModelIdentifier id;
  EXPECT_TRUE(id.Name("hello"));
  EXPECT_TRUE(id.Owner("world"));
  EXPECT_TRUE(id.SourceURL("https://ignitionrobotics.org"));
  EXPECT_EQ(std::string("hello"), id.Name());
  EXPECT_EQ(std::string("world"), id.Owner());
  EXPECT_EQ(std::string("https://ignitionrobotics.org"), id.SourceURL());
}

/////////////////////////////////////////////////
/// \brief Copy constructor deep copies
TEST(ModelIdentifier, CopyConstructorDeepCopy)
{
  ModelIdentifier id;
  id.Name("hello");

  ModelIdentifier id2(id);
  EXPECT_EQ(std::string("hello"), id2.Name());

  id2.Name("hello2");
  EXPECT_EQ(std::string("hello"), id.Name());
  EXPECT_EQ(std::string("hello2"), id2.Name());
}

/////////////////////////////////////////////////
/// \brief assignment operator deep copies
TEST(ModelIdentifier, AssignmentOperatorDeepCopy)
{
  ModelIdentifier id;
  id.Name("hello");

  ModelIdentifier id2(id);
  id2 = id;
  EXPECT_EQ(std::string("hello"), id2.Name());

  id2.Name("hello2");
  EXPECT_EQ(std::string("hello"), id.Name());
  EXPECT_EQ(std::string("hello2"), id2.Name());
}

/////////////////////////////////////////////////
/// \brief Names can be set
TEST(ModelIdentifier, InvalidNames)
{
  ModelIdentifier id;
  EXPECT_FALSE(id.Name("Hello"));
  EXPECT_FALSE(id.Name("hEllo"));
  EXPECT_FALSE(id.Name("heLlo"));
  EXPECT_FALSE(id.Name("helLo"));
  EXPECT_FALSE(id.Name("hellO"));
  EXPECT_FALSE(id.Name("&hello"));
  EXPECT_FALSE(id.Name("h&ello"));
  EXPECT_FALSE(id.Name("he&llo"));
  EXPECT_FALSE(id.Name("hel&lo"));
  EXPECT_FALSE(id.Name("hell&o"));
  EXPECT_FALSE(id.Name("hello&"));
  EXPECT_FALSE(id.Name("=hello"));
  EXPECT_FALSE(id.Name(" hello"));
  EXPECT_FALSE(id.Name("\thello"));
  EXPECT_FALSE(id.Name("\nhello"));
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

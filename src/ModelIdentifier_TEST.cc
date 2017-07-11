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
  id.Name("Hello");
  id.UniqueName("World");
  EXPECT_EQ(std::string("Hello"), id.Name());
  EXPECT_EQ(std::string("World"), id.UniqueName());
}

/////////////////////////////////////////////////
/// \brief Copy constructor deep copies
TEST(ModelIdentifier, CopyConstructorDeepCopy)
{
  ModelIdentifier id;
  id.Name("Hello");
  id.UniqueName("World");

  ModelIdentifier id2(id);
  EXPECT_EQ(std::string("Hello"), id2.Name());
  EXPECT_EQ(std::string("World"), id2.UniqueName());

  id2.Name("Hello2");
  id2.UniqueName("World2");
  EXPECT_EQ(std::string("Hello"), id.Name());
  EXPECT_EQ(std::string("World"), id.UniqueName());
  EXPECT_EQ(std::string("Hello2"), id2.Name());
  EXPECT_EQ(std::string("World2"), id2.UniqueName());
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

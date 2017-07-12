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
/// \brief Fields can be set
TEST(ModelIdentifier, SetFields)
{
  ModelIdentifier id;
  id.Name("Hello");
  id.UniqueName("World");
  id.Category("test");
  id.Uuid("lllooo000ooolll");
  id.FileSize(2048u);
  std::time_t d1;
  std::time(&d1);
  id.ModifyDate(d1);
  std::time_t d2;
  std::time(&d2);
  id.UploadDate(d2);

  EXPECT_EQ(std::string("Hello"), id.Name());
  EXPECT_EQ(std::string("World"), id.UniqueName());
  EXPECT_EQ(std::string("test"), id.Category());
  EXPECT_EQ(std::string("lllooo000ooolll"), id.Uuid());
  EXPECT_EQ(2048u, id.FileSize());
  EXPECT_EQ(d1, id.ModifyDate());
  EXPECT_EQ(d2, id.UploadDate());
}

/////////////////////////////////////////////////
/// \brief Copy constructor deep copies
TEST(ModelIdentifier, CopyConstructorDeepCopy)
{
  ModelIdentifier id;
  id.Name("Hello");
  id.UniqueName("World");
  id.Category("test");
  id.Uuid("lllooo000ooolll");
  id.FileSize(2048u);
  std::time_t d1;
  std::time(&d1);
  id.ModifyDate(d1);
  std::time_t d2;
  std::time(&d2);
  id.UploadDate(d2);

  ModelIdentifier id2(id);
  EXPECT_EQ(std::string("Hello"), id2.Name());
  EXPECT_EQ(std::string("World"), id2.UniqueName());
  EXPECT_EQ(std::string("test"), id.Category());
  EXPECT_EQ(std::string("lllooo000ooolll"), id2.Uuid());
  EXPECT_EQ(2048u, id2.FileSize());
  EXPECT_EQ(d1, id2.ModifyDate());
  EXPECT_EQ(d2, id2.UploadDate());

  id2.Name("Hello2");
  id2.UniqueName("World2");
  EXPECT_EQ(std::string("Hello"), id.Name());
  EXPECT_EQ(std::string("World"), id.UniqueName());
  EXPECT_EQ(std::string("Hello2"), id2.Name());
  EXPECT_EQ(std::string("World2"), id2.UniqueName());
}

/////////////////////////////////////////////////
/// \brief assignment operator deep copies
TEST(ModelIdentifier, AssignmentOperatorDeepCopy)
{
  ModelIdentifier id;
  id.Name("Hello");
  id.UniqueName("World");
  id.Category("test");
  id.Uuid("lllooo000ooolll");
  id.FileSize(2048u);
  std::time_t d1;
  std::time(&d1);
  id.ModifyDate(d1);
  std::time_t d2;
  std::time(&d2);
  id.UploadDate(d2);

  ModelIdentifier id2(id);
  id2 = id;
  EXPECT_EQ(std::string("Hello"), id2.Name());
  EXPECT_EQ(std::string("World"), id2.UniqueName());
  EXPECT_EQ(std::string("test"), id.Category());
  EXPECT_EQ(std::string("lllooo000ooolll"), id2.Uuid());
  EXPECT_EQ(2048u, id2.FileSize());
  EXPECT_EQ(d1, id2.ModifyDate());
  EXPECT_EQ(d2, id2.UploadDate());


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

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
#include <cstdio>

/// \brief Forward Declaration
class ModelIterTestFixture;

#include "ignition/fuel-tools/ModelIter.hh"
#include "ignition/fuel-tools/ModelIterPrivate.hh"


namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Class to misuse friendship
    class ModelIterTest
    {
      public: static ModelIter EmptyModelIter()
        {
          return ModelIterFactory::Create();
        }

      public: static ModelIter ModelIterThreeModels()
        {
          std::vector<ModelIdentifier> ids;
          for (int i = 0; i < 3; i++)
          {
            ModelIdentifier id;
            char buf[10];
            std::sprintf(buf, "model%d", i);
            std::string name = buf;
            std::sprintf(buf, "owner%d", i);
            std::string owner = buf;
            id.Name(name);
            id.Owner(owner);
            id.SourceURL("https://ignitionrobotics.org");
            ids.push_back(id);
          }
          return ModelIterFactory::Create(ids);
        }
    };
  }
}

/////////////////////////////////////////////////
/// \brief No ids, iter should evaluate to false
TEST(ModelIterTestFixture, FalseIfNoModels)
{
  EXPECT_FALSE(ModelIterTest::EmptyModelIter());
}

/////////////////////////////////////////////////
/// \brief Iter should be ready to move if there are ids
TEST(ModelIterTestFixture, TrueIfSomeModels)
{
  EXPECT_TRUE(ModelIterTest::ModelIterThreeModels());
}

/////////////////////////////////////////////////
/// \brief Iter should move through 3 ids
TEST(ModelIterTestFixture, MoveThroughIds)
{
  ModelIter iter = ModelIterTest::ModelIterThreeModels();
  EXPECT_TRUE(iter);
  EXPECT_EQ("model0", iter->Identification().Name());
  EXPECT_EQ("model0", (*iter).Identification().Name());

  ++iter;
  EXPECT_TRUE(iter);
  EXPECT_EQ("model1", iter->Identification().Name());
  EXPECT_EQ("model1", (*iter).Identification().Name());

  ++iter;
  EXPECT_TRUE(iter);
  EXPECT_EQ("model2", iter->Identification().Name());
  EXPECT_EQ("model2", (*iter).Identification().Name());

  ++iter;
  EXPECT_FALSE(iter);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

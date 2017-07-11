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

/// \brief Class to misuse friendship
class ignft::FuelClient
{
  public: static ModelIter EmptyModelIter()
    {
      std::unique_ptr<ModelIterPrivate> priv(new ModelIterPrivate);
      return ModelIter(std::move(priv));
    }

  public: static ModelIter ModelIterThreeModels()
    {
      std::unique_ptr<ModelIterPrivate> priv(new ModelIterPrivate);
      for (int i = 0; i < 3; i++)
      {
        ModelIdentifier id;
        char buf[10];
        std::sprintf(buf, "%d", i);
        id.Name("model" + buf[0]);
        id.UniqueName("unique_model" + buf[0]);
      }
      return ModelIter(std::move(priv));
    }
};

/////////////////////////////////////////////////
/// \brief No ids, iter should evaluate to false
TEST(ModelIterTestFixture, FalseIfNoModels)
{
  EXPECT_FALSE(FuelClient::EmptyModelIter());
}

/////////////////////////////////////////////////
/// \brief Iter should move through 3 ids
TEST(ModelIterTestFixture, TrueIfSomeModels)
{
  EXPECT_TRUE(FuelClient::ModelIterThreeModels());
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

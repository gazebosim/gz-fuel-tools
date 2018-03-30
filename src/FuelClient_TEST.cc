/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/ClientConfig.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

/////////////////////////////////////////////////
/// \brief Nothing crashes
TEST(FuelClient, ParseModelUrl)
{
  FuelClient client;
  ServerConfig srv;
  ModelIdentifier id;
  const std::string modelUrl =
    "https://api.ignitionfuel.org/1.0/german/models/Cardboard Box";
  EXPECT_TRUE(client.ParseModelURL(modelUrl, srv, id));
  EXPECT_EQ(srv.Url(), "https://api.ignitionfuel.org");
  EXPECT_EQ(id.Owner(), "german");
  EXPECT_EQ(id.Name(), "Cardboard Box");

  // bad URL
  EXPECT_FALSE(client.ParseModelURL("bad url", srv, id));
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

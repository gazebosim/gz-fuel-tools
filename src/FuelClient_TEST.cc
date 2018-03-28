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
#include <ignition/common/Console.hh>
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/ClientConfig.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

/////////////////////////////////////////////////
/// \brief Nothing crashes
TEST(FuelClient, ParseModelURL)
{
  common::Console::SetVerbosity(4);

  // URL - without client config
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    const std::string modelURL{
      "https://api.ignitionfuel.org/1.0/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(modelURL, srv, id));

    EXPECT_EQ(srv.URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(srv.Version(), "1.0");
    EXPECT_TRUE(srv.LocalName().empty());
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
  }

  // URL - with client config
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ServerConfig srv;
    ModelIdentifier id;
    const std::string modelURL{
      "https://api.ignitionfuel.org/1.0/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(modelURL, srv, id));

    EXPECT_EQ(srv.URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(srv.Version(), "1.0");
    EXPECT_EQ(srv.LocalName(), "osrf");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
  }

  // URL - version different from config
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ServerConfig srv;
    ModelIdentifier id;
    const std::string modelURL{
      "https://api.ignitionfuel.org/5.0/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(modelURL, srv, id));

    EXPECT_EQ(srv.URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(srv.Version(), "1.0");
    EXPECT_EQ(srv.LocalName(), "osrf");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
  }

  // Unique name - without client config
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    const std::string modelUniqueName{
      "https://api.ignitionfuel.org/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(modelUniqueName, srv, id));

    EXPECT_EQ(srv.URL(), "https://api.ignitionfuel.org");
    EXPECT_TRUE(srv.Version().empty());
    EXPECT_TRUE(srv.LocalName().empty());
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
  }

  // Unique name - with client config
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ServerConfig srv;
    ModelIdentifier id;
    const std::string modelUniqueName{
      "https://api.ignitionfuel.org/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(modelUniqueName, srv, id));

    EXPECT_EQ(srv.URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(srv.Version(), "1.0");
    EXPECT_EQ(srv.LocalName(), "osrf");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
  }

  // Bad URL
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelURL("bad url", srv, id));
  }
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

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
#include <fstream>
#include <ignition/common/Filesystem.hh>
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/ClientConfig.hh"

#include "test/test_config.h"

#ifdef _WIN32
#include <direct.h>
#define ChangeDirectory _chdir
#else
#include <unistd.h>
#define ChangeDirectory chdir
#endif

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

/// \brief Creates a directory structure in the build directory with 3 models
/// Taken from LocalCache_TEST
void createLocal1(ClientConfig &_conf)
{
  if (common::exists("LocalCache_TEST1"))
  {
    common::removeAll("LocalCache_TEST1");
  }
  common::createDirectories("LocalCache_TEST1/alice/models/My Model");

  std::ofstream fout("LocalCache_TEST1/alice/models/My Model/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8007/");
  srv.LocalName("LocalCache_TEST1");
  _conf.AddServer(srv);
}

/////////////////////////////////////////////////
/// \brief Nothing crashes
TEST(FuelClient, ParseModelURL)
{
  FuelClient client;
  ServerConfig srv;
  ModelIdentifier id;
  const std::string modelURL =
    "https://api.ignitionfuel.org/1.0/german/models/Cardboard Box";
  EXPECT_TRUE(client.ParseModelURL(modelURL, srv, id));
  EXPECT_EQ(srv.URL(), "https://api.ignitionfuel.org");
  EXPECT_EQ(srv.Version(), "1.0");
  EXPECT_EQ(id.Owner(), "german");
  EXPECT_EQ(id.Name(), "Cardboard Box");

  // bad URL
  EXPECT_FALSE(client.ParseModelURL("bad url", srv, id));
}

/////////////////////////////////////////////////
TEST(FuelClient, CachedModel)
{
  // Configure to use binary path as cache and populate it
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  ClientConfig config;
  config.CacheLocation(common::cwd());
  createLocal1(config);

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Check cached model
  std::string url{
    "http://localhost:8007/1.0/alice/models/My Model"};
  std::string path;
  EXPECT_TRUE(client.CachedModel(url, path));
  EXPECT_EQ(common::cwd() + "/LocalCache_TEST1/alice/models/My Model", path);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

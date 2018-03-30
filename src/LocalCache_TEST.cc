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

#include <fstream>
#include <set>
#include <string>
#include <ignition/common/Filesystem.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/LocalCache.hh"

#include "test/test_config.h"

#ifdef _WIN32
#include <direct.h>
#define ChangeDirectory _chdir
#else
#include <unistd.h>
#define ChangeDirectory chdir
#endif

using namespace ignition;
using namespace fuel_tools;

/// \brief Creates a directory structure in the build directory with 6 models
void createLocal6(ClientConfig &_conf)
{
  if (common::exists("localhost:8001"))
  {
    common::removeAll("localhost:8001");
  }
  common::createDirectories("localhost:8001/alice/am1");
  common::createDirectories("localhost:8001/alice/am2");
  common::createDirectories("localhost:8001/bob/bm1");
  common::createDirectories("localhost:8001/bob/bm2");
  common::createDirectories("localhost:8001/trudy/tm1");
  common::createDirectories("localhost:8001/trudy/tm2");

  std::ofstream fout("localhost:8001/alice/am1/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  common::copyFile("localhost:8001/alice/am1/model.config",
                   "localhost:8001/alice/am2/model.config");
  common::copyFile("localhost:8001/alice/am1/model.config",
                   "localhost:8001/bob/bm1/model.config");
  common::copyFile("localhost:8001/alice/am1/model.config",
                   "localhost:8001/bob/bm2/model.config");
  common::copyFile("localhost:8001/alice/am1/model.config",
                   "localhost:8001/trudy/tm1/model.config");
  common::copyFile("localhost:8001/alice/am1/model.config",
                   "localhost:8001/trudy/tm2/model.config");

  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8001/");
  _conf.AddServer(srv);
}

/// \brief Creates a directory structure in the build directory with 3 models
void createLocal3(ClientConfig &_conf)
{
  if (common::exists("localhost:8007"))
  {
    common::removeAll("localhost:8007");
  }
  common::createDirectories("localhost:8007/alice/am1");
  common::createDirectories("localhost:8007/bob/bm1");
  common::createDirectories("localhost:8007/trudy/tm1");

  std::ofstream fout("localhost:8007/alice/am1/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  common::copyFile("localhost:8007/alice/am1/model.config",
                   "localhost:8007/bob/bm1/model.config");
  common::copyFile("localhost:8007/alice/am1/model.config",
                   "localhost:8007/trudy/tm1/model.config");

  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8007/");
  _conf.AddServer(srv);
}

/////////////////////////////////////////////////
/// \brief Iterate through all models in cache
TEST(LocalCache, AllModels)
{
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  ClientConfig conf;
  conf.CacheLocation(common::cwd());
  createLocal6(conf);
  createLocal3(conf);

  ignition::fuel_tools::LocalCache cache(&conf);

  auto iter = cache.AllModels();
  std::set<std::string> uniqueNames;
  while (iter)
  {
    uniqueNames.insert(iter->Identification().UniqueName());
    ++iter;
  }
  EXPECT_EQ(9u, uniqueNames.size());
}

/////////////////////////////////////////////////
/// \brief Get a specific model from cache
TEST(LocalCache, MatchingModels)
{
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  ClientConfig conf;
  conf.CacheLocation(common::cwd());
  createLocal6(conf);
  createLocal3(conf);

  ignition::fuel_tools::LocalCache cache(&conf);

  ModelIdentifier am1;
  am1.Owner("alice");
  am1.Name("am1");
  auto iter = cache.MatchingModels(am1);

  std::set<std::string> uniqueNames;
  while (iter)
  {
    uniqueNames.insert(iter->Identification().UniqueName());
    ++iter;
  }
  EXPECT_EQ(2u, uniqueNames.size());
}


/////////////////////////////////////////////////
/// \brief Get a specific model from cache
TEST(LocalCache, MatchingModel)
{
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  ClientConfig conf;
  conf.CacheLocation(common::cwd());
  createLocal6(conf);

  ignition::fuel_tools::LocalCache cache(&conf);

  ignition::fuel_tools::ServerConfig srv1;
  srv1.URL("http://localhost:8001/");

  ignition::fuel_tools::ServerConfig srv2;
  srv2.URL("http://localhost:8002/");

  ModelIdentifier am1;
  am1.Server(srv1);
  am1.Owner("alice");
  am1.Name("am1");
  EXPECT_TRUE(cache.MatchingModel(am1));

  ModelIdentifier tm2;
  tm2.Server(srv1);
  tm2.Owner("trudy");
  tm2.Name("tm2");
  EXPECT_TRUE(cache.MatchingModel(tm2));

  ModelIdentifier bogus1;
  bogus1.Server(srv2);
  bogus1.Owner("trudy");
  bogus1.Name("tm2");
  EXPECT_FALSE(cache.MatchingModel(bogus1));

  ModelIdentifier bogus2;
  bogus2.Server(srv1);
  bogus2.Owner("tfudy");
  bogus2.Name("tm2");
  EXPECT_FALSE(cache.MatchingModel(bogus2));

  ModelIdentifier bogus3;
  bogus3.Server(srv1);
  bogus3.Owner("trudy");
  bogus3.Name("tm3");
  EXPECT_FALSE(cache.MatchingModel(bogus3));
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

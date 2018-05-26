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
  common::createDirectories("test_cache/localhost:8001/alice/models/am1/2");
  common::createDirectories("test_cache/localhost:8001/alice/models/am2/1");
  common::createDirectories("test_cache/localhost:8001/bob/models/bm1/1");
  common::createDirectories("test_cache/localhost:8001/bob/models/bm2/2");
  common::createDirectories("test_cache/localhost:8001/trudy/models/tm1/3");
  common::createDirectories("test_cache/localhost:8001/trudy/models/tm2/2");

  std::ofstream fout(
      "test_cache/localhost:8001/alice/models/am1/2/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  common::copyFile(
      "test_cache/localhost:8001/alice/models/am1/2/model.config",
      "test_cache/localhost:8001/alice/models/am2/1/model.config");
  common::copyFile(
      "test_cache/localhost:8001/alice/models/am1/2/model.config",
      "test_cache/localhost:8001/bob/models/bm1/1/model.config");
  common::copyFile(
      "test_cache/localhost:8001/alice/models/am1/2/model.config",
      "test_cache/localhost:8001/bob/models/bm2/2/model.config");
  common::copyFile(
      "test_cache/localhost:8001/alice/models/am1/2/model.config",
      "test_cache/localhost:8001/trudy/models/tm1/3/model.config");
  common::copyFile(
      "test_cache/localhost:8001/alice/models/am1/2/model.config",
      "test_cache/localhost:8001/trudy/models/tm2/2/model.config");

  ignition::fuel_tools::ServerConfig srv;
  srv.SetUrl(common::URI("http://localhost:8001/"));
  _conf.AddServer(srv);
}

/// \brief Creates a directory structure in the build directory with 3 models
void createLocal3(ClientConfig &_conf)
{
  common::createDirectories("test_cache/localhost:8007/alice/models/am1/2");
  common::createDirectories("test_cache/localhost:8007/bob/models/bm1/1");
  common::createDirectories("test_cache/localhost:8007/trudy/models/tm1/3");

  std::ofstream fout(
      "test_cache/localhost:8007/alice/models/am1/2/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  common::copyFile(
      "test_cache/localhost:8007/alice/models/am1/2/model.config",
      "test_cache/localhost:8007/bob/models/bm1/1/model.config");
  common::copyFile(
      "test_cache/localhost:8007/alice/models/am1/2/model.config",
      "test_cache/localhost:8007/trudy/models/tm1/3/model.config");

  ignition::fuel_tools::ServerConfig srv;
  srv.SetUrl(common::URI("http://localhost:8007/"));
  _conf.AddServer(srv);
}

/////////////////////////////////////////////////
/// \brief Iterate through all models in cache
TEST(LocalCache, AllModels)
{
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig conf;
  conf.SetCacheLocation(common::cwd() + "/test_cache");
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

  EXPECT_NE(uniqueNames.end(), uniqueNames.find(
      "http://localhost:8001/alice/models/am1"));
}

/////////////////////////////////////////////////
/// \brief Get all models that match some fields
TEST(LocalCache, MatchingModels)
{
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig conf;
  conf.SetCacheLocation(common::cwd() + "/test_cache");
  createLocal6(conf);
  createLocal3(conf);

  ignition::fuel_tools::LocalCache cache(&conf);

  ModelIdentifier am1;
  am1.SetOwner("alice");
  am1.SetName("am1");
  auto iter = cache.MatchingModels(am1);

  std::set<std::string> uniqueNames;
  while (iter)
  {
    EXPECT_EQ("alice", iter->Identification().Owner());
    EXPECT_EQ("am1", iter->Identification().Name());

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
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig conf;
  conf.SetCacheLocation(common::cwd() + "/test_cache");
  createLocal6(conf);

  ignition::fuel_tools::LocalCache cache(&conf);

  ignition::fuel_tools::ServerConfig srv1;
  srv1.SetUrl(common::URI("http://localhost:8001/"));

  ignition::fuel_tools::ServerConfig srv2;
  srv2.SetUrl(common::URI("http://localhost:8002/"));

  ModelIdentifier am1;
  am1.SetServer(srv1);
  am1.SetOwner("alice");
  am1.SetName("am1");
  auto am1Model = cache.MatchingModel(am1);
  ASSERT_TRUE(am1Model);
  EXPECT_EQ("alice", am1Model.Identification().Owner());
  EXPECT_EQ("am1", am1Model.Identification().Name());
  EXPECT_EQ("http://localhost:8001",
      am1Model.Identification().Server().Url().Str());

  ModelIdentifier tm2;
  tm2.SetServer(srv1);
  tm2.SetOwner("trudy");
  tm2.SetName("tm2");
  EXPECT_TRUE(cache.MatchingModel(tm2));

  ModelIdentifier bogus1;
  bogus1.SetServer(srv2);
  bogus1.SetOwner("trudy");
  bogus1.SetName("tm2");
  EXPECT_FALSE(cache.MatchingModel(bogus1));

  ModelIdentifier bogus2;
  bogus2.SetServer(srv1);
  bogus2.SetOwner("tfudy");
  bogus2.SetName("tm2");
  EXPECT_FALSE(cache.MatchingModel(bogus2));

  ModelIdentifier bogus3;
  bogus3.SetServer(srv1);
  bogus3.SetOwner("trudy");
  bogus3.SetName("tm3");
  EXPECT_FALSE(cache.MatchingModel(bogus3));
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

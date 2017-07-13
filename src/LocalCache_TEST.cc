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
#include <ignition/common/Filesystem.hh>
#include "ignition/fuel-tools/LocalCache.hh"


namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


/// \brief Creates a directory structure in the build directory with 6 models
void createLocal6(ClientConfig &_conf)
{
  if (common::exists("LocalCache_TEST6"))
  {
    common::removeAll("LocalCache_TEST6");
  }
  common::createDirectories("LocalCache_TEST6/alice/am1");
  common::createDirectories("LocalCache_TEST6/alice/am2");
  common::createDirectories("LocalCache_TEST6/bob/bm1");
  common::createDirectories("LocalCache_TEST6/bob/bm2");
  common::createDirectories("LocalCache_TEST6/trudy/tm1");
  common::createDirectories("LocalCache_TEST6/trudy/tm2");

  std::ofstream fout("LocalCache_TEST6/alice/am1/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  common::copyFile("LocalCache_TEST6/alice/am1/model.config",
                   "LocalCache_TEST6/alice/am2/model.config");
  common::copyFile("LocalCache_TEST6/alice/am1/model.config",
                   "LocalCache_TEST6/bob/bm1/model.config");
  common::copyFile("LocalCache_TEST6/alice/am1/model.config",
                   "LocalCache_TEST6/bob/bm2/model.config");
  common::copyFile("LocalCache_TEST6/alice/am1/model.config",
                   "LocalCache_TEST6/trudy/tm1/model.config");
  common::copyFile("LocalCache_TEST6/alice/am1/model.config",
                   "LocalCache_TEST6/trudy/tm2/model.config");

  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8001/");
  srv.LocalName("LocalCache_TEST6");
  _conf.AddServer(srv);
}


/////////////////////////////////////////////////
/// \brief Fetch moels
TEST(LocalCache, AllModels)
{
  ClientConfig conf;
  conf.CacheLocation(common::cwd());
  createLocal6(conf);

  ignition::fuel_tools::LocalCache cache(&conf);

  auto iter = cache.AllModels();
  std::set<std::string> uniqueNames;
  while (iter)
  {
    uniqueNames.insert(iter->Identification().UniqueName());
    ++iter;
  }
  EXPECT_EQ(6u, uniqueNames.size());
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

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
#include <ignition/common/Filesystem.hh>
#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/Interface.hh"

#include "test/test_config.h"

#ifdef _WIN32
#include <direct.h>
#define ChangeDirectory _chdir
#else
#include <unistd.h>
#define ChangeDirectory chdir
#endif

using namespace ignition;
using namespace ignition::fuel_tools;

/////////////////////////////////////////////////
TEST(Interface, FetchResource)
{
  // Configure to use binary path as cache
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.SetCacheLocation(common::cwd() + "/test_cache");

  common::URI url{
    "https://fuel.ignitionrobotics.org/1.0/chapulina/models/Test box"};

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Check it is not cached
  std::string cachedPath;
  Result res1 = client.CachedModel(url, cachedPath);
  EXPECT_FALSE(res1) << "Cached Path: " << cachedPath;
  EXPECT_EQ(Result(ResultType::FETCH_ERROR), res1);

  // Download
  std::string path = fetchResourceWithClient(url.Str(), client);

  // Check it was downloaded to `2`
  EXPECT_EQ(path, common::cwd() +
      "/test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2");
  EXPECT_TRUE(common::exists(
      "test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2"));
  EXPECT_TRUE(common::exists(
      "test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2/"
      "model.sdf"));
  EXPECT_TRUE(common::exists(
      "test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2/"
      "model.config"));

  // Check it wasn't downloaded to model root directory
  EXPECT_FALSE(common::exists(
      "test_cache/fuel.ignitionrobotics.org/chapulina/models"
      "/Test box/model.config"));

  // Check it is cached
  Result res3 = client.CachedModel(url, cachedPath);
  EXPECT_TRUE(res3);
  EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), res3);
  EXPECT_EQ(common::cwd() +
      "/test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2",
      cachedPath);
}

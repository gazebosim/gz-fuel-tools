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
#include <ignition/common/Filesystem.hh>
#include <ignition/utilities/ExtraTestMacros.hh>
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
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(Interface, FetchResources)
{
  common::Console::SetVerbosity(4);

  // Configure to use binary path as cache
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  ASSERT_TRUE(common::createDirectories("test_cache"));
  ClientConfig config;
  config.SetCacheLocation(common::joinPaths(common::cwd(), "test_cache"));

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  std::string cachedPath;

  // Model
  {
    // Check it's not cached
    common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/1.0/chapulina/models/Test box"};
    {
      Result res = client.CachedModel(modelUrl, cachedPath);
      EXPECT_FALSE(res) << "Cached Path: " << cachedPath;
      EXPECT_EQ(ResultType::FETCH_ERROR, res.Type());
    }

    // Download model
    std::string path = fetchResourceWithClient(modelUrl.Str(), client);

    // Check it was downloaded to `2`
    EXPECT_EQ(path, common::joinPaths(common::cwd(), "test_cache",
      "fuel.ignitionrobotics.org", "chapulina", "models", "test box", "2"));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "chapulina", "models", "test box", "2")));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "chapulina", "models", "test box", "2", "model.sdf")));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "chapulina", "models", "test box", "2", "model.config")));

    // Check it wasn't downloaded to model root directory
    EXPECT_FALSE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "chapulina", "models", "Test box", "model.config")));

    // Check it is cached
    {
      Result res = client.CachedModel(modelUrl, cachedPath);
      EXPECT_TRUE(res);
      EXPECT_EQ(ResultType::FETCH_ALREADY_EXISTS, res.Type());
      EXPECT_EQ(common::joinPaths(common::cwd(), "test_cache",
        "fuel.ignitionrobotics.org", "chapulina", "models", "test box", "2"),
        cachedPath);
     }
  }

  // Model file
  {
    // Check neither file nor its model are cached
    common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/models/Bus/1/"};
    common::URI modelFileUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/models/Bus/1/files"
      "/meshes/bus.obj"};

    {
      Result res = client.CachedModel(modelUrl, cachedPath);
      EXPECT_FALSE(res) << "Cached Path: " << cachedPath;
      EXPECT_EQ(ResultType::FETCH_ERROR, res.Type());
    }
    {
      Result res = client.CachedModelFile(modelFileUrl, cachedPath);
      EXPECT_FALSE(res) << "Cached Path: " << cachedPath;
      EXPECT_EQ(ResultType::FETCH_ERROR, res.Type());
    }

    // Download model file
    std::string path = fetchResourceWithClient(modelFileUrl.Str(), client);

    // Check entire model was downloaded to `1`
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "openrobotics", "models", "bus", "1")));
    EXPECT_EQ(path, common::joinPaths(common::cwd(), "test_cache",
      "fuel.ignitionrobotics.org", "openrobotics", "models", "bus",
      "1", "meshes", "bus.obj"));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
        "openrobotics", "models", "bus", "1", "model.sdf")));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "openrobotics", "models", "bus", "1", "model.config")));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "openrobotics", "models", "bus", "1", "meshes/bus.obj")));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "openrobotics", "models", "bus", "1", "meshes/bus.mtl")));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "openrobotics", "models", "bus", "1", "materials", "textures",
          "bus.png")));

    // Check model is cached
    {
      Result res = client.CachedModel(modelUrl, cachedPath);
      EXPECT_TRUE(res);
      EXPECT_EQ(ResultType::FETCH_ALREADY_EXISTS, res.Type());
      EXPECT_EQ(common::joinPaths(common::cwd(), "test_cache",
        "fuel.ignitionrobotics.org", "openrobotics", "models", "bus", "1"),
        cachedPath);
     }

    // Check file is cached
    {
      Result res = client.CachedModelFile(modelFileUrl, cachedPath);
      EXPECT_TRUE(res);
      EXPECT_EQ(ResultType::FETCH_ALREADY_EXISTS, res.Type());
      EXPECT_EQ(common::joinPaths(common::cwd(), "test_cache",
        "fuel.ignitionrobotics.org", "openrobotics", "models", "bus", "1",
        "meshes", "bus.obj"), cachedPath);
     }
  }

  // World
  {
    // Check it's not cached
    common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Test world"};
    {
      Result res = client.CachedWorld(worldUrl, cachedPath);
      EXPECT_FALSE(res) << "Cached Path: " << cachedPath;
      EXPECT_EQ(ResultType::FETCH_ERROR, res.Type());
    }

    // Download world
    std::string path = fetchResourceWithClient(worldUrl.Str(), client);

    // Check it was downloaded to `1`
    EXPECT_EQ(path, common::joinPaths(common::cwd(), "test_cache",
      "fuel.ignitionrobotics.org", "openrobotics", "worlds",
      "test world", "2"));
    EXPECT_TRUE(common::exists(common::joinPaths("test_cache",
      "fuel.ignitionrobotics.org", "openrobotics", "worlds", "test world",
      "2")));
    EXPECT_TRUE(common::exists(common::joinPaths("test_cache",
      "fuel.ignitionrobotics.org", "openrobotics", "worlds", "test world", "2",
      "test.world")));

    // Check it is cached
    {
      Result res = client.CachedWorld(worldUrl, cachedPath);
      EXPECT_TRUE(res);
      EXPECT_EQ(ResultType::FETCH_ALREADY_EXISTS, res.Type());
      EXPECT_EQ(common::joinPaths(common::cwd(), "test_cache",
        "fuel.ignitionrobotics.org", "openrobotics", "worlds", "test world",
        "2"), cachedPath);
     }
  }

  // World file
  {
    // Check neither file nor its world are cached
    common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/1.0/chapulina/worlds/Test world/2/"};
    common::URI worldFileUrl{
      "https://fuel.ignitionrobotics.org/1.0/chapulina/worlds/Test world/2/"
      "files/thumbnails/1.png"};

    {
      Result res = client.CachedWorld(worldUrl, cachedPath);
      EXPECT_FALSE(res) << "Cached Path: " << cachedPath;
      EXPECT_EQ(ResultType::FETCH_ERROR, res.Type());
    }
    {
      Result res = client.CachedWorldFile(worldFileUrl, cachedPath);
      EXPECT_FALSE(res) << "Cached Path: " << cachedPath;
      EXPECT_EQ(ResultType::FETCH_ERROR, res.Type());
    }

    // Download world file
    std::string path = fetchResourceWithClient(worldFileUrl.Str(), client);

    // Check entire world was downloaded to `1`
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "chapulina", "worlds", "test world", "2")));
    EXPECT_EQ(path, common::joinPaths(common::cwd(), "test_cache",
      "fuel.ignitionrobotics.org", "chapulina", "worlds", "test world", "2",
      "thumbnails", "1.png"));
    EXPECT_TRUE(common::exists(
        common::joinPaths("test_cache", "fuel.ignitionrobotics.org",
          "chapulina", "worlds", "test world", "2", "test.world")));

    // Check world is cached
    {
      Result res = client.CachedWorld(worldUrl, cachedPath);
      EXPECT_TRUE(res);
      EXPECT_EQ(ResultType::FETCH_ALREADY_EXISTS, res.Type());
      EXPECT_EQ(common::joinPaths(common::cwd(), "test_cache",
        "fuel.ignitionrobotics.org", "chapulina", "worlds", "test world", "2"),
        cachedPath);
     }

    // Check file is cached
    {
      Result res = client.CachedWorldFile(worldFileUrl, cachedPath);
      EXPECT_TRUE(res);
      EXPECT_EQ(ResultType::FETCH_ALREADY_EXISTS, res.Type());
      EXPECT_EQ(common::joinPaths(common::cwd(), "test_cache",
        "fuel.ignitionrobotics.org", "chapulina", "worlds", "test world", "2",
        "thumbnails", "1.png"), cachedPath);
     }
  }
}

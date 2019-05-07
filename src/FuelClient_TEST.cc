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
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/Result.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"

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

//////////////////////////////////////////////////
/// \brief Creates a directory structure in the build directory with 1 model
/// that has 2 versions
/// Taken from LocalCache_TEST
void createLocalModel(ClientConfig &_conf)
{
  common::createDirectories(
      "test_cache/localhost:8007/alice/models/My Model/2/meshes");
  common::createDirectories(
      "test_cache/localhost:8007/alice/models/My Model/3/meshes");

  {
    std::ofstream fout(
        "test_cache/localhost:8007/alice/models/My Model/2/model.config",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/alice/models/My Model/2/model.config",
        "test_cache/localhost:8007/alice/models/My Model/3/model.config");
  }

  {
    std::ofstream fout(
        "test_cache/localhost:8007/alice/models/My Model/2/model.sdf",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/alice/models/My Model/2/model.sdf",
        "test_cache/localhost:8007/alice/models/My Model/3/model.sdf");
  }

  {
    std::ofstream fout(
        "test_cache/localhost:8007/alice/models/My Model/2/meshes/model.dae",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/alice/models/My Model/2/meshes/model.dae",
        "test_cache/localhost:8007/alice/models/My Model/3/meshes/model.dae");
  }

  ignition::fuel_tools::ServerConfig srv;
  srv.SetUrl(common::URI("http://localhost:8007/"));
  _conf.AddServer(srv);
}

//////////////////////////////////////////////////
/// \brief Creates a directory structure in the build directory with 1 world
/// that has 2 versions
/// Taken from LocalCache_TEST
void createLocalWorld(ClientConfig &_conf)
{
  common::createDirectories(
      "test_cache/localhost:8007/banana/worlds/My World/2");
  common::createDirectories(
      "test_cache/localhost:8007/banana/worlds/My World/3");


  {
    std::ofstream fout(
        "test_cache/localhost:8007/banana/worlds/My World/2/strawberry.world",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/banana/worlds/My World/2/strawberry.world",
        "test_cache/localhost:8007/banana/worlds/My World/3/strawberry.world");
  }

  ignition::fuel_tools::ServerConfig srv;
  srv.SetUrl(ignition::common::URI("http://localhost:8007/"));
  _conf.AddServer(srv);
}

/////////////////////////////////////////////////
/// \brief Nothing crashes
TEST(FuelClient, ParseModelURL)
{
  common::Console::SetVerbosity(4);

  // * without client config
  // * with server API version
  // * without model version
  {
    FuelClient client;
    ModelIdentifier id;
    const std::string url{
      "https://some.example.org/1.0/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelUrl(ignition::common::URI(url), id));

    EXPECT_EQ(id.Server().Url().Str(), "https://some.example.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // * with client config
  // * with server API version
  // * with model version
  {
    ClientConfig config;

    FuelClient client(config);
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/1.0/german/models/Cardboard Box/4"};
    EXPECT_TRUE(client.ParseModelUrl(ignition::common::URI(url), id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 4u);
  }

  // * with client config
  // * with server API version different from config
  // * with model version
  {
    ClientConfig config;

    FuelClient client(config);
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/5.0/german/models/Cardboard Box/6"};
    EXPECT_TRUE(client.ParseModelUrl(ignition::common::URI(url), id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 6u);
  }

  // * without client config
  // * without server API version
  // * without model version
  {
    FuelClient client;
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelUrl(ignition::common::URI(url), id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_FALSE(id.Server().Version().empty());
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // * with client config
  // * without server API version
  // * with model version `tip`
  {
    ClientConfig config;

    FuelClient client(config);
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/german/models/Cardboard Box/tip"};
    EXPECT_TRUE(client.ParseModelUrl(ignition::common::URI(url), id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // Bad model Urls
  {
    FuelClient client;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI("http://bad.url"),
          id));
  }

  // Not Url
  {
    FuelClient client;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI("bad url"),
          id));
  }
  {
    FuelClient client;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI("ba://url"),
          id));
  }
  {
    FuelClient client;
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/german/models/Cardboard Box/banana"};
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI(url), id));
  }
  {
    FuelClient client;
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/banana/german/models/Cardboard Box"};
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI(url), id));
  }
  {
    FuelClient client;
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/99/german/models/Cardboard Box"};
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI(url), id));
  }
  {
    FuelClient client;
    ModelIdentifier id;
    const std::string url{
      "https://fuel.ignitionrobotics.org/2/2/german/models"
        "/Cardboard Box/banana"};
    EXPECT_FALSE(client.ParseModelUrl(ignition::common::URI(url), id));
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, ParseModelFileURL)
{
  common::Console::SetVerbosity(4);

  // URL - without client config
  {
    FuelClient client;
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/models/"
        "Cordless Drill/tip/files/meshes/cordless_drill.dae"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Cordless Drill");
    EXPECT_EQ(filePath, "meshes/cordless_drill.dae");
  }

  // URL - with client config
  {
    ClientConfig config;

    FuelClient client(config);
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/models/Pine Tree/tip/"
      "files/materials/scripts/pine_tree.material"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Pine Tree");
    EXPECT_EQ(filePath, "materials/scripts/pine_tree.material");
  }

  // URL - version different from config
  {
    ClientConfig config;

    FuelClient client(config);
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/5.0/openrobotics/models/Pine Tree/tip/"
      "files/model.sdf"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Pine Tree");
    EXPECT_EQ(filePath, "model.sdf");
  }

  // Unique name - without client config
  {
    FuelClient client;
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/openrobotics/models/Pine Tree/tip/"
      "files/materials/scripts/pine_tree.material"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_FALSE(id.Server().Version().empty());
    EXPECT_EQ("1.0", id.Server().Version());
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Pine Tree");
    EXPECT_EQ(filePath, "materials/scripts/pine_tree.material");
  }

  // Unique name - with client config
  {
    ClientConfig config;

    FuelClient client(config);
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://fuel.ignitionrobotics.org/openrobotics/models/Pine Tree/tip/"
      "files/materials/scripts/pine_tree.material"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Pine Tree");
    EXPECT_EQ(filePath, "materials/scripts/pine_tree.material");
  }

  // Bad URL
  {
    FuelClient client;
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{"http://bad.url"};
    EXPECT_FALSE(client.ParseModelFileUrl(modelUrl, id, filePath));
  }

  // Not URL
  {
    FuelClient client;
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{"bad_url"};
    EXPECT_FALSE(client.ParseModelFileUrl(modelUrl, id, filePath));
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, DownloadModel)
{
  // Configure to use binary path as cache
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.SetCacheLocation(common::cwd() + "/test_cache");

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Download model from URL
  {
    // Unversioned URL should get the latest available version
    common::URI url{
        "https://fuel.ignitionrobotics.org/1.0/chapulina/models/Test box"};

    // Check it is not cached
    std::string cachedPath;
    Result res1 = client.CachedModel(url, cachedPath);
    EXPECT_FALSE(res1);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), res1);

    // Download
    std::string path;
    Result res2 = client.DownloadModel(url, path);
    EXPECT_TRUE(res2);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), res2);

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
     "test_cache/fuel.ignitionrobotics.org/chapulina/models/"
     "Test box/model.config"));

    // Check it is cached
    Result res3 = client.CachedModel(url, cachedPath);
    EXPECT_TRUE(res3);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), res3);
    EXPECT_EQ(common::cwd() +
      "/test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2",
      cachedPath);
  }

  // Download model with pbr paths from URL and check that paths are fixed
  {
    // Unversioned URL should get the latest available version
    common::URI url{
        "https://fuel.ignitionrobotics.org/1.0/iche033/models/Rescue Randy"};

    // Check it is not cached
    std::string cachedPath;
    Result res1 = client.CachedModel(url, cachedPath);
    EXPECT_FALSE(res1);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), res1);

    // Download
    std::string path;
    Result res2 = client.DownloadModel(url, path);
    EXPECT_TRUE(res2);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), res2);

    // Check it was downloaded to `2`
    EXPECT_EQ(path, common::cwd() +
        "/test_cache/fuel.ignitionrobotics.org/iche033/models/Rescue Randy/2");
    EXPECT_TRUE(common::exists(
        "test_cache/fuel.ignitionrobotics.org/iche033/models/Rescue Randy/2"));
    const std::string model_sdf_path =
        "test_cache/fuel.ignitionrobotics.org/iche033/models/Rescue Randy/2/"
         "model.sdf";
    EXPECT_TRUE(common::exists(model_sdf_path));
    EXPECT_TRUE(common::exists(
       "test_cache/fuel.ignitionrobotics.org/iche033/models/Rescue Randy/2/"
       "model.config"));

    // Check it wasn't downloaded to model root directory
    EXPECT_FALSE(common::exists(
     "test_cache/fuel.ignitionrobotics.org/iche033/models/"
     "Rescue Randy/model.config"));

    // Check it is cached
    Result res3 = client.CachedModel(url, cachedPath);
    EXPECT_TRUE(res3);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), res3);
    EXPECT_EQ(common::cwd() +
      "/test_cache/fuel.ignitionrobotics.org/iche033/models/Rescue Randy/2",
      cachedPath);

    // Check that pbr paths have been updated.
    std::ifstream ifs(model_sdf_path);
    std::string model_sdf((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
    EXPECT_EQ(std::string::npos, model_sdf.find("<albedo_map>model://"));
    EXPECT_EQ(std::string::npos, model_sdf.find("<normal_map>model://"));
    EXPECT_EQ(std::string::npos, model_sdf.find("<metalness_map>model://"));
    EXPECT_EQ(std::string::npos, model_sdf.find("<roughness_map>model://"));
  }

  // Try using nonexistent URL
  {
    std::string url{
        "https://fuel.ignitionrobotics.org/1.0/chapulina/models/"
          "Inexistent model"};
    std::string path;
    Result result = client.DownloadModel(common::URI(url), path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Try using bad URL
  {
    std::string url{"banana"};
    std::string path;
    Result result = client.DownloadModel(common::URI(url), path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, CachedModel)
{
  common::Console::SetVerbosity(4);

  // Configure to use binary path as cache and populate it
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.SetCacheLocation(common::cwd() + "/test_cache");
  createLocalModel(config);

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Cached model (no version)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/3", path);
  }

  // Cached model (tip)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/tip"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/3", path);
  }

  // Cached model (version number)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/2"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/2", path);
  }

  // Cached model file (tip)
  {
    common::URI url{
        "http://localhost:8007/1.0/alice/models/My Model/tip/files/model.sdf"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/3/model.sdf", path);
  }

  // Deeper cached model file
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/2/files/"
                    "meshes/model.dae"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/2/meshes/model.dae",
        path);
  }

  // Non-cached model
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/Banana"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Non-cached model (when looking for file)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/Banana/model.sdf"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Non-cached model file
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/tip/files/"
                    "meshes/banana.dae"
    };
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Model root URL to model file
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Bad model URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Bad model file URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }
}

/////////////////////////////////////////////////
/// \brief Nothing crashes
TEST(FuelClient, ParseWorldUrl)
{
  common::Console::SetVerbosity(4);

  // * without client config
  // * with server API version
  // * without world version
  {
    FuelClient client;
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/1.0/german/worlds/Cardboard Box"};
    EXPECT_TRUE(client.ParseWorldUrl(url, id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // * with client config
  // * with server API version
  // * with world version
  {
    ClientConfig config;

    FuelClient client(config);
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/1.0/german/worlds/Cardboard Box/4"};
    EXPECT_TRUE(client.ParseWorldUrl(url, id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 4u);
  }

  // * with client config
  // * with server API version different from config
  // * with world version
  {
    ClientConfig config;

    FuelClient client(config);
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/5.0/german/worlds/Cardboard Box/6"};
    EXPECT_TRUE(client.ParseWorldUrl(url, id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 6u);
  }

  // * without client config
  // * without server API version
  // * without world version
  {
    FuelClient client;
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/german/worlds/Cardboard Box"};
    EXPECT_TRUE(client.ParseWorldUrl(url, id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_FALSE(id.Server().Version().empty());
    EXPECT_EQ("1.0", id.Server().Version());
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // * with client config
  // * without server API version
  // * with world version `tip`
  {
    ClientConfig config;

    FuelClient client(config);
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/german/worlds/Cardboard Box/tip"};
    EXPECT_TRUE(client.ParseWorldUrl(url, id));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // Bad world URLs
  {
    FuelClient client;
    WorldIdentifier id;
    EXPECT_FALSE(client.ParseWorldUrl(common::URI("http://bad.url"), id));
  }

  // Not URL
  {
    FuelClient client;
    WorldIdentifier id;
    EXPECT_FALSE(client.ParseWorldUrl(common::URI("bad url"), id));
  }
  {
    FuelClient client;
    WorldIdentifier id;
    EXPECT_FALSE(client.ParseWorldUrl(common::URI("ba://url"), id));
  }
  {
    FuelClient client;
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/german/worlds/Cardboard Box/banana"};
    EXPECT_FALSE(client.ParseWorldUrl(url, id));
  }
  {
    FuelClient client;
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/banana/german/worlds/Cardboard Box"};
    EXPECT_FALSE(client.ParseWorldUrl(url, id));
  }
  {
    FuelClient client;
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/99/german/worlds/Cardboard Box"};
    EXPECT_FALSE(client.ParseWorldUrl(url, id));
  }
  {
    FuelClient client;
    WorldIdentifier id;
    const common::URI url{
      "https://fuel.ignitionrobotics.org/2/2/german/worlds/Cardboard Box"
        "/banana"};
    EXPECT_FALSE(client.ParseWorldUrl(url, id));
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, ParseWorldFileUrl)
{
  common::Console::SetVerbosity(4);

  // URL - without client config
  {
    FuelClient client;
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty/tip/"
      "files/empty.world"};
    EXPECT_TRUE(client.ParseWorldFileUrl(worldUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Empty");
    EXPECT_EQ(filePath, "empty.world");
  }

  // URL - with client config
  {
    ClientConfig config;

    FuelClient client(config);
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty sky/tip/"
      "files/empty_sky.world"};
    EXPECT_TRUE(client.ParseWorldFileUrl(worldUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Empty sky");
    EXPECT_EQ(filePath, "empty_sky.world");
  }

  // URL - version different from config
  {
    ClientConfig config;

    FuelClient client(config);
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/5.0/openrobotics/worlds/Empty/tip/"
      "files/empty.world"};
    EXPECT_TRUE(client.ParseWorldFileUrl(worldUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Empty");
    EXPECT_EQ(filePath, "empty.world");
  }

  // Unique name - without client config
  {
    FuelClient client;
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/openrobotics/worlds/Empty sky/tip/"
      "files/empty_sky.world"};
    EXPECT_TRUE(client.ParseWorldFileUrl(worldUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_FALSE(id.Server().Version().empty());
    EXPECT_EQ("1.0", id.Server().Version());
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Empty sky");
    EXPECT_EQ(filePath, "empty_sky.world");
  }

  // Unique name - with client config
  {
    ClientConfig config;

    FuelClient client(config);
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{
      "https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty/tip/"
      "files/empty.world"};
    EXPECT_TRUE(client.ParseWorldFileUrl(worldUrl, id, filePath));

    EXPECT_EQ(id.Server().Url().Str(), "https://fuel.ignitionrobotics.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Empty");
    EXPECT_EQ(filePath, "empty.world");
  }

  // Bad URL
  {
    FuelClient client;
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{"http://bad.url"};
    EXPECT_FALSE(client.ParseWorldFileUrl(worldUrl, id, filePath));
  }

  // Not URL
  {
    FuelClient client;
    WorldIdentifier id;
    std::string filePath;
    const common::URI worldUrl{"bad_url"};
    EXPECT_FALSE(client.ParseWorldFileUrl(worldUrl, id, filePath));
  }
}

//////////////////////////////////////////////////
TEST(FuelClient, DownloadWorld)
{
  common::Console::SetVerbosity(4);

  // Configure to use binary path as cache
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");

  ServerConfig server;
  server.SetUrl(ignition::common::URI(
        "https://staging-fuel.ignitionrobotics.org"));

  ClientConfig config;
  config.AddServer(server);
  config.SetCacheLocation(common::cwd() + "/test_cache");

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Download world from URL
  {
    // Unversioned URL should get the latest available version
    common::URI url{
        "https://staging-fuel.ignitionrobotics.org/1.0/nate/worlds/Empty"};

    // Check it is not cached
    std::string cachedPath;
    auto res1 = client.CachedWorld(url, cachedPath);
    EXPECT_FALSE(res1);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), res1);

    // Download
    std::string path;
    auto res2 = client.DownloadWorld(url, path);
    EXPECT_TRUE(res2);
    EXPECT_EQ(Result(ResultType::FETCH), res2);

    // Check it was downloaded to `1`
    EXPECT_EQ(path, common::cwd() +
        "/test_cache/staging-fuel.ignitionrobotics.org/nate/worlds/Empty/1");
    EXPECT_TRUE(common::exists(
        "test_cache/staging-fuel.ignitionrobotics.org/nate/worlds/Empty/1"));
    EXPECT_TRUE(common::exists(
       "test_cache/staging-fuel.ignitionrobotics.org/nate/worlds/Empty/1/"
       "empty.world"));

    // Check it wasn't downloaded to world root directory
    EXPECT_FALSE(common::exists(
          "test_cache/staging-fuel.ignitionrobotics.org/" +
          std::string("nate/worlds/Empty/empty.world")));

    // Check it is cached
    auto res3 = client.CachedWorld(url, cachedPath);
    EXPECT_TRUE(res3);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), res3);
    EXPECT_EQ(common::cwd() +
      "/test_cache/staging-fuel.ignitionrobotics.org/nate/worlds/Empty/1",
      cachedPath);
  }

  // Try using nonexistent URL
  {
    common::URI url{
        "https://staging-fuel.ignitionrobotics.org/1.0/nate/worlds/Bad world"};
    std::string path;
    auto result = client.DownloadWorld(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Try using bad URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.DownloadWorld(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, CachedWorld)
{
  common::Console::SetVerbosity(4);

  // Configure to use binary path as cache and populate it
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.SetCacheLocation(common::cwd() + "/test_cache");
  createLocalWorld(config);

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Cached world (no version)
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World"};
    std::string path;
    auto result = client.CachedWorld(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/banana/worlds/My World/3", path);
  }

  // Cached world (tip)
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World/tip"};
    std::string path;
    auto result = client.CachedWorld(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/banana/worlds/My World/3", path);
  }

  // Cached world (version number)
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World/2"};
    std::string path;
    auto result = client.CachedWorld(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/banana/worlds/My World/2", path);
  }

  // Cached world file (tip)
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World/tip/"
                    "files/strawberry.world"};
    std::string path;
    auto result = client.CachedWorldFile(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/banana/worlds/My World/3/strawberry.world",
        path);
  }

  // Deeper cached world file
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World/2/files/"
                    "strawberry.world"};
    std::string path;
    auto result = client.CachedWorldFile(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/banana/worlds/My World/2/strawberry.world",
        path);
  }

  // Non-cached world
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/Banana"};
    std::string path;
    auto result = client.CachedWorld(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Non-cached world (when looking for file)
  {
    common::URI url{
        "http://localhost:8007/1.0/banana/worlds/Banana/strawberry.world"};
    std::string path;
    auto result = client.CachedWorldFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Non-cached world file
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World/tip/"
                    "files/banana.sdf"};
    std::string path;
    auto result = client.CachedWorldFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // World root URL to world file
  {
    common::URI url{"http://localhost:8007/1.0/banana/worlds/My World"};
    std::string path;
    auto result = client.CachedWorldFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Bad world URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.CachedWorld(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }

  // Bad world file URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.CachedWorldFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(ResultType::FETCH_ERROR), result);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, Config)
{
  FuelClient client;
  ClientConfig &config = client.Config();

  // Check a few values. More client config tests in ClientConfig_TEST
  EXPECT_FALSE(config.UserAgent().empty());
  EXPECT_FALSE(config.CacheLocation().empty());
  EXPECT_FALSE(config.Servers().empty());
}

/////////////////////////////////////////////////
/// \brief Expect model download to fail with lack of server
TEST(FuelClient, ModelDownload)
{
  FuelClient client;

  std::string path;
  Result result = client.DownloadModel(common::URI("bad"), path);
  EXPECT_EQ(ResultType::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, ModelDetails)
{
  FuelClient client;
  ModelIdentifier modelId;
  ModelIdentifier model;

  Result result = client.ModelDetails(modelId, model);
  EXPECT_EQ(ResultType::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, Models)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  {
    ModelIter iter = client.Models(modelId);
    EXPECT_FALSE(iter);
  }

  {
    ModelIter const iter = client.Models(modelId);
    EXPECT_FALSE(iter);
  }

  {
    ModelIter iter = client.Models(serverConfig);
    EXPECT_TRUE(iter);
  }

  {
    serverConfig.Clear();
    ModelIter const iter = client.Models(serverConfig);
    EXPECT_FALSE(iter);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, DownloadModelFail)
{
  FuelClient client;
  ModelIdentifier modelId;

  Result result = client.DownloadModel(modelId);
  EXPECT_EQ(ResultType::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, DeleteModelFail)
{
  FuelClient client;
  ModelIdentifier modelId;

  Result result = client.DeleteModel(modelId);
  EXPECT_EQ(ResultType::DELETE_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, UploadModelFail)
{
  FuelClient client;
  ModelIdentifier modelId;

  Result result = client.UploadModel("path", modelId);
  EXPECT_EQ(ResultType::UPLOAD_ERROR, result.Type());
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

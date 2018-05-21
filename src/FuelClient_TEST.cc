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

//////////////////////////////////////////////////
/// \brief Creates a directory structure in the build directory with 1 model
/// Taken from LocalCache_TEST
void createLocal1(ClientConfig &_conf)
{
  common::createDirectories(
      "test_cache/localhost:8007/alice/models/My Model/tip/meshes");
  common::createDirectories(
      "test_cache/localhost:8007/alice/models/My Model/3/meshes");

  {
    std::ofstream fout(
        "test_cache/localhost:8007/alice/models/My Model/tip/model.config",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/alice/models/My Model/tip/model.config",
        "test_cache/localhost:8007/alice/models/My Model/3/model.config");
  }

  {
    std::ofstream fout(
        "test_cache/localhost:8007/alice/models/My Model/tip/model.sdf",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/alice/models/My Model/tip/model.sdf",
        "test_cache/localhost:8007/alice/models/My Model/3/model.sdf");
  }

  {
    std::ofstream fout(
        "test_cache/localhost:8007/alice/models/My Model/tip/meshes/model.dae",
        std::ofstream::trunc);
    fout << "<?xml version=\"1.0\"?>";
    fout.flush();
    fout.close();

    common::copyFile(
        "test_cache/localhost:8007/alice/models/My Model/tip/meshes/model.dae",
        "test_cache/localhost:8007/alice/models/My Model/3/meshes/model.dae");
  }

  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8007/");
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
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/1.0/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(url, srv, id));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
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
    config.LoadConfig();

    FuelClient client(config);
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/1.0/german/models/Cardboard Box/4"};
    EXPECT_TRUE(client.ParseModelURL(url, srv, id));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
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
    config.LoadConfig();

    FuelClient client(config);
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/5.0/german/models/Cardboard Box/6"};
    EXPECT_TRUE(client.ParseModelURL(url, srv, id));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
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
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/german/models/Cardboard Box"};
    EXPECT_TRUE(client.ParseModelURL(url, srv, id));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_TRUE(id.Server().Version().empty());
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // * with client config
  // * without server API version
  // * with model version `tip`
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/german/models/Cardboard Box/tip"};
    EXPECT_TRUE(client.ParseModelURL(url, srv, id));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "german");
    EXPECT_EQ(id.Name(), "Cardboard Box");
    EXPECT_EQ(id.Version(), 0u);
  }

  // Bad model URLs
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelURL("http://bad.url", srv, id));
  }

  // Not URL
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelURL("bad url", srv, id));
  }
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    EXPECT_FALSE(client.ParseModelURL("ba://url", srv, id));
  }
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/german/models/Cardboard Box/banana"};
    EXPECT_FALSE(client.ParseModelURL(url, srv, id));
  }
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/banana/german/models/Cardboard Box"};
    EXPECT_FALSE(client.ParseModelURL(url, srv, id));
  }
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/99/german/models/Cardboard Box"};
    EXPECT_FALSE(client.ParseModelURL(url, srv, id));
  }
  {
    FuelClient client;
    ServerConfig srv;
    ModelIdentifier id;
    const std::string url{
      "https://api.ignitionfuel.org/2/2/german/models/Cardboard Box/banana"};
    EXPECT_FALSE(client.ParseModelURL(url, srv, id));
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
      "https://api.ignitionfuel.org/1.0/openrobotics/models/Cordless Drill/tip/"
      "files/meshes/cordless_drill.dae"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Cordless Drill");
    EXPECT_EQ(filePath, "meshes/cordless_drill.dae");
  }

  // URL - with client config
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://api.ignitionfuel.org/1.0/openrobotics/models/Pine Tree/tip/"
      "files/materials/scripts/pine_tree.material"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Pine Tree");
    EXPECT_EQ(filePath, "materials/scripts/pine_tree.material");
  }

  // URL - version different from config
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://api.ignitionfuel.org/5.0/openrobotics/models/Pine Tree/tip/"
      "files/model.sdf"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
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
      "https://api.ignitionfuel.org/openrobotics/models/Pine Tree/tip/"
      "files/materials/scripts/pine_tree.material"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_TRUE(id.Server().Version().empty());
    EXPECT_EQ(id.Owner(), "openrobotics");
    EXPECT_EQ(id.Name(), "Pine Tree");
    EXPECT_EQ(filePath, "materials/scripts/pine_tree.material");
  }

  // Unique name - with client config
  {
    ClientConfig config;
    config.LoadConfig();

    FuelClient client(config);
    ModelIdentifier id;
    std::string filePath;
    const common::URI modelUrl{
      "https://api.ignitionfuel.org/openrobotics/models/Pine Tree/tip/"
      "files/materials/scripts/pine_tree.material"};
    EXPECT_TRUE(client.ParseModelFileUrl(modelUrl, id, filePath));

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
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
  common::removeDirectory(
      "test_cache/api.ignitionfuel.org/chapulina/models/Test box/tip");
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.LoadConfig();
  config.CacheLocation(common::cwd() + "/test_cache");

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Download model from URL
  {
    // Unversioned URL should get tip
    common::URI url{
        "https://api.ignitionfuel.org/1.0/chapulina/models/Test box"};

    // Check it is not cached
    std::string cachedPath;
    auto res1 = client.CachedModel(url, cachedPath);
    EXPECT_FALSE(res1);
    EXPECT_EQ(Result(Result::FETCH_ERROR), res1);

    // Download
    std::string path;
    auto res2 = client.DownloadModel(url.Str(), path);
    EXPECT_TRUE(res2);
    EXPECT_EQ(Result(Result::FETCH), res2);

    // The returned path says `tip`
    EXPECT_EQ(common::cwd() +
      "/test_cache/api.ignitionfuel.org/chapulina/models/Test box/tip", path);

    // Check it was downloaded to `1`
    EXPECT_TRUE(common::exists(
        "test_cache/api.ignitionfuel.org/chapulina/models/Test box/1"));
    EXPECT_TRUE(common::exists(
        "test_cache/api.ignitionfuel.org/chapulina/models/Test box/1/"
         "model.sdf"));
    EXPECT_TRUE(common::exists(
       "test_cache/api.ignitionfuel.org/chapulina/models/Test box/1/"
       "model.config"));

    // And `tip` also exists (symlink)
    EXPECT_TRUE(common::exists(
        "test_cache/api.ignitionfuel.org/chapulina/models/Test box/tip"));
    EXPECT_TRUE(common::exists(
        "test_cache/api.ignitionfuel.org/chapulina/models/Test box/tip/"
         "model.sdf"));
    EXPECT_TRUE(common::exists(
       "test_cache/api.ignitionfuel.org/chapulina/models/Test box/tip/"
       "model.config"));

    // Check it wasn't downloaded to model root directory
    EXPECT_FALSE(common::exists(
     "test_cache/api.ignitionfuel.org/chapulina/models/Test box/model.config"));

    // Check it is cached
    auto res3 = client.CachedModel(url, cachedPath);
    EXPECT_TRUE(res3);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), res3);
    EXPECT_EQ(common::cwd() +
      "/test_cache/api.ignitionfuel.org/chapulina/models/Test box/1",
      cachedPath);
  }

  // Try using nonexistent URL
  {
    std::string url{
        "https://api.ignitionfuel.org/1.0/chapulina/models/Inexistent model"};
    std::string path;
    auto result = client.DownloadModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Try using bad URL
  {
    std::string url{"banana"};
    std::string path;
    auto result = client.DownloadModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, CachedModel)
{
  common::Console::SetVerbosity(4);

  // Configure to use binary path as cache and populate it
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
  common::removeDirectory(
      "test_cache/api.ignitionfuel.org/chapulina/models/Test box/tip");
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.CacheLocation(common::cwd() + "/test_cache");
  createLocal1(config);

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Cached model (no version)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/tip", path);
  }

  // Cached model (tip)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/tip"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/tip", path);
  }

  // Cached model (version number)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/3"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/3", path);
  }

  // Cached model file
  {
    common::URI url{
        "http://localhost:8007/1.0/alice/models/My Model/tip/files/model.sdf"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/tip/model.sdf", path);
  }

  // Deeper cached model file
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/tip/files/"
                    "meshes/model.dae"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model/tip/meshes/model.dae",
        path);
  }

  // Non-cached model
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/Banana"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Non-cached model (when looking for file)
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/Banana/model.sdf"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Non-cached model file
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model/tip/files/"
                    "meshes/banana.dae"
    };
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Model root URL to model file
  {
    common::URI url{"http://localhost:8007/1.0/alice/models/My Model"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Bad model URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Bad model file URL
  {
    common::URI url{"banana"};
    std::string path;
    auto result = client.CachedModelFile(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, Config)
{
  FuelClient client;
  ClientConfig &config = client.Config();

  // Check a few values. More client config tests in ClientConfig_TEST
  EXPECT_FALSE(config.UserAgent().empty());
  EXPECT_TRUE(config.CacheLocation().empty());
  EXPECT_TRUE(config.Servers().empty());
}

/////////////////////////////////////////////////
/// \brief Expect model download to fail with lack of server
TEST(FuelClient, ModelDownload)
{
  FuelClient client;

  std::string path;
  Result result = client.DownloadModel("bad", path);
  EXPECT_EQ(Result::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, ModelDetails)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;
  ModelIdentifier model;

  Result result = client.ModelDetails(serverConfig, modelId, model);
  EXPECT_EQ(Result::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, Models)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  {
    ModelIter iter = client.Models(serverConfig, modelId);
    EXPECT_FALSE(iter);
  }

  {
    ModelIter const iter = client.Models(serverConfig, modelId);
    EXPECT_FALSE(iter);
  }

  {
    ModelIter iter = client.Models(serverConfig);
    EXPECT_FALSE(iter);
  }

  {
    ModelIter const iter = client.Models(serverConfig);
    EXPECT_FALSE(iter);
  }
}

/////////////////////////////////////////////////
TEST(FuelClient, DownloadModelFail)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  Result result = client.DownloadModel(serverConfig, modelId);
  EXPECT_EQ(Result::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, DeleteModelFail)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  Result result = client.DeleteModel(serverConfig, modelId);
  EXPECT_EQ(Result::DELETE_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, UploadModelFail)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  Result result = client.UploadModel(serverConfig, "path", modelId);
  EXPECT_EQ(Result::UPLOAD_ERROR, result.Type());
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

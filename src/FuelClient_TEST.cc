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
  common::createDirectories("test_cache/localhost:8007/alice/models/My Model");

  std::ofstream fout(
      "test_cache/localhost:8007/alice/models/My Model/model.config",
      std::ofstream::trunc);
  fout << "<?xml version=\"1.0\"?>";
  fout.flush();
  fout.close();

  ignition::fuel_tools::ServerConfig srv;
  srv.URL("http://localhost:8007/");
  _conf.AddServer(srv);
}

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

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
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

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
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

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
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

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_TRUE(id.Server().Version().empty());
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

    EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
    EXPECT_EQ(id.Server().Version(), "1.0");
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

/////////////////////////////////////////////////
TEST(FuelClient, DownloadModel)
{
  // Configure to use binary path as cache
  ASSERT_EQ(0, ChangeDirectory(PROJECT_BINARY_PATH));
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
    std::string url{
        "https://api.ignitionfuel.org/1.0/chapulina/models/Test box"};

    // Check it is not cached
    std::string cachedPath;
    auto res1 = client.CachedModel(url, cachedPath);
    EXPECT_FALSE(res1);
    EXPECT_EQ(Result(Result::FETCH_ERROR), res1);

    // Download
    std::string path;
    auto res2 = client.DownloadModel(url, path);
    EXPECT_TRUE(res2);
    EXPECT_EQ(Result(Result::FETCH), res2);
    EXPECT_EQ(common::cwd() +
      "/test_cache/api.ignitionfuel.org/chapulina/models/Test box", path);
    EXPECT_TRUE(common::exists(
      "test_cache/api.ignitionfuel.org/chapulina/models/Test box"));
    EXPECT_TRUE(common::exists(
      "test_cache/api.ignitionfuel.org/chapulina/models/Test box/model.sdf"));
    EXPECT_TRUE(common::exists(
     "test_cache/api.ignitionfuel.org/chapulina/models/Test box/model.config"));

    // Check it is cached
    auto res3 = client.CachedModel(url, cachedPath);
    EXPECT_TRUE(res3);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), res3);
    EXPECT_EQ(path, cachedPath);
  }

  // Try using inexistent URL
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
  common::removeAll("test_cache");
  common::createDirectories("test_cache");
  ClientConfig config;
  config.CacheLocation(common::cwd() + "/test_cache");
  createLocal1(config);

  // Create client
  FuelClient client(config);
  EXPECT_EQ(config.CacheLocation(), client.Config().CacheLocation());

  // Cached model
  {
    std::string url{"http://localhost:8007/1.0/alice/models/My Model"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_TRUE(result);
    EXPECT_EQ(Result(Result::FETCH_ALREADY_EXISTS), result);
    EXPECT_EQ(common::cwd() +
        "/test_cache/localhost:8007/alice/models/My Model", path);
  }

  // Non-cached model
  {
    std::string url{"http://localhost:8007/1.0/alice/models/Banana"};
    std::string path;
    auto result = client.CachedModel(url, path);
    EXPECT_FALSE(result);
    EXPECT_EQ(Result(Result::FETCH_ERROR), result);
  }

  // Bad URL
  {
    std::string url{"banana"};
    std::string path;
    auto result = client.CachedModel(url, path);
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
  ModelIter iter = client.Models(serverConfig, modelId);
  EXPECT_FALSE(iter);
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

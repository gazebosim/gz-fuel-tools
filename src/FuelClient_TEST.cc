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
#include "ignition/fuel_tools/FuelClient.hh"
#include "ignition/fuel_tools/ClientConfig.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

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
  EXPECT_EQ(id.Server().URL(), "https://api.ignitionfuel.org");
  EXPECT_EQ(id.Owner(), "german");
  EXPECT_EQ(id.Name(), "Cardboard Box");

  // bad URL
  EXPECT_FALSE(client.ParseModelURL("bad url", srv, id));
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
TEST(FuelClient, DownloadModel)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  Result result = client.DownloadModel(serverConfig, modelId);
  EXPECT_EQ(Result::FETCH_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, DeleteModel)
{
  FuelClient client;
  ServerConfig serverConfig;
  ModelIdentifier modelId;

  Result result = client.DeleteModel(serverConfig, modelId);
  EXPECT_EQ(Result::DELETE_ERROR, result.Type());
}

/////////////////////////////////////////////////
TEST(FuelClient, UploadModel)
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

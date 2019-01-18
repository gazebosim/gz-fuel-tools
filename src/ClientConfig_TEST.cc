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
#include <string>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>
#include "ignition/fuel_tools/ClientConfig.hh"
#include "test/test_config.h"

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
/// \brief Get home directory.
/// \return Home directory or empty string if home wasn't found.
/// \ToDo: Move this function to ignition::common::Filesystem
std::string homePath()
{
  std::string homePath;
#ifndef _WIN32
  ignition::common::env("HOME", homePath);
#else
  ignition::common::env("HOMEPATH", homePath);
#endif

  return homePath;
}

/////////////////////////////////////////////////
/// \brief Initially only the default server in config
TEST(ClientConfig, InitiallyDefaultServers)
{
  ClientConfig config;
  EXPECT_EQ(1u, config.Servers().size());
}

/////////////////////////////////////////////////
/// \brief Servers can be added
TEST(ClientConfig, ServersCanBeAdded)
{
  ClientConfig config;
  ServerConfig srv;
  srv.SetUrl(common::URI("http://asdf"));
  config.AddServer(srv);

  ASSERT_EQ(2u, config.Servers().size());
  EXPECT_EQ(std::string("http://asdf"), config.Servers().back().Url().Str());
}

/////////////////////////////////////////////////
/// \brief We can load the default configuration file.
TEST(ClientConfig, CustomDefaultConfiguration)
{
  ClientConfig config;
  ASSERT_EQ(1u, config.Servers().size());
  EXPECT_EQ("https://fuel.ignitionrobotics.org",
    config.Servers().front().Url().Str());

  std::string defaultCacheLocation = ignition::common::joinPaths(
    homePath(), ".ignition", "fuel");
  EXPECT_EQ(defaultCacheLocation, config.CacheLocation());
}

/////////////////////////////////////////////////
TEST(ClientConfig, UserAgent)
{
  ClientConfig config;
  EXPECT_EQ("IgnitionFuelTools-" IGNITION_FUEL_TOOLS_VERSION_FULL,
            config.UserAgent());

  config.SetUserAgent("my_user_agent");
  EXPECT_EQ("my_user_agent", config.UserAgent());
}

/////////////////////////////////////////////////
TEST(ServerConfig, ApiKey)
{
  ServerConfig config;
  EXPECT_TRUE(config.ApiKey().empty());

  config.SetApiKey("my_api_key");
  EXPECT_EQ("my_api_key", config.ApiKey());

  config.SetApiKey("my_other_api_key");
  EXPECT_EQ("my_other_api_key", config.ApiKey());
}

/////////////////////////////////////////////////
TEST(ClientConfig, AsString)
{
  common::Console::SetVerbosity(4);
  {
    ClientConfig client;

    auto str = client.AsString();
    igndbg << str << std::endl;

    EXPECT_NE(str.find(".ignition/fuel"), std::string::npos);
    EXPECT_NE(str.find("https://fuel.ignitionrobotics.org"), std::string::npos);
  }

  {
    ClientConfig client;
    client.Clear();
    std::string str = "Cache location: \nServers:\n";
    EXPECT_EQ(str, client.AsString());
  }

  {
    ServerConfig server;
    server.Clear();
    std::string str = "URL: \nVersion: 1.0\nAPI key: \n";
    EXPECT_EQ(str, server.AsString());
  }

  {
    ServerConfig srv;
    srv.SetUrl(common::URI("http://serverurl.com"));
    srv.SetVersion("2.0");
    srv.SetApiKey("ABCD");

    auto str = srv.AsString();
    igndbg << str << std::endl;

    EXPECT_NE(str.find("http://serverurl.com"), std::string::npos);
    EXPECT_EQ(str.find("local_name"), std::string::npos);
    EXPECT_NE(str.find("2.0"), std::string::npos);
    EXPECT_NE(str.find("ABCD"), std::string::npos);
  }

  {
    ClientConfig client;
    client.SetCacheLocation("cache/location");

    ServerConfig srv;
    srv.SetUrl(common::URI("http://serverurl.com"));
    client.AddServer(srv);

    auto str = client.AsString();
    igndbg << str << std::endl;

    EXPECT_NE(str.find("cache/location"), std::string::npos);
    EXPECT_NE(str.find("http://serverurl.com"), std::string::npos);
  }
}

/////////////////////////////////////////////////
TEST(ClientConfig, AsPrettyString)
{
  common::Console::SetVerbosity(4);

  {
    ServerConfig server;
    server.Clear();
    std::string str = "\x1B[96m\x1B[1mVersion: \x1B[0m\x1B[37m1.0\x1B[0m\n";
    EXPECT_EQ(str, server.AsPrettyString());
  }

  {
    ServerConfig srv;
    srv.SetUrl(common::URI("http://serverurl.com"));
    srv.SetVersion("2.0");
    srv.SetApiKey("ABCD");

    auto str = srv.AsPrettyString();
    igndbg << str << std::endl;

    EXPECT_NE(str.find("http://serverurl.com"), std::string::npos);
    EXPECT_EQ(str.find("local_name"), std::string::npos);
    EXPECT_NE(str.find("2.0"), std::string::npos);
    EXPECT_NE(str.find("ABCD"), std::string::npos);
  }
}

/////////////////////////////////////////////////
TEST(ServerConfig, Url)
{
  // Invalid URL string
  {
    ServerConfig srv;
    srv.SetUrl(common::URI("asdf"));
    EXPECT_TRUE(srv.Url().Str().empty());
  }

  // Valid URL
  {
    ServerConfig srv;
    srv.SetUrl(common::URI("http://banana:8080"));
    EXPECT_EQ("http://banana:8080", srv.Url().Str());
    EXPECT_EQ("http", srv.Url().Scheme());
    EXPECT_EQ("banana:8080", srv.Url().Path().Str());
  }

  // Trailing /
  {
    ServerConfig srv;
    srv.SetUrl(common::URI("http://banana:8080/"));
    EXPECT_EQ("http://banana:8080", srv.Url().Str());
  }

  // Set from URI
  {
    auto url = common::URI();
    url.SetScheme("http");
    url.Path() = common::URIPath("banana:8080");

    ServerConfig srv;
    srv.SetUrl(url);
    EXPECT_EQ("http://banana:8080", srv.Url().Str());
    EXPECT_EQ("http", srv.Url().Scheme());
    EXPECT_EQ("banana:8080", srv.Url().Path().Str());
  }
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


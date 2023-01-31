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
#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/common/Util.hh>
#include "gz/fuel_tools/ClientConfig.hh"
#include "test/test_config.h"

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
/// \brief Helper to remove file according to OS, while Windows
/// has this issue:
/// https://github.com/gazebosim/gz-common/issues/51
/// \todo(anyone) Remove this once Windows issue is solved.
/// \param[in] _path Path to file to be removed.
void removeFileTemp(const std::string &_path)
{
#ifndef _WIN32
  EXPECT_TRUE(common::removeFile(_path));
#else
  common::removeFile(_path);
#endif
}

/////////////////////////////////////////////////
/// \brief Get home directory.
/// \return Home directory or empty string if home wasn't found.
/// \ToDo: Move this function to common::Filesystem
std::string homePath()
{
  std::string homePath;
#ifndef _WIN32
  common::env("HOME", homePath);
#else
  common::env("USERPROFILE", homePath);
#endif

  return homePath;
}

/////////////////////////////////////////////////
/// \brief Get cache directory.
/// \return Cache directory
/// \ToDo: Move this function to common::Filesystem
std::string cachePath()
{
#ifndef _WIN32
  return std::string("/tmp/ignition/fuel");
#else
  return std::string("C:\\Windows\\Temp");
#endif
}

/////////////////////////////////////////////////
/// \brief Initially only the default server in config
TEST(ClientConfig, InitiallyDefaultServers)
{
  ClientConfig config;
  EXPECT_EQ(2u, config.Servers().size());
}

/////////////////////////////////////////////////
/// \brief Servers can be added
TEST(ClientConfig, ServersCanBeAdded)
{
  ClientConfig config;
  ServerConfig srv;
  srv.SetUrl(common::URI("http://asdf"));
  config.AddServer(srv);

  ASSERT_EQ(3u, config.Servers().size());
  EXPECT_EQ(std::string("http://asdf"), config.Servers().back().Url().Str());
}

/////////////////////////////////////////////////
/// \brief We can load the default configuration file.
TEST(ClientConfig, CustomDefaultConfiguration)
{
  ClientConfig config;
  ASSERT_EQ(2u, config.Servers().size());
  EXPECT_EQ("https://fuel.gazebosim.org",
    config.Servers().front().Url().Str());
  EXPECT_EQ("https://fuel.ignitionrobotics.org",
    config.Servers()[1].Url().Str());

  std::string defaultCacheLocation = common::joinPaths(
    homePath(), ".ignition", "fuel");
  EXPECT_EQ(defaultCacheLocation, config.CacheLocation());
}

/////////////////////////////////////////////////
/// \brief We can load custom settings in a configuration file.
TEST(ClientConfig, CustomConfiguration)
{
  ClientConfig config;

  // Create a temporary file with the configuration.
  std::ofstream ofs;
  std::string testPath = "test_conf.yaml";
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"                                    << std::endl
      << "# The list of servers."                 << std::endl
      << "servers:"                               << std::endl
      << "  -"                                    << std::endl
      << "    url: https://api.ignitionfuel.org"  << std::endl
      << ""                                       << std::endl
      << "  -"                                    << std::endl
      << "    url: https://myserver"              << std::endl
      << ""                                       << std::endl
      << "# Where are the assets stored in disk." << std::endl
      << "cache:"                                 << std::endl
      << "  path: " + cachePath()                 << std::endl
      << std::endl;
  ofs.close();

  EXPECT_TRUE(config.LoadConfig(testPath));

  ASSERT_EQ(4u, config.Servers().size());
  EXPECT_EQ("https://fuel.gazebosim.org",
    config.Servers().front().Url().Str());
  EXPECT_EQ("https://fuel.ignitionrobotics.org",
    config.Servers()[1].Url().Str());
  EXPECT_EQ("https://api.ignitionfuel.org",
    config.Servers()[2].Url().Str());
  EXPECT_EQ("https://myserver",
    config.Servers().back().Url().Str());

  EXPECT_EQ(cachePath(), config.CacheLocation());
  // Remove the configuration file.
  removeFileTemp(testPath);
}

/////////////////////////////////////////////////
/// \brief A server contains an already used URL.
TEST(ClientConfig, RepeatedServerConfiguration)
{
  ClientConfig config;

  // Create a temporary file with the configuration.
  std::ofstream ofs;
  std::string testPath = "test_conf.yaml";
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"                                    << std::endl
      << "# The list of servers."                 << std::endl
      << "servers:"                               << std::endl
      << "  -"                                    << std::endl
      << "    url: https://fuel.gazebosim.org"  << std::endl
      << ""                                       << std::endl
      << "  -"                                    << std::endl
      << "    url: https://fuel.gazebosim.org"  << std::endl
      << ""                                       << std::endl
      << "# Where are the assets stored in disk." << std::endl
      << "cache:"                                 << std::endl
      << "  path: " + cachePath()                 << std::endl
      << std::endl;
  ofs.close();

  EXPECT_TRUE(config.LoadConfig(testPath));

  // Remove the configuration file.
  removeFileTemp(testPath);
}

/////////////////////////////////////////////////
/// \brief A server without URL is not valid.
TEST(ClientConfig, NoServerUrlConfiguration)
{
  ClientConfig config;

  // Create a temporary file with the configuration.
  std::ofstream ofs;
  std::string testPath = "test_conf.yaml";
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"                                    << std::endl
      << "# The list of servers."                 << std::endl
      << "servers:"                               << std::endl
      << "  -"                                    << std::endl
      << "    banana: coconut"                           << std::endl
      << std::endl;
  ofs.close();

  EXPECT_FALSE(config.LoadConfig(testPath));

  // Remove the configuration file.
  removeFileTemp(testPath);
}

/////////////////////////////////////////////////
/// \brief A server with an empty URL is not valid.
TEST(ClientConfig, EmptyServerUrlConfiguration)
{
  ClientConfig config;

  // Create a temporary file with the configuration.
  std::ofstream ofs;
  std::string testPath = "test_conf.yaml";
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"                                    << std::endl
      << "# The list of servers."                 << std::endl
      << "servers:"                               << std::endl
      << "  -"                                    << std::endl
      << "    url: "                              << std::endl
      << std::endl;
  ofs.close();

  EXPECT_FALSE(config.LoadConfig(testPath));

  // Remove the configuration file.
  removeFileTemp(testPath);
}

/////////////////////////////////////////////////
/// \brief The "cache" option requires to set "path".
TEST(ClientConfig, NoCachePathConfiguration)
{
  ClientConfig config;

  // Create a temporary file with the configuration.
  std::ofstream ofs;
  std::string testPath = "test_conf.yaml";
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"    << std::endl
      << "cache:" << std::endl
      << std::endl;
  ofs.close();

  EXPECT_FALSE(config.LoadConfig(testPath));

  // Remove the configuration file.
  removeFileTemp(testPath);
}

/////////////////////////////////////////////////
/// \brief The path parameter cannot be empty.
TEST(ClientConfig, EmptyCachePathConfiguration)
{
  ClientConfig config;

  // Create a temporary file with the configuration.
  std::ofstream ofs;
  std::string testPath = "test_conf.yaml";
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"     << std::endl
      << "cache:"  << std::endl
      << "  path:" << std::endl
      << std::endl;
  ofs.close();

  EXPECT_FALSE(config.LoadConfig(testPath));

  // Remove the configuration file.
  removeFileTemp(testPath);
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

    std::string str = client.AsString();
    igndbg << str << std::endl;

#ifndef _WIN32
    EXPECT_NE(str.find(".ignition/fuel"), std::string::npos);
#else
    EXPECT_NE(str.find(".ignition\\fuel"), std::string::npos);
#endif
    EXPECT_NE(str.find("https://fuel.gazebosim.org"), std::string::npos);
  }

  {
    ClientConfig client;
    client.Clear();
    std::string str = "Config path: \nCache location: \nServers:\n";
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
    EXPECT_EQ("http", srv.Url().Scheme());
    EXPECT_EQ("banana:8080", srv.Url().Path().Str());
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

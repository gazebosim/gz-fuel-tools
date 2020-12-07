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
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <ignition/common/Filesystem.hh>
#include <ignition/utilities/ExtraTestMacros.hh>

#include "ign.hh"
#include "test/test_config.h"  // NOLINT(build/include)

using namespace ignition;

// Std streams
static std::streambuf *g_stdOutFile;
static std::streambuf *g_stdErrFile;

/////////////////////////////////////////////////
// \brief Redirect stdout and stderr to streams.
void redirectIO(std::stringstream &_stdOutBuffer,
                std::stringstream &_stdErrBuffer)
{
  g_stdOutFile = std::cout.rdbuf(_stdOutBuffer.rdbuf());
  g_stdErrFile = std::cerr.rdbuf(_stdErrBuffer.rdbuf());
}

/////////////////////////////////////////////////
// \brief Clear all streams (including state flags).
void clearIOStreams(std::stringstream &_stdOutBuffer,
                    std::stringstream &_stdErrBuffer)
{
  _stdOutBuffer.str("");
  _stdOutBuffer.clear();
  _stdErrBuffer.str("");
  _stdErrBuffer.clear();
}

/////////////////////////////////////////////////
/// \brief Restore stdout and stderr redirections.
void restoreIO()
{
  std::cout.rdbuf(g_stdOutFile);
  std::cerr.rdbuf(g_stdErrFile);
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListFail)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_FALSE(listModels("fake_url"));

  EXPECT_NE(stdOutBuffer.str().find("Invalid URL"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(CmdLine, IGN_UTILS_TEST_DISABLED_ON_WIN32(ModelListConfigServerUgly))
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listModels("", "", "true"));

  EXPECT_NE(stdOutBuffer.str().find("https://fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find("owners"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(CmdLine, IGN_UTILS_TEST_DISABLED_ON_WIN32(ModelListConfigServerPretty))
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listModels("https://staging-fuel.ignitionrobotics.org"));

  EXPECT_NE(stdOutBuffer.str().find(
        "https://staging-fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("owners"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("models"), std::string::npos)
      << stdOutBuffer.str();

  EXPECT_EQ(stdOutBuffer.str().find("https://fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find(
      "https://staging-fuel.ignitionrobotics.org/1.0/"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(CmdLine, IGN_UTILS_TEST_DISABLED_ON_WIN32(
    ModelListConfigServerPrettyOwner))
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listModels("https://staging-fuel.ignitionrobotics.org",
      "OpenRobotics"));

  EXPECT_NE(stdOutBuffer.str().find(
        "https://staging-fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("1 owners"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("models"), std::string::npos)
      << stdOutBuffer.str();

  // If pagination fails, we only get the first 20 models
  EXPECT_EQ(stdOutBuffer.str().find("20 models"), std::string::npos)
      << stdOutBuffer.str();

  EXPECT_EQ(stdOutBuffer.str().find("https://fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find(
      "https://staging-fuel.ignitionrobotics.org/1.0/"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelDownloadBadUrl)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_FALSE(downloadUrl("fake_url"));

  EXPECT_NE(stdOutBuffer.str().find("Malformed URL"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelDownloadWrongUrl)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_FALSE(downloadUrl(
      "https://site.com/1.0/ownername/modelname"));

  EXPECT_NE(stdOutBuffer.str().find("Invalid URL"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(CmdLine, IGN_UTILS_TEST_DISABLED_ON_WIN32(ModelDownloadUnversioned))
{
  cmdVerbosity("4");

  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");
  setenv("IGN_FUEL_CACHE_PATH", "test_cache", true);

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(downloadUrl(
      "https://fuel.ignitionrobotics.org/1.0/chapulina/models/Test box"));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty()) << stdErrBuffer.str();

  // Check files
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/chapulina/models/Test box/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/chapulina/models") +
      "/Test box/2/model.sdf"));

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(CmdLine, IGN_UTILS_TEST_DISABLED_ON_WIN32(DownloadConfigCache))
{
  cmdVerbosity("4");

  unsetenv("IGN_FUEL_CACHE_PATH");
  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");

  // Test config
  std::ofstream ofs;
  std::string testPath = common::joinPaths(
      std::string(PROJECT_BINARY_PATH), "test_cache", "test_conf.yaml");
  ofs.open(testPath, std::ofstream::out | std::ofstream::app);

  ofs << "---"                                    << std::endl
      << "servers:"                               << std::endl
      << "  -"                                    << std::endl
      << "    url: https://fuel.ignitionrobotics.org"  << std::endl
      << ""                                       << std::endl
      << "cache:"                                 << std::endl
      << "  path: " << PROJECT_BINARY_PATH << "/test_cache" << std::endl
      << std::endl;
  ofs.close();

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(downloadUrl(
      "https://fuel.ignitionrobotics.org/1.0/chapulina/models/Test box",
      testPath.c_str()));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty()) << stdErrBuffer.str();

  // Check files
  auto modelPath = common::joinPaths(std::string(PROJECT_BINARY_PATH),
      "test_cache", "fuel.ignitionrobotics.org", "chapulina", "models",
      "Test box");
  EXPECT_TRUE(ignition::common::isDirectory(modelPath));
  EXPECT_TRUE(ignition::common::isDirectory(common::joinPaths(modelPath, "2")));
  EXPECT_TRUE(ignition::common::isFile(common::joinPaths(modelPath, "2",
      "model.sdf")));

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, WorldListFail)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_FALSE(listWorlds("fake_url"));

  EXPECT_NE(stdOutBuffer.str().find("Invalid URL"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(FuelClientTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(
    WorldListConfigServerUgly))
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listWorlds(
        "https://staging-fuel.ignitionrobotics.org", "", "true"));

  EXPECT_NE(stdOutBuffer.str().find(
        "https://staging-fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find("owners"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(FuelClientTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(
    WorldListConfigServerPretty))
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listWorlds("https://staging-fuel.ignitionrobotics.org"));

  EXPECT_NE(stdOutBuffer.str().find(
        "https://staging-fuel.ignitionrobotics.org"), std::string::npos)
    << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("owners"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("worlds"), std::string::npos)
      << stdOutBuffer.str();

  EXPECT_EQ(stdOutBuffer.str().find("https://fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find(
      "https://staging-fuel.ignitionrobotics.org/1.0/"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, WorldListCustomServerPrettyOwner)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listWorlds("https://staging-fuel.ignitionrobotics.org",
      "OpenRobotics"));

  EXPECT_NE(stdOutBuffer.str().find(
        "https://staging-fuel.ignitionrobotics.org"), std::string::npos)
    << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("worlds"), std::string::npos)
      << stdOutBuffer.str();

  // If pagination fails, we only get the first 20 worlds
  EXPECT_EQ(stdOutBuffer.str().find("20 worlds"), std::string::npos)
      << stdOutBuffer.str();

  EXPECT_EQ(stdOutBuffer.str().find("https://fuel.ignitionrobotics.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find(
      "https://staging-fuel.ignitionrobotics.org/1.0/"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(FuelClientTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(
    WorldDownloadBadUrl))
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_FALSE(downloadUrl("fake_url"));

  EXPECT_NE(stdOutBuffer.str().find("Malformed URL"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, WorldDownloadWrongUrl)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_FALSE(downloadUrl(
      "https://site.com/1.0/ownername/worldname"));

  EXPECT_NE(stdOutBuffer.str().find("Invalid URL"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST(FuelClientTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(
    WorldDownloadUnversioned))
{
  cmdVerbosity("4");

  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");
  setenv("IGN_FUEL_CACHE_PATH", "test_cache", true);

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(downloadUrl(
      "https://fuel.ignitionrobotics.org/1.0/OpenRobotics/worlds/Test world"));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  // Check files
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/")
      + "Test world/2/test.world"));

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

class DownloadCollectionTest
    : public ::testing::Test,
      public ::testing::WithParamInterface<int>
{};

INSTANTIATE_TEST_CASE_P(CollectionTest, DownloadCollectionTest,
                        ::testing::Values(1, 2, 16)); // NOLINT

/////////////////////////////////////////////////
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST_P(DownloadCollectionTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(AllItems))
{
  cmdVerbosity("4");

  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");
  setenv("IGN_FUEL_CACHE_PATH", "test_cache", true);

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(
      downloadUrl("https://fuel.ignitionrobotics.org/1.0/OpenRobotics/"
                  "collections/TestCollection",
                  nullptr, nullptr, nullptr, GetParam()));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  // Check files
  // Model: Backpack
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/Backpack"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/Backpack/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/") +
      "Backpack/2/model.sdf"));

  // Model: TEAMBASE
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/TEAMBASE"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/TEAMBASE/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/") +
      "TEAMBASE/2/model.sdf"));

  // World: Test World
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/") +
      "Test world/2/test.world"));

  // World: Test World 2
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world 2"));
  EXPECT_TRUE(
      ignition::common::isDirectory("test_cache/fuel.ignitionrobotics.org/"
                                    "OpenRobotics/worlds/Test world 2/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/") +
      "Test world 2/2/test.world"));
  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
/// Download only models
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST_P(DownloadCollectionTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(Models))
{
  cmdVerbosity("4");

  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");
  setenv("IGN_FUEL_CACHE_PATH", "test_cache", true);

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(
      downloadUrl("https://fuel.ignitionrobotics.org/1.0/OpenRobotics/"
                  "collections/TestCollection",
                  nullptr, nullptr, "model", GetParam()));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  // Check files
  // Model: Backpack
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/Backpack"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/Backpack/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/") +
      "Backpack/2/model.sdf"));

  // Model: TEAMBASE
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/TEAMBASE"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/TEAMBASE/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/") +
      "TEAMBASE/2/model.sdf"));

  // World: Test World
  EXPECT_FALSE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world"));

  // World: Test World 2
  EXPECT_FALSE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world 2"));
  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
/// Download only worlds
// Protocol "https" not supported or disabled in libcurl for Windows
// https://github.com/ignitionrobotics/ign-fuel-tools/issues/105
TEST_P(DownloadCollectionTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(Worlds))
{
  cmdVerbosity("4");

  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");
  setenv("IGN_FUEL_CACHE_PATH", "test_cache", true);

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(
      downloadUrl("https://fuel.ignitionrobotics.org/1.0/OpenRobotics/"
                  "collections/TestCollection",
                  nullptr, nullptr, "world", GetParam()));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  // Check files
  // Model: Backpack
  EXPECT_FALSE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/Backpack"));

  // Model: TEAMBASE
  EXPECT_FALSE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/models/TEAMBASE"));

  // World: Test World
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world"));
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/") +
      "Test world/2/test.world"));

  // World: Test World 2
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/Test world 2"));
  EXPECT_TRUE(
      ignition::common::isDirectory("test_cache/fuel.ignitionrobotics.org/"
                                    "OpenRobotics/worlds/Test world 2/2"));
  EXPECT_TRUE(ignition::common::isFile(
      std::string("test_cache/fuel.ignitionrobotics.org/OpenRobotics/worlds/") +
      "Test world 2/2/test.world"));
  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

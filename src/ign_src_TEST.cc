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

#include "ignition/fuel_tools/ign.hh"
#include "test/test_config.h"  // NOLINT(build/include)

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

  EXPECT_NE(stdOutBuffer.str().find("failed to fetch model list"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListConfigServerUgly)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listModels("", "", "true"));

  EXPECT_NE(stdOutBuffer.str().find("https://api.ignitionfuel.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find("owners"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListCustomServerPretty)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listModels("https://staging-api.ignitionfuel.org"));

  EXPECT_NE(stdOutBuffer.str().find("https://staging-api.ignitionfuel.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("owners"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("models"), std::string::npos)
      << stdOutBuffer.str();

  EXPECT_EQ(stdOutBuffer.str().find("https://api.ignitionfuel.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find(
      "https://staging-api.ignitionfuel.org/1.0/"), std::string::npos)
      << stdOutBuffer.str();

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListCustomServerPrettyOwner)
{
  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  EXPECT_TRUE(listModels("https://staging-api.ignitionfuel.org",
      "openrobotics"));

  EXPECT_NE(stdOutBuffer.str().find("https://staging-api.ignitionfuel.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("1 owners"), std::string::npos)
      << stdOutBuffer.str();
  EXPECT_NE(stdOutBuffer.str().find("models"), std::string::npos)
      << stdOutBuffer.str();

  // If pagination fails, we only get the first 20 models
  EXPECT_EQ(stdOutBuffer.str().find("20 models"), std::string::npos)
      << stdOutBuffer.str();

  EXPECT_EQ(stdOutBuffer.str().find("https://api.ignitionfuel.org"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_EQ(stdOutBuffer.str().find(
      "https://staging-api.ignitionfuel.org/1.0/"), std::string::npos)
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
TEST(CmdLine, ModelDownloadUnversioned)
{
  ignition::common::removeAll("test_cache");
  ignition::common::createDirectories("test_cache");
  setenv("IGN_FUEL_CACHE_PATH", "test_cache", true);

  std::stringstream stdOutBuffer;
  std::stringstream stdErrBuffer;
  redirectIO(stdOutBuffer, stdErrBuffer);

  // Download
  EXPECT_TRUE(downloadUrl(
      "https://api.ignitionfuel.org/1.0/chapulina/models/Test box"));

  // Check output
  EXPECT_NE(stdOutBuffer.str().find("Download succeeded"),
      std::string::npos) << stdOutBuffer.str();
  EXPECT_TRUE(stdErrBuffer.str().empty());

  // Check files
  EXPECT_TRUE(ignition::common::isDirectory(
      "test_cache/api.ignitionfuel.org/chapulina/models/Test box"));
  EXPECT_TRUE(ignition::common::isFile(
      "test_cache/api.ignitionfuel.org/chapulina/models/Test box/model.sdf"));

  clearIOStreams(stdOutBuffer, stdErrBuffer);
  restoreIO();
}


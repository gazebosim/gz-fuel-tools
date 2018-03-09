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

  EXPECT_TRUE(listModels("", "true"));

// FIXME, issue #43: This fails on pipelines with:
// [Err] [ClientConfig.cc:222] Error copying default configuration file from
//   [/usr/local/share/ignition/fuel_tools//config.yaml] to
//   [/root/.ignition/fuel/config.yaml]

//  EXPECT_NE(stdOutBuffer.str().find("https://api.ignitionfuel.org"),
//      std::string::npos) << stdOutBuffer.str();
//  EXPECT_NE(stdOutBuffer.str().find("owners"), std::string::npos)
//      << stdOutBuffer.str();
//  EXPECT_NE(stdOutBuffer.str().find("models"), std::string::npos)
//      << stdOutBuffer.str();

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


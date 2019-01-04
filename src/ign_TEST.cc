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

#include "ignition/fuel_tools/config.hh"
#include "test/test_config.h"  // NOLINT(build/include)

/////////////////////////////////////////////////
std::string custom_exec_str(std::string _cmd)
{
  _cmd += " 2>&1";
  FILE *pipe = popen(_cmd.c_str(), "r");

  if (!pipe)
    return "ERROR";

  char buffer[128];
  std::string result = "";

  while (!feof(pipe))
  {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}

auto g_version = std::string(strdup(IGNITION_FUEL_TOOLS_VERSION_FULL));

auto g_listCmd = "ign fuel list -v 4 --force-version " + g_version;

/////////////////////////////////////////////////
TEST(CmdLine, Versions)
{
  auto output = custom_exec_str("ign fuel --versions");
  EXPECT_NE(output.find(g_version), std::string::npos) << output;
}

/////////////////////////////////////////////////
TEST(CmdLine, Help)
{
  auto output = custom_exec_str("ign fuel --force-version " + g_version +
      " --help");
  EXPECT_NE(output.find("list"), std::string::npos) << output;

  output = custom_exec_str("ign fuel --force-version " + g_version + " -h");
  EXPECT_NE(output.find("list"), std::string::npos) << output;

  output = custom_exec_str("ign fuel --force-version " + g_version);
  EXPECT_NE(output.find("list"), std::string::npos) << output;
}

/////////////////////////////////////////////////
TEST(CmdLine, ListFail)
{
  auto output = custom_exec_str(g_listCmd);
  EXPECT_NE(output.find("Missing resource type"), std::string::npos) << output;

  output = custom_exec_str(g_listCmd + " -t banana");
  EXPECT_NE(output.find("Invalid resource"), std::string::npos) << output;

  output = custom_exec_str(g_listCmd + " -t model -u fake_url");
  EXPECT_NE(output.find("Invalid URL"), std::string::npos)
      << output;

  output = custom_exec_str(g_listCmd + " -t world -u fake_url");
  EXPECT_NE(output.find("Invalid URL"), std::string::npos)
      << output;
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListConfigServerUgly)
{
  auto output = custom_exec_str(g_listCmd + " -t model --raw");
  EXPECT_NE(output.find("https://fuel.ignitionrobotics.org/1.0/"),
            std::string::npos) << output;
  EXPECT_EQ(output.find("owners"), std::string::npos) << output;
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListCustomServerPretty)
{
  auto output = custom_exec_str(
      g_listCmd + " -t model -u https://staging-fuel.ignitionrobotics.org");

  EXPECT_NE(output.find("https://staging-fuel.ignitionrobotics.org"),
      std::string::npos) << output;
  EXPECT_NE(output.find("owners"), std::string::npos) << output;
  EXPECT_NE(output.find("models"), std::string::npos) << output;

  EXPECT_EQ(output.find("https://fuel.ignitionrobotics.org"), std::string::npos)
      << output;
  EXPECT_EQ(output.find("https://staging-fuel.ignitionrobotics.org/1.0/"),
      std::string::npos) << output;
}

/////////////////////////////////////////////////
TEST(CmdLine, WorldListConfigServerUgly)
{
  auto output = custom_exec_str(g_listCmd +
      " -t world --raw -u https://staging-fuel.ignitionrobotics.org");
  EXPECT_NE(output.find("https://staging-fuel.ignitionrobotics.org"),
      std::string::npos) << output;
  EXPECT_EQ(output.find("owners"), std::string::npos) << output;
}

/////////////////////////////////////////////////
TEST(CmdLine, WorldListCustomServerPretty)
{
  auto output = custom_exec_str(
      g_listCmd + " -t world -u https://staging-fuel.ignitionrobotics.org");

  EXPECT_NE(output.find("https://staging-fuel.ignitionrobotics.org"),
      std::string::npos) << output;
  EXPECT_NE(output.find("owners"), std::string::npos) << output;
  EXPECT_NE(output.find("worlds"), std::string::npos) << output;
}

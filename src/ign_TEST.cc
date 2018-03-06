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

/////////////////////////////////////////////////
TEST(CmdLine, Help)
{
  auto output = custom_exec_str("ign fuel --help");
  EXPECT_NE(output.find("list"), std::string::npos);

  output = custom_exec_str("ign fuel -h");
  EXPECT_NE(output.find("list"), std::string::npos);

  output = custom_exec_str("ign fuel");
  EXPECT_NE(output.find("list"), std::string::npos);
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListFail)
{
  auto output = custom_exec_str("ign fuel list");
  EXPECT_NE(output.find("Missing resource type"), std::string::npos);

  output = custom_exec_str("ign fuel list -t banana");
  EXPECT_NE(output.find("Only model resources"), std::string::npos);

  output = custom_exec_str("ign fuel list -t model -u fake_url");
  EXPECT_NE(output.find("0 owners"), std::string::npos);
  EXPECT_NE(output.find("0 models"), std::string::npos);
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListConfigServer)
{
  auto output = custom_exec_str("ign fuel list -t model");
  EXPECT_NE(output.find("https://api.ignitionfuel.org"), std::string::npos);
  EXPECT_NE(output.find("owners"), std::string::npos);
  EXPECT_NE(output.find("models"), std::string::npos);
}

/////////////////////////////////////////////////
TEST(CmdLine, ModelListCustomServer)
{
  auto output = custom_exec_str(
      "ign fuel list -t model -u https://staging-api.ignitionfuel.org");
  EXPECT_EQ(output.find("https://api.ignitionfuel.org"), std::string::npos);
  EXPECT_NE(output.find("https://staging-api.ignitionfuel.org"),
      std::string::npos);
  EXPECT_NE(output.find("owners"), std::string::npos);
  EXPECT_NE(output.find("models"), std::string::npos);
}


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

#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "ignition/fuel-tools/GirderClient.hh"

using namespace ignition;
using namespace fuel_tools;

// Global constants.
const std::string g_URL = "https://data.ignitionfuel.org/";
const std::string g_apiVersion = "api/v1/";

/////////////////////////////////////////////////
/// \brief Request the Girder version installed.
TEST(GirderClientTEST, Version)
{
  CommonConfiguration conf = {g_URL, g_apiVersion};
  GirderClient girderClient(conf);
  std::vector<std::string> headers = {"Accept: application/json"};
  auto resp = girderClient.Request("GET", "system/version", {}, headers, "");

  ASSERT_EQ(resp.statusCode, 200);
  ASSERT_TRUE(resp.data.find("apiVersion") != std::string::npos);
}

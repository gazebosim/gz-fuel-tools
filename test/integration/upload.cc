/*
 * Copyright (C) 2024 Open Source Robotics Foundation
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

#include <iostream>
#include <gtest/gtest.h>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/fuel_tools/FuelClient.hh>
#include <gz/fuel_tools/WorldIdentifier.hh>

#include "test_config.hh"

using namespace gz;
using namespace fuel_tools;

/////////////////////////////////////////////////
class UploadIntegrationTest : public ::testing::Test
{
  public: void SetUp() override
  {
    gz::common::Console::SetVerbosity(4);
  }
};

/////////////////////////////////////////////////
TEST_F(UploadIntegrationTest, WorldUploadNoAccessToken)
{
  std::string worldPath = common::joinPaths(std::string(TEST_PATH),
                                            "media", "worlds", "test_empty");
  EXPECT_TRUE(gz::common::exists(worldPath)) << worldPath;
  FuelClient client;
  WorldIdentifier worldId;
  std::vector<std::string> headers;

  // This will fail because an access token has not been specified.
  Result result = client.UploadWorld(worldPath, worldId, headers,
                                     true, "test_owner");
  EXPECT_EQ(ResultType::UPLOAD_ERROR, result.Type()) << result.ReadableResult();
}

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

#include <ignition/common/Filesystem.hh>

#include "ignition/fuel_tools/Zip.hh"
#include "test/test_config.h"

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
// Test compressing files in a directory into a zip archive
TEST(Zip, Extract)
{
  // extract a compressed archive
  std::string dst = std::string(PROJECT_BINARY_PATH);
  std::string archivePath = std::string(TEST_PATH) + "/media/box.zip";
  EXPECT_TRUE(Zip::Extract(archivePath, dst));

  // check extracted contents
  std::string boxPath = dst + "/box";
  EXPECT_TRUE(ignition::common::exists(boxPath));
  EXPECT_TRUE(ignition::common::isDirectory(boxPath));

  std::string boxFilePath = boxPath +"/file";
  EXPECT_TRUE(ignition::common::exists(boxFilePath));
  EXPECT_TRUE(ignition::common::isFile(boxFilePath));

  std::string boxDirPath = boxPath +"/dir";
  EXPECT_TRUE(ignition::common::exists(boxDirPath));
  EXPECT_TRUE(ignition::common::isDirectory(boxDirPath));

  std::string boxFile2Path = boxDirPath +"/file2";
  EXPECT_TRUE(ignition::common::exists(boxFile2Path));
  EXPECT_TRUE(ignition::common::isFile(boxFile2Path));

  // cleanup
  EXPECT_TRUE(ignition::common::removeAll(boxPath));
  EXPECT_FALSE(ignition::common::exists(boxPath));
}

/////////////////////////////////////////////////
// Test extracting a zip archive
TEST(Zip, Compress)
{
  // compress a directory into a zip archive
  std::string archivePath = std::string(PROJECT_BINARY_PATH) + "/box.zip";
  std::string src = std::string(TEST_PATH) + "/media/box";
  EXPECT_TRUE(Zip::Compress(src, archivePath));

  EXPECT_TRUE(ignition::common::exists(archivePath));
  EXPECT_TRUE(ignition::common::isFile(archivePath));

  // extract the zip archive and verify contents
  std::string dst = std::string(PROJECT_BINARY_PATH);
  EXPECT_TRUE(Zip::Extract(archivePath, dst));

  // check extracted contents
  std::string boxPath = dst + "/box";
  EXPECT_TRUE(ignition::common::exists(boxPath));
  EXPECT_TRUE(ignition::common::isDirectory(boxPath));

  std::string boxFilePath = boxPath +"/file";
  EXPECT_TRUE(ignition::common::exists(boxFilePath));
  EXPECT_TRUE(ignition::common::isFile(boxFilePath));

  std::string boxDirPath = boxPath +"/dir";
  EXPECT_TRUE(ignition::common::exists(boxDirPath));
  EXPECT_TRUE(ignition::common::isDirectory(boxDirPath));

  std::string boxFile2Path = boxDirPath +"/file2";
  EXPECT_TRUE(ignition::common::exists(boxFile2Path));
  EXPECT_TRUE(ignition::common::isFile(boxFile2Path));

  // cleanup
  EXPECT_TRUE(ignition::common::removeAll(archivePath));
  EXPECT_FALSE(ignition::common::exists(archivePath));

  EXPECT_TRUE(ignition::common::removeAll(boxPath));
  EXPECT_FALSE(ignition::common::exists(boxPath));
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

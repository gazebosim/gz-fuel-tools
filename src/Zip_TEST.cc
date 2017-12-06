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

// All these helper functions have been copied from
// https://bitbucket.org/ignitionrobotics/ign-common/raw/default/src/Filesystem_TEST.cc

#ifndef _WIN32
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>

/////////////////////////////////////////////////
bool create_and_switch_to_temp_dir(std::string &_new_temp_path)
{
  std::string tmppath;
  const char *tmp = getenv("TMPDIR");
  if (tmp)
  {
    tmppath = std::string(tmp);
  }
  else
  {
    tmppath = std::string("/tmp");
  }

  tmppath += "/XXXXXX";

  char *dtemp = mkdtemp(const_cast<char *>(tmppath.c_str()));
  if (dtemp == nullptr)
  {
    return false;
  }
  if (chdir(dtemp) < 0)
  {
    return false;
  }

  char resolved[PATH_MAX];
  if (realpath(dtemp, resolved) == nullptr)
  {
    return false;
  }

  _new_temp_path = std::string(resolved);

  return true;
}

/////////////////////////////////////////////////
bool create_new_empty_file(const std::string &_filename)
{
  int fd = open(_filename.c_str(), O_RDWR | O_CREAT, 0644);
  if (fd < 0)
  {
    return false;
  }

  close(fd);

  return true;
}

/////////////////////////////////////////////////
bool create_new_file_symlink(const std::string &_symlink,
                             const std::string &_target)
{
  return symlink(_target.c_str(), _symlink.c_str()) == 0;
}

/////////////////////////////////////////////////
bool create_new_dir_symlink(const std::string &_symlink,
                            const std::string &_target)
{
  return symlink(_target.c_str(), _symlink.c_str()) == 0;
}

/////////////////////////////////////////////////
bool create_new_file_hardlink(const std::string &_hardlink,
                              const std::string &_target)
{
  return link(_target.c_str(), _hardlink.c_str()) == 0;
}

#else
#include <windows.h>
#include <winnt.h>
#include <cstdint>
#include "PrintWindowsSystemWarning.hh"

/////////////////////////////////////////////////
bool create_and_switch_to_temp_dir(std::string &_new_temp_path)
{
  char temp_path[MAX_PATH + 1];
  DWORD path_len = ::GetTempPathA(MAX_PATH, temp_path);
  if (path_len >= MAX_PATH || path_len <= 0)
  {
    return false;
  }
  std::string path_to_create(temp_path);
  srand(static_cast<uint32_t>(time(nullptr)));

  for (int count = 0; count < 50; ++count)
  {
    // Try creating a new temporary directory with a randomly generated name.
    // If the one we chose exists, keep trying another path name until we reach
    // some limit.
    std::string new_dir_name;
    new_dir_name.append(std::to_string(::GetCurrentProcessId()));
    new_dir_name.push_back('_');
    // On Windows, rand_r() doesn't exist as an alternative to rand(), so the
    // cpplint warning is spurious.  This program is not multi-threaded, so
    // it is safe to suppress the threadsafe_fn warning here.
    new_dir_name.append(
       std::to_string(rand()    // NOLINT(runtime/threadsafe_fn)
                      % ((int16_t)0x7fff)));

    path_to_create += new_dir_name;
    if (::CreateDirectoryA(path_to_create.c_str(), nullptr))
    {
      _new_temp_path = path_to_create;
      return ::SetCurrentDirectoryA(_new_temp_path.c_str()) != 0;
    }
  }

  return false;
}

/////////////////////////////////////////////////
bool create_new_empty_file(const std::string &_filename)
{
  return ::CreateFileA(_filename.c_str(),
                       FILE_READ_DATA,
                       FILE_SHARE_READ,
                       nullptr,
                       OPEN_ALWAYS,
                       0,
                       nullptr) != INVALID_HANDLE_VALUE;
}

/////////////////////////////////////////////////
bool create_new_file_symlink(const std::string &_symlink,
                             const std::string &_target)
{
  const bool linked = ::CreateSymbolicLinkA(
        _symlink.c_str(), _target.c_str(), 0);

  if (!linked)
  {
    ignition::common::PrintWindowsSystemWarning(
          "Failed to create file symlink from [" + _target
          + "] to [" + _symlink + "]");
  }

  return linked;
}

/////////////////////////////////////////////////
bool create_new_dir_symlink(const std::string &_symlink,
                            const std::string &_target)
{
  const bool linked = ::CreateSymbolicLinkA(_symlink.c_str(), _target.c_str(),
                                            SYMBOLIC_LINK_FLAG_DIRECTORY);
  if (!linked)
  {
    ignition::common::PrintWindowsSystemWarning(
          "Failed to create directory symlink from [" + _target
          + "] to [" + _symlink + "]");
  }

  return linked;
}

/////////////////////////////////////////////////
bool create_new_file_hardlink(const std::string &_hardlink,
                              const std::string &_target)
{
  return ::CreateHardLinkA(_hardlink.c_str(), _target.c_str(), nullptr) == TRUE;
}

#endif

#include <gtest/gtest.h>
#include <ignition/common/Filesystem.hh>
#include "ignition/fuel-tools/Zip.hh"

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
/// \brief Test basic API
TEST(Zip, API)
{
  // Extract invalid paths
  EXPECT_FALSE(Zip::Extract("", ""));
  EXPECT_FALSE(Zip::Extract("aaa.zip", "/tmp"));

  // Compress invalid paths
  EXPECT_FALSE(Zip::Extract("", ""));
  EXPECT_FALSE(Zip::Compress("aaa", "aaa.zip"));
}

/////////////////////////////////////////////////
/// \brief Test compress and extract
TEST(Zip, CompressAndExtract)
{
  std::string newTempDir;
  ASSERT_TRUE(create_and_switch_to_temp_dir(newTempDir));
  auto d = ignition::common::joinPaths(newTempDir, "d1", "d2");
  ASSERT_TRUE(ignition::common::createDirectories(d));
  auto f = ignition::common::joinPaths(d, "new_file");
  ASSERT_TRUE(create_new_empty_file(f));
  EXPECT_TRUE(ignition::common::exists(f));

  // Compress.
  auto d1 = ignition::common::joinPaths(newTempDir, "d1");
  auto zipOutFile = ignition::common::joinPaths(newTempDir, "new_file.zip");
  EXPECT_TRUE(Zip::Compress(d1, zipOutFile));
  EXPECT_TRUE(ignition::common::exists(zipOutFile));

  // Extract.
  auto extractOutDir = ignition::common::joinPaths(newTempDir, "extract");
  EXPECT_TRUE(Zip::Extract(zipOutFile, extractOutDir));
  auto extractOutFile =
    ignition::common::joinPaths(extractOutDir, "d1", "d2", "new_file");
  EXPECT_TRUE(ignition::common::exists(extractOutFile));

  // Clean.
  ignition::common::removeAll(newTempDir);
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

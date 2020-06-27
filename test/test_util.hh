/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#ifndef IGNITION_TEST_UTIL_HH_
#define IGNITION_TEST_UTIL_HH_

#include <gtest/gtest.h>
#include <string>
#include "ignition/common/Console.hh"
#include "ignition/common/Filesystem.hh"
#include "ignition/common/Util.hh"

#include "test/test_config.h"

#define IGN_TMP_DIR "tmp-ign/"

namespace ignition
{
  namespace testing
  {
    bool TestDataPath(std::string &_dataDir)
    {
      std::string dataDir;
      if(ignition::common::env("TEST_SRCDIR", dataDir))
      {
        _dataDir = ignition::common::joinPaths(dataDir, "__main__/ign_fuel_tools");
        return true;
      }
      else
      {
        _dataDir = PROJECT_SOURCE_PATH;
        return true;
      }

      return false;
    }

    bool TestTmpPath(std::string &_tmpDir)
    {
      std::string tmpDir;

      if(ignition::common::env("TEST_UNDECLARED_OUTPUTS_DIR", tmpDir))
      {
        _tmpDir = tmpDir;
        return true;
      }

      std::string homeDir;
      if(ignition::common::env(IGN_HOMEDIR, homeDir))
      {
        _tmpDir = ignition::common::joinPaths(homeDir, IGN_TMP_DIR);
        return true;
      }

      return false;
    }
  }
}
#endif

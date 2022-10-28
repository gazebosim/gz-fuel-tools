/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#include <gz/common/Filesystem.hh>
#include "gz/fuel_tools/Helpers.hh"

using namespace gz;
using namespace fuel_tools;

/////////////////////////////////////////////////
TEST(HelpersTEST, UriToPathNoAuthority)
{
// TO-DO: Update this test after gz-fuel-tools#204 is addressed
#ifdef WIN32
  const std::string testStr1 = R"(localhost:8000\some\path)";
  const std::string testStr2 = R"(localhost:8000\some\path\)";
#else
  const std::string testStr1 = R"(localhost:8000/some/path)";
  const std::string testStr2 = R"(localhost:8000/some/path/)";
#endif

  {
    common::URI uri{"http://localhost:8000"};
    EXPECT_EQ("localhost:8000", uriToPath(uri));
  }

  {
    common::URI uri{"http://localhost:8000/some/path"};
    EXPECT_EQ(testStr1, uriToPath(uri));
  }

  {
    common::URI uri{"http://localhost:8000/some/path/"};
    EXPECT_EQ(testStr2, uriToPath(uri));
  }
}

/////////////////////////////////////////////////
TEST(HelpersTEST, UriToPathHasAuthority)
{
  {
    common::URI uri{"http://localhost:8000", true};
    EXPECT_EQ("localhost:8000", uriToPath(uri));
  }

  {
    common::URI uri{"http://localhost:8000/some/path", true};
    EXPECT_EQ(common::joinPaths("localhost:8000", "some", "path"),
        uriToPath(uri));
  }

  {
    common::URI uri{"http://localhost:8000/some/path/", true};
    EXPECT_EQ(common::separator(common::joinPaths("localhost:8000", "some",
        "path")), uriToPath(uri));
  }
}

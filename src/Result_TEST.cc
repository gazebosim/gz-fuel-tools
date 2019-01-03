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
#include "ignition/fuel_tools/Result.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

/////////////////////////////////////////////////
/// \brief Create a request object with a type and check that it
TEST(Result, TypeCanBeSet)
{
  Result r1(ResultType::UPLOAD);
  EXPECT_EQ(ResultType::UPLOAD, r1.Type());

  Result r2(ResultType::DELETE);
  EXPECT_EQ(ResultType::DELETE, r2.Type());
}

/////////////////////////////////////////////////
/// \brief Check that there are readable strings returned
TEST(Result, ReadableStringsHaveLength)
{
  ignition::fuel_tools::Result r(ResultType::FETCH);

  EXPECT_FALSE(Result(ResultType::DELETE).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::FETCH).ReadableResult().empty());
  EXPECT_FALSE(
      Result(ResultType::FETCH_ALREADY_EXISTS).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::UPLOAD).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::UNKNOWN).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::DELETE_NOT_FOUND).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::DELETE_ERROR).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::FETCH_NOT_FOUND).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::FETCH_ERROR).ReadableResult().empty());
  EXPECT_FALSE(
      Result(ResultType::UPLOAD_ALREADY_EXISTS).ReadableResult().empty());
  EXPECT_FALSE(Result(ResultType::UPLOAD_ERROR).ReadableResult().empty());
}

//////////////////////////////////////////////////
/// \brief Check that operator bool returns true for successfull cases
TEST(Result, TrueOnSuccess)
{
  EXPECT_TRUE(Result(ResultType::DELETE));
  EXPECT_TRUE(Result(ResultType::FETCH));
  EXPECT_TRUE(Result(ResultType::FETCH_ALREADY_EXISTS));
  EXPECT_TRUE(Result(ResultType::UPLOAD));
  EXPECT_FALSE(Result(ResultType::UNKNOWN));
  EXPECT_FALSE(Result(ResultType::DELETE_NOT_FOUND));
  EXPECT_FALSE(Result(ResultType::DELETE_ERROR));
  EXPECT_FALSE(Result(ResultType::FETCH_NOT_FOUND));
  EXPECT_FALSE(Result(ResultType::FETCH_ERROR));
  EXPECT_FALSE(Result(ResultType::UPLOAD_ALREADY_EXISTS));
  EXPECT_FALSE(Result(ResultType::UPLOAD_ERROR));
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

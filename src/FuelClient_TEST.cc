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
#include <gmock/gmock.h>

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/FuelClient.hh"
#include "ignition/fuel-tools/REST.hh"


namespace ignition
{
  namespace fuel_tools
  {
    class MockREST : public REST {
      public: MOCK_CONST_METHOD6(Request, RESTResponse(const std::string &,
          const std::string &, const std::string &,
          const std::vector<std::string> &, const std::vector<std::string> &,
          const std::string &));
    };
  }
}

using namespace ignition;
using namespace fuel_tools;

/////////////////////////////////////////////////
RESTResponse MockREST(const std::string &, const std::string &,
    const std::string &, const std::vector<std::string> &,
    const std::vector<std::string> &, const std::string &)
{
  RESTResponse response;
  response.statusCode = 200;
  response.data = "{}";
  return response;
}

/////////////////////////////////////////////////
/// \brief Fetch models
TEST(FuelClent, FetchModels)
{
  ignition::fuel_tools::ClientConfig conf;
  conf.AddServer("http://localhost:8001/");

  ignition::fuel_tools::MockREST rest;
  ignition::fuel_tools::FuelClient client(conf, rest);
  auto iter = client.Models();
}

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

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

#include <ignition/common/Console.hh>
#include <ignition/fuel-tools/FuelClient.hh>
#include <ignition/fuel-tools/LocalCache.hh>

int main()
{
  ignition::common::Console::SetVerbosity(4);

  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL("https://api-staging:ignition-fuel.org/");
  srv.LocalName("local");
  conf.AddServer(srv);

  conf.CacheLocation("/tmp/ign-fuel-tools");

  ignition::fuel_tools::FuelClient client(conf);
  auto iter = client.Models();

  // TODO LocalCache should be used inside FuelClient
  ignition::fuel_tools::LocalCache cache(&conf);

  auto localIter = cache.AllModels();
  while (localIter)
  {
    ignmsg << "Model: " << localIter->Identification().Name() << "\n";
    ++localIter;
  }

  ignition::fuel_tools::ModelIdentifier someId;
  someId.SourceURL("http://localhost:8001/");
  someId.Owner("alice");
  someId.Name("am2");
  auto model = cache.MatchingModel(someId);
  if (model)
  {
    ignmsg << "Found model: " << model.Identification().UniqueName() << "\n";
  }

  // test file upload
  std::ofstream fout("dummy.zip");
  fout << "dummy file";
  fout.flush();
  fout.close();

  ignition::fuel_tools::REST::Protocol protocol =
      ignition::fuel_tools::REST::POST_FORM;
  std::vector<std::string> headers =
      {"Accept: application/json", "content-type: multipart/form-data"};
  std::map<std::string, std::string> form;
  form["name"] = "box";
  form["data"] = "@dummy.zip";
  ignition::fuel_tools::REST rest;
  ignition::fuel_tools::RESTResponse resp = rest.Request(
      protocol, "http://localhost:8001/", "1.0/models", {}, headers, "",
      form);
  ignmsg << "file update status: " << resp.statusCode << std::endl;
  ignition::common::removeAll("dummy.zip");

  ignmsg << "exiting\n";
  return 0;
}

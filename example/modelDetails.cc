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

#include <iostream>

#include <gflags/gflags.h>
#include <ignition/fuel-tools.hh>

DEFINE_bool(h, false, "Show help");
DEFINE_string(m, "", "Model name");
DEFINE_string(o, "anonymous", "Owner name");
DEFINE_string(s, "https://localhost:4430", "Server name");

int main(int argc, char **argv)
{
  // Simple usage.
  std::string usage("Show details of a model.");
  usage += " Usage:\n  ./modelDetails <options>\n\n";
  usage += "  Example:\n\t ./modelDetails -s https://localhost:4430 "
           " -o anonymous -m Beer";

  gflags::SetUsageMessage(usage);

  // Parse command line arguments.
  gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);

  // Show help, if specified.
  if (FLAGS_h || FLAGS_m == "")
  {
    gflags::SetCommandLineOptionWithMode("help", "false",
        gflags::SET_FLAGS_DEFAULT);
    gflags::SetCommandLineOptionWithMode("helpshort", "true",
        gflags::SET_FLAGS_DEFAULT);
  }
  gflags::HandleCommandLineHelpFlags();

  // Create a ClientConfig, TODO create this from a yaml file
  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL(FLAGS_s);
  srv.LocalName("ignitionfuel");
  conf.AddServer(srv);

  ignition::fuel_tools::FuelClient client(conf);

  // Set the properties of the model that we want to download.
  ignition::fuel_tools::ModelIdentifier modelIdentifier;
  modelIdentifier.Owner(FLAGS_o);
  modelIdentifier.Name(FLAGS_m);

  // Fetch the model details.
  ignition::fuel_tools::ModelIdentifier model;
  auto result = client.ModelDetails(modelIdentifier, model);

  // Show model details.
  if (result)
  {
    std::cout << "Name: " << model.Name() << std::endl;
    std::cout << "Owner: " << model.Owner() << std::endl;
    std::cout << "Description: " << model.Description() << std::endl;
    std::cout << "Likes: " << model.Likes() << std::endl;
    std::cout << "Downloads: " << model.Downloads() << std::endl;
    std::cout << "License name: " << model.LicenseName() << std::endl;
    std::cout << "License URL: " << model.LicenseURL() << std::endl;
    std::cout << "License image URL: " << model.LicenseImageURL() << std::endl;
  }
}

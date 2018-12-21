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

#include <gflags/gflags.h>

#include <iostream>

#include <ignition/fuel_tools.hh>

DEFINE_bool(h, false, "Show help");

DEFINE_string(c, "", "Config file");
DEFINE_string(m, "", "Model name");
DEFINE_string(o, "anonymous", "Owner name");
DEFINE_string(s, "", "Server name");

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Simple usage.
  std::string usage("Show details of a model.");
  usage += " Usage:\n  ./modelDetails <options>\n\n";
  usage += "  Example:\n"
           "\t ./modelDetails -o anonymous -m Beer"
           "\t ./modelDetails -s https://localhost:4430 -o anonymous -m Beer"
           "\t ./modelDetails -c /tmp/my_config.yaml -o anonymous -m Beer";

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

  // Setup ClientConfig.
  ignition::fuel_tools::ClientConfig conf;

  if (FLAGS_s != "")
  {
    // The user specified a Fuel server via command line.
    ignition::fuel_tools::ServerConfig srv;
    srv.SetUrl(ignition::common::URI(FLAGS_s));

    // Add the extra Fuel server.
    conf.AddServer(srv);
  }

  if (FLAGS_c != "")
    conf.SetConfigPath(FLAGS_c);

  if (!conf.LoadConfig())
  {
    std::cerr << "Error loading configuration file [" << FLAGS_c << "]"
              << std::endl;
    return -1;
  }

  // Instantiate the FuelClient object with the configuration.
  ignition::fuel_tools::FuelClient client(conf);

  // Set the properties of the model that we want to search.
  ignition::fuel_tools::ModelIdentifier modelIdentifier;
  modelIdentifier.SetOwner(FLAGS_o);
  modelIdentifier.SetName(FLAGS_m);

  // Fetch the model details.
  for (const auto &server : client.Config().Servers())
  {
    ignition::fuel_tools::ModelIdentifier model;
    modelIdentifier.SetServer(server);
    if (!client.ModelDetails(modelIdentifier, model))
      continue;

    // Show server.
    std::cout << "[" << server.Url().Str() << "]\n\n";

    // Show model details.
    std::cout << "  Name: " << model.Name() << std::endl;
    std::cout << "  Source URL: " << model.Server().Url().Str() << std::endl;
    std::cout << "  Unique name: " << model.UniqueName() << std::endl;
    std::cout << "  Owner: " << model.Owner() << std::endl;
    std::cout << "  Description: " << model.Description() << std::endl;
    std::cout << "  Likes: " << model.LikeCount() << std::endl;
    std::cout << "  Downloads: " << model.DownloadCount() << std::endl;
    std::cout << "  License name: " << model.LicenseName() << std::endl;
    std::cout << "  License URL: " << model.LicenseUrl() << std::endl;
    std::cout << "  License image URL: " << model.LicenseImageUrl()
        << std::endl;
    std::cout << "  Tags: " << std::endl;
    for (auto const &tag : model.Tags())
      std::cout << "  " << tag << std::endl;
  }

  return 0;
}

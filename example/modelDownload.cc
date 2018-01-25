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
  std::string usage("Download a model.");
  usage += " Usage:\n  ./modelDownload <options>\n\n";
  usage += "  Examples:\n"
           "\t ./modelDownload -o caguero -m Beer\n"
           "\t ./modelDownload -s https://localhost:4430 -o caguero -m Beer\n"
           "\t ./modelDownload -c /tmp/my_config.yaml -o caguero -m Beer\n";

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
    srv.URL(FLAGS_s);
    srv.LocalName("ignitionfuel");

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

  // Set the properties of the model that we want to download.
  ignition::fuel_tools::ModelIdentifier modelIdentifier;
  modelIdentifier.Owner(FLAGS_o);
  modelIdentifier.Name(FLAGS_m);

  // Fetch the model.
  for (const auto &server : client.Config().Servers())
  {
    if (client.DownloadModel(server, modelIdentifier))
      return 0;
  }

  std::cerr << "Unable to download model" << std::endl;
  return -1;
}
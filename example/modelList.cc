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
DEFINE_string(s, "", "Server name");

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Simple usage.
  std::string usage("List all models.");
  usage += " Usage:\n  ./modelList <options>\n\n";
  usage += "  Example:\n"
           "\t ./modelList\n"
           "\t ./modelList -s https://localhost:4430"
           "\t ./modelList -c /tmp/my_config.yaml\n";

  gflags::SetUsageMessage(usage);

  // Parse command line arguments.
  gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);

  // Show help, if specified.
  if (FLAGS_h)
  {
    gflags::SetCommandLineOptionWithMode("help", "false",
        gflags::SET_FLAGS_DEFAULT);
    gflags::SetCommandLineOptionWithMode("helpshort", "true",
        gflags::SET_FLAGS_DEFAULT);
  }
  gflags::HandleCommandLineHelpFlags();

  if (FLAGS_c != "" && FLAGS_s != "")
  {
    std::cerr << "Please, do not use -c <config_file> and -s <server> together."
              << std::endl;
    return -1;
  }

  // Setup ClientConfig.
  ignition::fuel_tools::ClientConfig conf;
  conf.SetUserAgent("ExampleModelList");

  if (FLAGS_s != "")
  {
    // The user specified a Fuel server via command line.
    ignition::fuel_tools::ServerConfig srv;
    srv.URL(FLAGS_s);
    srv.LocalName("ignitionfuel");

    // Add the extra Fuel server.
    conf.AddServer(srv);
  }
  else
  {
    if (FLAGS_c != "")
      conf.SetConfigPath(FLAGS_c);

    if (!conf.LoadConfig())
    {
      std::cerr << "Error loading configuration file [" << FLAGS_c << "]"
                << std::endl;
      return -1;
    }
  }

  // Instantiate the FuelClient object with the configuration.
  ignition::fuel_tools::FuelClient client(conf);

  for (const auto &server : client.Config().Servers())
  {
    std::cout << "[" << server.URL() << "]\n\n";
    for (auto iter = client.Models(server); iter; ++iter)
      std::cout << "  " << iter->Identification().Name() << "\n";
    std::cout << std::endl;
  }
}

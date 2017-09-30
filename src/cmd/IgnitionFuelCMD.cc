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

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/Helpers.hh"
#include "ignition/fuel-tools/FuelClient.hh"


/// \brief Read a config file from disk
ignition::fuel_tools::ClientConfig IGNITION_FUEL_TOOLS_HIDDEN getConfig()
{
  // TODO READ config file, don't hardcode this
  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL("https://staging-api.ignitionfuel.org/");
  srv.LocalName("staging_ignitionfuel");
  conf.AddServer(srv);

  if(const char *cacheLoc = std::getenv("IGN_FUEL_CACHE"))
    conf.CacheLocation(cacheLoc);
  else
  {
    std::string home = "";
#if _WIN32
    const char *drive = std::getenv("HOMEDRIVE");
    const char *path = std::getenv("HOMEPATH");

    if (drive && path)
      home = ignition::common::joinPaths(drive, path);
#else
    const char *homeLoc = std::getenv("HOME");

    if (homeLoc)
      home = homeLoc;
#endif
    conf.CacheLocation(ignition::common::joinPaths(home, ".ignition", "fuel"));
  }
  if (!ignition::common::isDirectory(conf.CacheLocation()))
  {
    ignwarn << "[" << conf.CacheLocation() << "] doesn't exist, creating\n";
    ignition::common::createDirectories(conf.CacheLocation());
  }
  return conf;
}

/// \brief Print models from a ModelIter to stdout
void IGNITION_FUEL_TOOLS_HIDDEN printModels(
    ignition::fuel_tools::ModelIter _iter)
{
  // print out the models
  while (_iter)
  {
    std::cout << _iter->Identification().UniqueName() << "\n";
    ++_iter;
  }
}

extern "C"
{
  /// \brief Prints "Hello World!\n" to STDOUT
  void IGNITION_FUEL_TOOLS_VISIBLE helloWorld()
  {
    std::cout << "Hello world!\n";
  }

  /// \brief Prints "Hello World!\n" to STDOUT
  /// \param[in] _level [0-4] Verbosity level
  void IGNITION_FUEL_TOOLS_VISIBLE verbosity(int _level)
  {
    if (_level < 0 || _level > 4)
    {
      std::cerr << "Invalid verbosity level\n";
      std::exit(-1);
    }
    ignition::common::Console::SetVerbosity(_level);
  }

  /// \brief Lists Models
  /// \param[in] _name only list models with this name
  /// \param[in] _owner only list models from this owner
  /// \param[in] _url only list models from this server
  void IGNITION_FUEL_TOOLS_VISIBLE listModels(char *_name, char *_owner,
      char *_url)
  {
    auto conf = getConfig();
    ignition::fuel_tools::FuelClient client(conf);

    if (!std::strlen(_name) && !std::strlen(_owner) && !std::strlen(_url))
    {
      // All models
      printModels(client.Models());
    }
    else
    {
      // Get some models
      ignition::fuel_tools::ModelIdentifier id;
      if (std::strlen(_name))
        id.Name(_name);
      if (std::strlen(_owner))
        id.Owner(_owner);
      if (std::strlen(_url))
        id.SourceURL(_url);
      printModels(client.Models(id));
    }
  }

  /// \brief Prints where a model is on disk, downloading it if necessary
  /// \remarks if multiple models match, returns only the first one
  /// \param[in] _name Name of the model (required)
  /// \param[in] _owner Who owns the model (optional)
  /// \param[in] _url URL to get the model from (optional)
  void IGNITION_FUEL_TOOLS_VISIBLE locateModel(char *_name, char *_owner,
      char *_url)
  {
    auto conf = getConfig();
    ignition::fuel_tools::FuelClient client(conf);

    ignition::fuel_tools::ModelIdentifier id;
    id.Name(_name);
    if (std::strlen(_owner))
      id.Owner(_owner);
    if (std::strlen(_url))
      id.SourceURL(_url);
    auto iter = client.Models(id);
    if (!iter)
    {
      std::cerr << "Model not found\n";
    }
    else
    {
      iter->Fetch();
      std::cout << iter->PathToModel() << "\n";
    }
  }

  /// \brief Download a model
  /// \param[in] _name Name of the model (required)
  /// \param[in] _owner Who owns the model (required)
  /// \param[in] _url URL to get the model from (optional)
  void IGNITION_FUEL_TOOLS_VISIBLE pullModel(char *_name, char *_owner,
      char *_url)
  {
    auto conf = getConfig();
    ignition::fuel_tools::FuelClient client(conf);

    ignition::fuel_tools::ModelIdentifier id;
    id.Name(_name);
    id.Owner(_owner);
    if (std::strlen(_url))
      id.SourceURL(_url);

    std::cerr << "TODO Download a model\n";
  }

  /// \brief upload a model
  /// \param[in] _name Name of the model (required)
  /// \param[in] _owner Who owns the model (required)
  /// \param[in] _url URL to get the model from (required)
  /// \param[in] _path only list models from this server (required)
  void IGNITION_FUEL_TOOLS_VISIBLE pushModel(char *_name, char *_owner,
      char *_url, char *_path)
  {
    auto conf = getConfig();
    ignition::fuel_tools::FuelClient client(conf);

    ignition::fuel_tools::ModelIdentifier id;
    id.Name(_name);
    id.Owner(_owner);
    id.SourceURL(_url);

    std::cerr << "TODO Upload a model\n";
  }
}

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

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include "ignition/fuel-tools/ClientConfig.hh"
#include "ignition/fuel-tools/config.hh"

using namespace ignition;
using namespace fuel_tools;

// Constants.
static const std::string initialConfigFile = ignition::common::joinPaths(
    IGNITION_FUEL_INITIAL_CONFIG_PATH, "config.yaml");

//////////////////////////////////////////////////
/// \brief Private data class
class ignition::fuel_tools::ClientConfigPrivate
{
  /// \brief A list of servers.
  public: std::vector<ServerConfig> servers;

  /// \brief a path on disk to where data is cached.
  public: std::string cacheLocation;

  /// \brief The path where the configuration file is located.
  public: std::string configPath;
};

//////////////////////////////////////////////////
/// \brief Private data class
class ignition::fuel_tools::ServerConfigPrivate
{
  /// \brief URL to reach server
  public: std::string url;

  /// \brief Local directory server stuff is saved in
  public: std::string localName;

  /// \brief A key to auth with the server
  public: std::string key;

  /// \brief The protocol version used when talking with this server.
  public: std::string version = "1.0";
};

//////////////////////////////////////////////////
ServerConfig::ServerConfig()
  : dataPtr (new ServerConfigPrivate)
{
}

//////////////////////////////////////////////////
ServerConfig::ServerConfig(const ServerConfig &_orig)
  : dataPtr(new ServerConfigPrivate)
{
  *(this->dataPtr) = *(_orig.dataPtr);
}

//////////////////////////////////////////////////
ServerConfig &ServerConfig::operator=(const ServerConfig &_orig)
{
  *(this->dataPtr) = *(_orig.dataPtr);
  return *this;
}

//////////////////////////////////////////////////
ServerConfig::~ServerConfig()
{
}

//////////////////////////////////////////////////
std::string ServerConfig::URL() const
{
  return this->dataPtr->url;
}

//////////////////////////////////////////////////
void ServerConfig::URL(const std::string &_url)
{
  // Strip trailing slashes
  std::string url = _url;
  while (!url.empty() && url.back() == '/')
    url.pop_back();

  this->dataPtr->url = url;
}

//////////////////////////////////////////////////
std::string ServerConfig::LocalName() const
{
  return this->dataPtr->localName;
}

//////////////////////////////////////////////////
void ServerConfig::LocalName(const std::string &_name)
{
  this->dataPtr->localName = _name;
}

//////////////////////////////////////////////////
std::string ServerConfig::APIKey() const
{
  return this->dataPtr->key;
}

//////////////////////////////////////////////////
void ServerConfig::APIKey(const std::string &_key)
{
  this->dataPtr->key = _key;
}

//////////////////////////////////////////////////
std::string ServerConfig::Version() const
{
  return this->dataPtr->version;
}

//////////////////////////////////////////////////
void ServerConfig::Version(const std::string &_version)
{
  this->dataPtr->version = _version;
}

/////////////////////////////////////////////////
/// \brief Get home directory.
/// \return Home directory or empty string if home wasn't found.
/// \ToDo: Move this function to ignition::common::Filesystem
std::string homePath()
{
  std::string homePath;
#ifndef _WIN32
  ignition::common::env("HOME", homePath);
#else
  ignition::common::env("HOMEPATH", homePath);
#endif

  return homePath;
}

//////////////////////////////////////////////////
ClientConfig::ClientConfig() : dataPtr(new ClientConfigPrivate)
{
  this->dataPtr->configPath = ignition::common::joinPaths(
    homePath(), ".ignition", "fuel", "config.yaml");

  // If the custom config file doesn't exist, we create it.
  if (!ignition::common::exists(this->dataPtr->configPath))
  {
    if (!ignition::common::copyFile(
          initialConfigFile, this->dataPtr->configPath))
    {
      std::cerr << "Error copying default configuration file from ["
                << initialConfigFile << "] to [" << this->dataPtr->configPath
                << "]" << std::endl;
    }
  }

  if (!this->LoadConfig())
  {
    ignerr << "Error loading configuration file [" << this->dataPtr->configPath
           << std::endl;
  }

  std::string ignFuelPath = "";
  if (ignition::common::env("IGN_FUEL_PATH", ignFuelPath))
  {
    if (!ignition::common::isDirectory(ignFuelPath))
    {
      ignerr << "[" << ignFuelPath << "] is not a directory" << std::endl;
      return;
    }
    this->CacheLocation(ignFuelPath);
  }
}

//////////////////////////////////////////////////
ClientConfig::ClientConfig(const ClientConfig &_copy)
  : dataPtr(new ClientConfigPrivate)
{
  *(this->dataPtr) = *(_copy.dataPtr);
}

//////////////////////////////////////////////////
ClientConfig &ClientConfig::operator=(const ClientConfig &_rhs)
{
  *(this->dataPtr) = *(_rhs.dataPtr);

  return *this;
}

//////////////////////////////////////////////////
ClientConfig::~ClientConfig()
{
}

//////////////////////////////////////////////////
bool ClientConfig::LoadConfig()
{
  // Sanity check: Verify that the configuration file exists.
  if (!ignition::common::exists(this->dataPtr->configPath))
    return false;

  YAML::Node config = YAML::LoadFile(this->dataPtr->configPath);

  try
  {
    // Server configuration.
    if (config["servers"])
    {
      auto servers = config["servers"];
      for (auto it = servers.begin(); it != servers.end(); ++it)
      {
        auto server = *it;
        if (!server["name"])
        {
          ignerr << "Missing [name] key. Ignoring server." << std::endl;
          continue;
        }
        auto name = server["name"].as<std::string>();
        if (!server["url"])
        {
          ignerr << "Missing [url] key. Ignoring server." << std::endl;
          continue;
        }
        auto url = server["url"].as<std::string>();

        // Sanity check: Make sure that the URL is not already stored.
        bool repeated = false;
        for (auto const savedServer : this->Servers())
        {
          if (savedServer.LocalName() == name)
          {
            ignerr << "Server [" << name << "] already exists. Ignoring server"
                   << std::endl;
            repeated = true;
            break;
          }
          if (savedServer.URL() == url)
          {
            ignerr << "URL [" << url << "] already exists. Ignoring server"
                   << std::endl;
            repeated = true;
            break;
          }
        }
        if (repeated)
          continue;

        ServerConfig newServer;
        newServer.LocalName(name);
        newServer.URL(url);
        this->AddServer(newServer);
      }

      // Cache configuration.
      std::string cacheLocation = ignition::common::joinPaths(
        homePath(), ".ignition", "fuel");
      if (config["cache"])
      {
        auto cache = config["cache"];
        if (cache["path"])
          cacheLocation = cache["path"].as<std::string>();
        else
        {
          ignerr << "Missing [path] key in [cache] section. Ignoring cache."
                 << std::endl;
        }
      }
      this->CacheLocation(cacheLocation);
    }
  }
  catch (const std::exception& e)
  {
    return false;
  }

  return true;
}

//////////////////////////////////////////////////
std::vector<ServerConfig> ClientConfig::Servers() const
{
  return this->dataPtr->servers;
}

//////////////////////////////////////////////////
void ClientConfig::AddServer(const ServerConfig &_srv)
{
  this->dataPtr->servers.push_back(_srv);
}

//////////////////////////////////////////////////
std::string ClientConfig::CacheLocation() const
{
  return this->dataPtr->cacheLocation;
}

//////////////////////////////////////////////////
void ClientConfig::CacheLocation(const std::string &_path)
{
  this->dataPtr->cacheLocation = _path;
}

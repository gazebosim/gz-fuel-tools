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

#include <yaml.h>
#include <cstdio>
#include <stack>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/config.hh"

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
  public: common::URI url;

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
  return this->dataPtr->url.Str();
}

//////////////////////////////////////////////////
void ServerConfig::URL(const std::string &_url)
{
  this->SetUrl(common::URI(_url));
}

//////////////////////////////////////////////////
common::URI ServerConfig::Url() const
{
  return this->dataPtr->url;
}

//////////////////////////////////////////////////
void ServerConfig::SetUrl(const common::URI &_url)
{
  this->dataPtr->url = _url;
}

//////////////////////////////////////////////////
std::string ServerConfig::LocalName() const
{
  ignwarn << "LocalName is not used and will be deprecated on version 2"
          << std::endl;
  return "";
}

//////////////////////////////////////////////////
void ServerConfig::LocalName(const std::string &/*_name*/)
{
  ignwarn << "LocalName is not used and will be deprecated on version 2"
          << std::endl;
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
  std::string ignFuelPath = "";
  if (ignition::common::env("IGN_FUEL_CACHE_PATH", ignFuelPath))
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
  // SetConfigPath() wasn't used. Using the default one.
  if (this->dataPtr->configPath.empty())
  {
    auto configDir = ignition::common::joinPaths(
      homePath(), ".ignition", "fuel");
    if (!ignition::common::exists(configDir) &&
        !ignition::common::createDirectories(configDir))
    {
      ignerr << "Error creating default configuration directory ["
             << configDir << "]" << std::endl;
      return false;
    }

    this->dataPtr->configPath = ignition::common::joinPaths(
      configDir, "config.yaml");

    // If the custom config file doesn't exist, we create it.
    if (!ignition::common::exists(this->dataPtr->configPath))
    {
      if (!ignition::common::copyFile(
            initialConfigFile, this->dataPtr->configPath))
      {
        ignerr << "Error copying default configuration file from ["
               << initialConfigFile << "] to [" << this->dataPtr->configPath
               << "]" << std::endl;
        return false;
      }
    }
  }

  // Sanity check: Verify that the configuration file exists.
  if (!ignition::common::exists(this->dataPtr->configPath))
  {
    ignerr << "Unable to find configuration file in  ["
           << this->dataPtr->configPath << "]" << std::endl;
    return false;
  }

  FILE *fh = fopen(this->dataPtr->configPath.c_str(), "r");
  if (!fh)
  {
    ignerr << "Failed to open file [" << this->dataPtr->configPath
           << "]" << std::endl;
    return false;
  }

  // Initialize parser.
  yaml_parser_t parser;
  if (!yaml_parser_initialize(&parser))
  {
    ignerr << "Failed to initialize parser" << std::endl;
    fclose(fh);
    return false;
  }

  // Set input file.
  yaml_parser_set_input_file(&parser, fh);

  bool res = true;
  yaml_event_t event;
  std::stack<std::string> tokens;
  tokens.push("root");
  std::string serverURL = "";
  std::string cacheLocationConfig = "";

  do
  {
    if (!yaml_parser_parse(&parser, &event))
    {
      ignerr << "Parser error [" << parser.error << "]" << std::endl;
      res = false;
      break;
    }

    switch (event.type)
    {
      case YAML_NO_EVENT:
      // Stream start/end.
      case YAML_STREAM_START_EVENT:
      case YAML_STREAM_END_EVENT:
      // Block delimeters.
      case YAML_DOCUMENT_START_EVENT:
      case YAML_DOCUMENT_END_EVENT:
      case YAML_SEQUENCE_START_EVENT:
        break;
      case YAML_SEQUENCE_END_EVENT:
        if (!tokens.empty())
          tokens.pop();
        break;
      case YAML_MAPPING_START_EVENT:
        if (!tokens.empty() && tokens.top() == "servers")
        {
          tokens.push("server");
          serverURL = "";
        }
        break;
      case YAML_MAPPING_END_EVENT:
        if (!tokens.empty() && tokens.top() == "cache")
        {
          if (cacheLocationConfig.empty())
          {
            ignerr << "[path] parameter is required for a cache" << std::endl;
            res = false;
          }
        }
        else if (!tokens.empty() && tokens.top() == "server")
        {
          if (!serverURL.empty())
          {
            // Sanity check: Make sure that the server is not already stored.
            bool repeated = false;
            for (auto const savedServer : this->Servers())
            {
              if (savedServer.URL() == serverURL)
              {
                ignerr << "URL [" << serverURL << "] already exists. "
                       << "Ignoring server" << std::endl;
                repeated = true;
                res = false;
                break;
              }
            }
            if (!repeated)
            {
              // Add the new server.
              ServerConfig newServer;
              newServer.URL(serverURL);
              this->AddServer(newServer);
            }
          }
          else
          {
            ignerr << "[url] parameter is required for a server"
                      << std::endl;
            res = false;
          }
        }

        if (!tokens.empty())
          tokens.pop();
        break;
      // Data .
      case YAML_ALIAS_EVENT:
        break;
      case YAML_SCALAR_EVENT:
        if (!tokens.empty() && tokens.top() == "url")
        {
          std::string url(
            reinterpret_cast<const char *>(event.data.scalar.value));
          serverURL = url;
          tokens.pop();
        }
        else if (!tokens.empty() && tokens.top() == "path")
        {
          std::string path(
            reinterpret_cast<const char *>(event.data.scalar.value));
          cacheLocationConfig = path;
          tokens.pop();
        }
        else
        {
          std::string key(
            reinterpret_cast<const char *>(event.data.scalar.value));
          if (!key.empty())
            tokens.push(key);
        }

        break;
      default:
      {
        ignerr << "Unknown event type [" << event.type << "]" << std::endl;
        res = false;
      }
    }

    if (event.type != YAML_STREAM_END_EVENT)
      yaml_event_delete(&event);
  } while (event.type != YAML_STREAM_END_EVENT);

  // Default cache path.
  std::string cacheLocation = ignition::common::joinPaths(
    homePath(), ".ignition", "fuel");

  // The user wants to overwrite the default cache path.
  if (!cacheLocationConfig.empty())
    cacheLocation = cacheLocationConfig;

  // Do not overwrite the cache location if IGN_FUEL_CACHE_PATH is set.
  std::string ignFuelPath = "";
  if (ignition::common::env("IGN_FUEL_CACHE_PATH", ignFuelPath))
  {
    ignwarn << "IGN_FUEL_CACHE_PATH is set to [" << ignFuelPath << "]. The "
            << "path in the configuration file will be ignored" << std::endl;
  }
  else
    this->CacheLocation(cacheLocation);

  // Cleanup.
  yaml_parser_delete(&parser);
  fclose(fh);

  return res;
}

//////////////////////////////////////////////////
void ClientConfig::SetConfigPath(const std::string &_path)
{
  this->dataPtr->configPath = _path;
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

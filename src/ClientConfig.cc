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
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/common/Util.hh>

#include "gz/fuel_tools/ClientConfig.hh"
#include "gz/fuel_tools/config.hh"

using namespace ignition;
using namespace fuel_tools;

//////////////////////////////////////////////////
/// \brief Private data class
class gz::fuel_tools::ClientConfigPrivate
{
  /// \brief Constructor.
  public: ClientConfigPrivate()
          {
            std::string homePath;
            common::env(IGN_HOMEDIR, homePath);
            this->cacheLocation = common::joinPaths(
                homePath, ".ignition", "fuel");

            this->servers.push_back(ServerConfig());

            // Add in fuel.ignitionrobotics.org as another default server
            // config.
            ServerConfig ignServerConfig;
            ignServerConfig.SetUrl(
                common::URI("https://fuel.ignitionrobotics.org"));
            ignServerConfig.SetVersion("1.0");
            this->servers.push_back(ignServerConfig);
          }

  /// \brief Clear values.
  public: void Clear()
          {
            this->servers.clear();
            this->cacheLocation = "";
            this->configPath = "";
            this->userAgent =
              "IgnitionFuelTools-" IGNITION_FUEL_TOOLS_VERSION_FULL;
          }

  /// \brief A list of servers.
  public: std::vector<ServerConfig> servers;

  /// \brief a path on disk to where data is cached.
  public: std::string cacheLocation = "";

  /// \brief The path where the configuration file is located.
  public: std::string configPath = "";

  /// \brief Name of the user agent.
  public: std::string userAgent =
          "IgnitionFuelTools-" IGNITION_FUEL_TOOLS_VERSION_FULL;
};

//////////////////////////////////////////////////
/// \brief Private data class
class gz::fuel_tools::ServerConfigPrivate
{
  /// \brief Clear values.
  public: void Clear()
          {
            this->url.Clear();
            this->key = "";
            this->version = "1.0";
          }

  /// \brief URL to reach server
  public: common::URI url{"https://fuel.gazebosim.org"};

  /// \brief A key to auth with the server
  public: std::string key = "";

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
void ServerConfig::Clear()
{
  this->dataPtr->Clear();
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
std::string ServerConfig::ApiKey() const
{
  return this->dataPtr->key;
}

//////////////////////////////////////////////////
void ServerConfig::SetApiKey(const std::string &_key)
{
  this->dataPtr->key = _key;
}

//////////////////////////////////////////////////
std::string ServerConfig::Version() const
{
  return this->dataPtr->version;
}

//////////////////////////////////////////////////
void ServerConfig::SetVersion(const std::string &_version)
{
  this->dataPtr->version = _version;
}

//////////////////////////////////////////////////
std::string ServerConfig::AsString(const std::string &_prefix) const
{
  std::stringstream out;
  out << _prefix << "URL: " << this->Url().Str() << std::endl
      << _prefix << "Version: " << this->Version() << std::endl
      << _prefix << "API key: " << this->ApiKey() << std::endl;
  return out.str();
}

//////////////////////////////////////////////////
std::string ServerConfig::AsPrettyString(const std::string &_prefix) const
{
  std::string prop = "\033[96m\033[1m";
  std::string value = "\033[37m";
  std::string reset = "\033[0m";

  std::stringstream out;

  if (this->Url().Valid())
  {
    out << _prefix << prop << "URL: " << reset
        << value << this->Url().Str() << reset << std::endl;
  }

  if (!this->Version().empty())
  {
    out << _prefix << prop << "Version: " << reset
        << value << this->Version() << reset << std::endl;
  }

  if (!this->ApiKey().empty())
  {
    out << _prefix << prop << "API key: " << reset
        << value << this->ApiKey() << reset << std::endl;
  }
  return out.str();
}

//////////////////////////////////////////////////
ClientConfig::ClientConfig() : dataPtr(new ClientConfigPrivate)
{
  std::string ignFuelPath = "";
  if (common::env("IGN_FUEL_CACHE_PATH", ignFuelPath))
  {
    if (!common::isDirectory(ignFuelPath))
    {
      ignerr << "[" << ignFuelPath << "] is not a directory" << std::endl;
      return;
    }
    this->SetCacheLocation(ignFuelPath);
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
void ClientConfig::Clear()
{
  this->dataPtr->Clear();
}

//////////////////////////////////////////////////
bool ClientConfig::LoadConfig(const std::string &_file)
{
  // Sanity check: Verify that the configuration file exists.
  if (!common::exists(_file))
  {
    ignerr << "Unable to find configuration file [" << _file<< "]" << std::endl;
    return false;
  }


  FILE *fh = fopen(_file.c_str(), "r");
  if (!fh)
  {
    ignerr << "Failed to open configuration file ["
      << _file << "]" << std::endl;
    return false;
  }

  this->dataPtr->configPath = _file;

  // Initialize parser.
  yaml_parser_t parser;
  if (!yaml_parser_initialize(&parser))
  {
    ignerr << "Failed to initialize YAML parser" << std::endl;
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
  std::string privateToken = "";

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
            for (auto & savedServer : this->MutableServers())
            {
              if (savedServer.Url().Str() == serverURL)
              {
                if (!privateToken.empty())
                {
                  ignmsg << "Set private token for " << serverURL << " server."
                    << std::endl;
                  savedServer.SetApiKey(privateToken);
                }
                ignwarn << "URL [" << serverURL << "] already exists. "
                  << "Ignoring server" << std::endl;
                repeated = true;
                break;
              }
            }
            if (!repeated)
            {
              // Add the new server.
              ServerConfig newServer;
              newServer.SetUrl(common::URI(serverURL));
              if (!privateToken.empty())
              {
                ignmsg << "Set private token for " << serverURL << " server."
                  << std::endl;
                newServer.SetApiKey(privateToken);
              }
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
        else if (!tokens.empty() && tokens.top() == "private-token")
        {
          std::string token(
            reinterpret_cast<const char *>(event.data.scalar.value));
          privateToken = token;
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
  std::string homePath;
  common::env(IGN_HOMEDIR, homePath);
  std::string cacheLocation = common::joinPaths(
    homePath, ".ignition", "fuel");

  // The user wants to overwrite the default cache path.
  if (!cacheLocationConfig.empty())
    cacheLocation = cacheLocationConfig;

  // Do not overwrite the cache location if IGN_FUEL_CACHE_PATH is set.
  std::string ignFuelPath = "";
  if (common::env("IGN_FUEL_CACHE_PATH", ignFuelPath))
  {
    ignwarn << "IGN_FUEL_CACHE_PATH is set to [" << ignFuelPath << "]. The "
            << "path in the configuration file will be ignored" << std::endl;
    cacheLocation = ignFuelPath;
  }
  this->SetCacheLocation(cacheLocation);

  // Cleanup.
  yaml_parser_delete(&parser);
  fclose(fh);

  return res;
}

//////////////////////////////////////////////////
std::string ClientConfig::ConfigPath() const
{
  return this->dataPtr->configPath;
}

//////////////////////////////////////////////////
std::vector<ServerConfig> ClientConfig::Servers() const
{
  return this->dataPtr->servers;
}

//////////////////////////////////////////////////
std::vector<ServerConfig> & ClientConfig::MutableServers() const
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
void ClientConfig::SetCacheLocation(const std::string &_path)
{
  this->dataPtr->cacheLocation = _path;
}

//////////////////////////////////////////////////
void ClientConfig::SetUserAgent(const std::string &_agent)
{
  this->dataPtr->userAgent = _agent;
}

//////////////////////////////////////////////////
const std::string &ClientConfig::UserAgent() const
{
  return this->dataPtr->userAgent;
}

//////////////////////////////////////////////////
std::string ClientConfig::AsString(const std::string &_prefix) const
{
  std::stringstream out;
  out << _prefix << "Config path: " << this->ConfigPath() << std::endl
      << _prefix << "Cache location: " << this->CacheLocation() << std::endl
      << _prefix << "Servers:" << std::endl;

  for (auto s : this->Servers())
  {
    out << _prefix << "  ---" << std::endl;
    out << _prefix << s.AsString("  ");
  }

  return out.str();
}

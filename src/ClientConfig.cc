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

#include <cstdio>
#include <sstream>
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

//////////////////////////////////////////////////
/// \brief Private data class
class ignition::fuel_tools::ClientConfigPrivate
{
  /// \brief Constructor.
  public: ClientConfigPrivate()
          {
            std::string homePath;
            ignition::common::env(IGN_HOMEDIR, homePath);
            this->cacheLocation = common::joinPaths(
                homePath, ".ignition", "fuel");

            this->servers.push_back(ServerConfig());
          }

  /// \brief Clear values.
  public: void Clear()
          {
            this->servers.clear();
            this->cacheLocation = "";
            this->userAgent =
              "IgnitionFuelTools-" IGNITION_FUEL_TOOLS_VERSION_FULL;
          }

  /// \brief A list of servers.
  public: std::vector<ServerConfig> servers;

  /// \brief a path on disk to where data is cached.
  public: std::string cacheLocation = "";

  /// \brief Name of the user agent.
  public: std::string userAgent =
          "IgnitionFuelTools-" IGNITION_FUEL_TOOLS_VERSION_FULL;
};

//////////////////////////////////////////////////
/// \brief Private data class
class ignition::fuel_tools::ServerConfigPrivate
{
  /// \brief Clear values.
  public: void Clear()
          {
            this->url.Clear();
            this->key = "";
            this->version = "1.0";
          }

  /// \brief URL to reach server
  public: common::URI url{"https://fuel.ignitionrobotics.org"};

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
  if (ignition::common::env("IGN_FUEL_CACHE_PATH", ignFuelPath))
  {
    if (!ignition::common::isDirectory(ignFuelPath))
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
  out << _prefix << "Cache location: " << this->CacheLocation() << std::endl
      << _prefix << "Servers:" << std::endl;

  for (auto s : this->Servers())
  {
    out << _prefix << "  ---" << std::endl;
    out << _prefix << s.AsString("  ");
  }

  return out.str();
}

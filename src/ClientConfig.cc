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

#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include "ignition/fuel-tools/ClientConfig.hh"

using namespace ignition;
using namespace fuel_tools;

//////////////////////////////////////////////////
/// \brief Private data class
class ignition::fuel_tools::ClientConfigPrivate
{
  /// \brief A list of servers.
  public: std::vector<ServerConfig> servers;

  /// \brief a path on disk to where data is cached
  public: std::string cacheLocation;
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
  /// \brief The path containing the default cache location.
  const std::string kDefaultCacheLocation = ignition::common::joinPaths(
    homePath(), ".ignition", "fuel", "models");
  this->CacheLocation(kDefaultCacheLocation);

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

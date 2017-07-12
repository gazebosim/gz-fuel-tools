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

#include <ignition/fuel-tools/ClientConfig.hh>


namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

//////////////////////////////////////////////////
/// \brief Private data class
class ignft::ClientConfigPrivate
{
  /// \brief A list of servers;
  public: std::vector<ServerConfig> servers;

  /// \brief a path on disk to where data is cached
  public: std::string cacheLocation;
};

//////////////////////////////////////////////////
/// \brief Private data class
class ignft::ServerConfigPrivate
{
  /// \brief URL to reach server
  public: std::string url;

  /// \brief Local directory server stuff is saved in
  public: std::string localName;

  /// \brief A key to auth with the server
  public: std::string key;
};

//////////////////////////////////////////////////
ServerConfig::ServerConfig() : dataPtr (new ServerConfigPrivate)
{
}

//////////////////////////////////////////////////
ServerConfig::ServerConfig(const ServerConfig &_orig)
 : dataPtr(new ServerConfigPrivate)
{
  *(this->dataPtr) = *(_orig.dataPtr);
}

//////////////////////////////////////////////////
void ServerConfig::operator=(const ServerConfig &_orig)
{
  *(this->dataPtr) = *(_orig.dataPtr);
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
  this->dataPtr->url = _url;
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
ClientConfig::ClientConfig() : dataPtr(new ClientConfigPrivate)
{
}

//////////////////////////////////////////////////
ClientConfig::ClientConfig(const ClientConfig &_copy)
 : dataPtr(new ClientConfigPrivate)
{
  *(this->dataPtr) = *(_copy.dataPtr);
}

//////////////////////////////////////////////////
void ClientConfig::operator=(const ClientConfig &_rhs)
{
  *(this->dataPtr) = *(_rhs.dataPtr);
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

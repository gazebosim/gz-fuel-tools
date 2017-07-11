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

/// \brief Private data class
class ignft::ClientConfigPrivate
{
  /// \brief A list of servers;
  public: std::vector<std::string> servers;

  /// \brief a path on disk to where data is cached
  public: std::string cacheLocation;
};

//////////////////////////////////////////////////
ClientConfig::ClientConfig() : dataPtr(new ClientConfigPrivate)
{
}

//////////////////////////////////////////////////
ClientConfig::ClientConfig(const ClientConfig &_copy)
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
std::vector<std::string> ClientConfig::Servers() const
{
  return this->dataPtr->servers;
}

//////////////////////////////////////////////////
void ClientConfig::AddServer(const std::string &_url)
{
  this->dataPtr->servers.push_back(_url);
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

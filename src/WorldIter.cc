/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <memory>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/JSONParser.hh"
#include "ignition/fuel_tools/World.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"
#include "ignition/fuel_tools/WorldIter.hh"
#include "ignition/fuel_tools/WorldIterPrivate.hh"
#include "ignition/fuel_tools/WorldPrivate.hh"
#include "ignition/fuel_tools/REST.hh"

using namespace ignition;
using namespace fuel_tools;

//////////////////////////////////////////////////
WorldIter WorldIterFactory::Create(const std::vector<WorldIdentifier> &_ids)
{
  std::unique_ptr<WorldIterPrivate> priv(new WorldIterIds(_ids));
  return std::move(WorldIter(std::move(priv)));
}

//////////////////////////////////////////////////
WorldIter WorldIterFactory::Create(const REST &_rest,
    const ServerConfig &_server, const std::string &_api)
{
  std::unique_ptr<WorldIterPrivate> priv(new WorldIterRESTIds(
    _rest, _server, _api));
  return std::move(WorldIter(std::move(priv)));
}

//////////////////////////////////////////////////
WorldIter WorldIterFactory::Create()
{
  std::unique_ptr<WorldIterPrivate> priv(new WorldIterIds({}));
  return std::move(WorldIter(std::move(priv)));
}

//////////////////////////////////////////////////
WorldIterPrivate::~WorldIterPrivate()
{
}

//////////////////////////////////////////////////
WorldIterIds::~WorldIterIds()
{
}

//////////////////////////////////////////////////
WorldIterIds::WorldIterIds(std::vector<WorldIdentifier> _ids)
  : ids(_ids)
{
  this->idIter = this->ids.begin();
  if (!this->ids.empty())
  {
    std::shared_ptr<WorldPrivate> ptr(new WorldPrivate);
    ptr->id = *(this->idIter);
    this->world = World(ptr);
  }
}

//////////////////////////////////////////////////
void WorldIterIds::Next()
{
  // advance pointer
  ++(this->idIter);

  // Update personal world class
  if (this->idIter != this->ids.end())
  {
    std::shared_ptr<WorldPrivate> ptr(new WorldPrivate);
    ptr->id = *(this->idIter);
    this->world = World(ptr);
  }
}

//////////////////////////////////////////////////
bool WorldIterIds::HasReachedEnd()
{
  return this->ids.empty() || this->idIter == this->ids.end();
}

//////////////////////////////////////////////////
IterWorlds::~IterWorlds()
{
}

//////////////////////////////////////////////////
IterWorlds::IterWorlds(std::vector<World> _worlds)
  : worlds(_worlds)
{
  this->worldIter = this->worlds.begin();
  if (!this->worlds.empty())
  {
    this->world = this->worlds.front();
  }
}

//////////////////////////////////////////////////
void IterWorlds::Next()
{
  // advance pointer
  ++(this->worldIter);

  // Update personal world class
  if (this->worldIter != this->worlds.end())
  {
    this->world = *(this->worldIter);
  }
}

//////////////////////////////////////////////////
bool IterWorlds::HasReachedEnd()
{
  return this->worlds.empty() || this->worldIter == this->worlds.end();
}

//////////////////////////////////////////////////
WorldIterRESTIds::~WorldIterRESTIds()
{
}

//////////////////////////////////////////////////
WorldIterRESTIds::WorldIterRESTIds(const REST &_rest,
    const ServerConfig &_config, const std::string &_api)
  : config(_config), rest(_rest)
{
  REST::Method method = REST::GET;
  this->config = _config;
  std::vector<std::string> headers = {"Accept: application/json"};
  RESTResponse resp;
  std::vector<WorldIdentifier> worldIds;
  this->ids.clear();

  // Empty query string will get the first page of worlds.
  std::string queryStrPage  = "";
  std::string queryStrPageKey = "page=";
  do
  {
    // Fire the request.
    resp = this->rest.Request(method, this->config.URL(),
      this->config.Version(), _api, {queryStrPage}, headers, "");

    // Reset the query string
    queryStrPage = "";

    // Get the next page from the headers.
    if (resp.headers.find("Link") != resp.headers.end())
    {
      std::vector<std::string> links = ignition::common::split(
          resp.headers["Link"], ",");
      for (const auto &l : links)
      {
        if (l.find("next") != std::string::npos)
        {
          auto start = l.find(queryStrPageKey);
          auto end = l.find(">", start+1);
          queryStrPage = l.substr(start, end-start);
          break;
        }
      }
    }

    // Fallsafe - break if response code is invalid
    if (resp.data == "null\n" || resp.statusCode != 200)
    {
      break;
    }

    // Parse the response.
    worldIds = JSONParser::ParseWorlds(resp.data, this->config);

    // Add the vector of worlds to the list.
    this->ids.insert(std::end(this->ids), std::begin(worldIds),
      std::end(worldIds));
  } while (!worldIds.empty() && !queryStrPage.empty());

  if (this->ids.empty())
    return;

  this->idIter = this->ids.begin();

  // make first world
  std::shared_ptr<WorldPrivate> ptr(new WorldPrivate);
  ptr->id = *(this->idIter);
  ptr->id.SetServer(this->config);
  this->world = World(ptr);

  igndbg << "Got response [" << resp.data << "]\n";
}

//////////////////////////////////////////////////
void WorldIterRESTIds::Next()
{
  // advance pointer
  ++(this->idIter);

  // Update personal world class
  if (this->idIter != this->ids.end())
  {
    std::shared_ptr<WorldPrivate> ptr(new WorldPrivate);
    ptr->id = *(this->idIter);
    ptr->id.SetServer(this->config);
    this->world = World(ptr);
  }
  // TODO request next page if api is paginated
}

//////////////////////////////////////////////////
bool WorldIterRESTIds::HasReachedEnd()
{
  return this->ids.empty() || this->idIter == this->ids.end();
}

//////////////////////////////////////////////////
WorldIter::WorldIter(std::unique_ptr<WorldIterPrivate> _dptr)
{
  this->dataPtr.reset(_dptr.release());
}

//////////////////////////////////////////////////
WorldIter::WorldIter(WorldIter && _old)
{
  this->dataPtr.reset(_old.dataPtr.release());
}

//////////////////////////////////////////////////
WorldIter::~WorldIter()
{
}

//////////////////////////////////////////////////
WorldIter::operator bool() const
{
  return !this->dataPtr->HasReachedEnd();
}

//////////////////////////////////////////////////
WorldIter &WorldIter::operator++()
{
  // TODO Request more data if there are more pages
  if (!this->dataPtr->HasReachedEnd())
  {
    this->dataPtr->Next();
  }
  return *this;
}

//////////////////////////////////////////////////
World &WorldIter::operator*()
{
  return this->dataPtr->world;
}

//////////////////////////////////////////////////
World *WorldIter::operator->()
{
  return &(this->dataPtr->world);
}

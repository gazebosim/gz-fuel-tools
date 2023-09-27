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

#include <gz/fuel_tools/ModelIter.hh>
#include "ModelIterPrivate.hh"

#include <regex>

namespace gz
{
namespace fuel_tools
{

//////////////////////////////////////////////////
ModelIterPrivate::~ModelIterPrivate() = default;

//////////////////////////////////////////////////
IterIds::~IterIds() = default;

//////////////////////////////////////////////////
IterIds::IterIds(const std::vector<ModelIdentifier> &_ids)
  : ids(_ids)
{
  this->idIter = this->ids.begin();
  if (!this->ids.empty())
  {
    this->model = Model(*this->idIter);
  }
}

//////////////////////////////////////////////////
void IterIds::Next()
{
  // advance pointer
  ++(this->idIter);

  // Update personal model class
  if (this->idIter != this->ids.end())
  {
    this->model = Model(*this->idIter);
  }
}

//////////////////////////////////////////////////
bool IterIds::HasReachedEnd() const
{
  return this->ids.empty() || this->idIter == this->ids.end();
}

//////////////////////////////////////////////////
IterModels::~IterModels() = default;

//////////////////////////////////////////////////
IterModels::IterModels(const std::vector<Model> &_models)
  : models(_models)
{
  this->modelIter = this->models.begin();
  if (!this->models.empty())
  {
    this->model = this->models.front();
  }
}

//////////////////////////////////////////////////
void IterModels::Next()
{
  // advance pointer
  ++(this->modelIter);

  // Update personal model class
  if (this->modelIter != this->models.end())
  {
    this->model = *(this->modelIter);
  }
}

//////////////////////////////////////////////////
bool IterModels::HasReachedEnd() const
{
  return this->models.empty() || this->modelIter == this->models.end();
}

//////////////////////////////////////////////////
IterRestIds::~IterRestIds() = default;

//////////////////////////////////////////////////
std::vector<ModelIdentifier> IterRestIds::ParseIdsFromResponse(
    const RestResponse &resp)
{
  if (resp.data == "null\n" || resp.statusCode != 200)
    return {};

  // Parse the response.
  return JSONParser::ParseModels(resp.data, this->config);
}

//////////////////////////////////////////////////
IterRestIds::IterRestIds(const Rest &_rest, const ServerConfig &_server,
    const std::string &_api)
  : config(_server), rest(_rest), api(_api)
{
  this->idIter = this->ids.begin();
  this->Next();
}

//////////////////////////////////////////////////
RestResponse IterRestIds::MakeRestRequest(std::size_t _page)
{
  HttpMethod method = HttpMethod::GET;
  std::vector<std::string> headers = {"Accept: application/json"};
  // Prepare the request with the requested page.
  std::string queryStrPage = "page=" + std::to_string(_page);
  std::string path = this->api;
  // Fire the request.
  return this->rest.Request(method, this->config.Url().Str(),
      this->config.Version(),
      std::regex_replace(path, std::regex(R"(\\)"), "/"),
      {queryStrPage}, headers, "");
}

//////////////////////////////////////////////////
void IterRestIds::Next()
{
  // advance pointer
  if (this->idIter != this->ids.end())
    ++(this->idIter);

  if (this->idIter == this->ids.end())
  {
    ++this->currentPage;
    RestResponse resp = this->MakeRestRequest(this->currentPage);
    this->ids = this->ParseIdsFromResponse(resp);
    this->idIter = this->ids.begin();
  }

  // Update personal model class
  if (this->idIter != this->ids.end())
  {
    auto id = *this->idIter;
    id.SetServer(this->config);
    this->model = Model(id);
  }
}

//////////////////////////////////////////////////
bool IterRestIds::HasReachedEnd() const
{
  return this->ids.empty() || this->idIter == this->ids.end();
}


}  // namespace fuel_tools
}  // namespace gz

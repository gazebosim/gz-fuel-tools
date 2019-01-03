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

#include <memory>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/JSONParser.hh"
#include "ignition/fuel_tools/Model.hh"
#include "ignition/fuel_tools/ModelIdentifier.hh"
#include "ignition/fuel_tools/ModelIter.hh"
#include "ignition/fuel_tools/ModelIterPrivate.hh"
#include "ignition/fuel_tools/ModelPrivate.hh"
#include "ignition/fuel_tools/RestClient.hh"

using namespace ignition;
using namespace fuel_tools;

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create(const std::vector<ModelIdentifier> &_ids)
{
  std::unique_ptr<ModelIterPrivate> priv(new IterIds(_ids));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create(const std::vector<Model> &_models)
{
  std::unique_ptr<ModelIterPrivate> priv(new IterModels(_models));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create(const Rest &_rest,
    const ServerConfig &_server, const std::string &_api)
{
  std::unique_ptr<ModelIterPrivate> priv(new IterRestIds(
    _rest, _server, _api));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create()
{
  std::unique_ptr<ModelIterPrivate> priv(new IterIds({}));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIterPrivate::~ModelIterPrivate()
{
}

//////////////////////////////////////////////////
IterIds::~IterIds()
{
}

//////////////////////////////////////////////////
IterIds::IterIds(std::vector<ModelIdentifier> _ids)
  : ids(_ids)
{
  this->idIter = this->ids.begin();
  if (!this->ids.empty())
  {
    std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
    ptr->id = *(this->idIter);
    this->model = Model(ptr);
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
    std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
    ptr->id = *(this->idIter);
    this->model = Model(ptr);
  }
}

//////////////////////////////////////////////////
bool IterIds::HasReachedEnd()
{
  return this->ids.empty() || this->idIter == this->ids.end();
}

//////////////////////////////////////////////////
IterModels::~IterModels()
{
}

//////////////////////////////////////////////////
IterModels::IterModels(std::vector<Model> _models)
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
bool IterModels::HasReachedEnd()
{
  return this->models.empty() || this->modelIter == this->models.end();
}

//////////////////////////////////////////////////
IterRestIds::~IterRestIds()
{
}

//////////////////////////////////////////////////
IterRestIds::IterRestIds(const Rest &_rest, const ServerConfig &_config,
    const std::string &_api)
  : config(_config), rest(_rest)
{
  HttpMethod method = HttpMethod::GET;
  this->config = _config;
  int page = 1;
  std::vector<std::string> headers = {"Accept: application/json"};
  RestResponse resp;
  std::vector<ModelIdentifier> modelIds;
  this->ids.clear();

  do
  {
    // Prepare the request with the next page.
    std::string queryStrPage = "page=" + std::to_string(page);
    std::string path = _api;
    ++page;

    // Fire the request.
    resp = this->rest.Request(method, this->config.Url().Str(),
      this->config.Version(), path, {queryStrPage}, headers, "");

    // TODO(nkoenig): resp.statusCode should return != 200 when the page
    // requested does
    // not exist. When this happens we should stop without calling ParseModels()
    // https://bitbucket.org/ignitionrobotics/ign-fuelserver/issues/7
    if (resp.data == "null\n" || resp.statusCode != 200)
      break;

    // Parse the response.
    modelIds = JSONParser::ParseModels(resp.data, this->config);

    // Add the vector of models to the list.
    this->ids.insert(std::end(this->ids), std::begin(modelIds),
      std::end(modelIds));
  } while (!modelIds.empty());

  if (this->ids.empty())
    return;

  this->idIter = this->ids.begin();

  // make first model
  std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
  ptr->id = *(this->idIter);
  ptr->id.SetServer(this->config);
  this->model = Model(ptr);

  igndbg << "Got response [" << resp.data << "]\n";
}

//////////////////////////////////////////////////
void IterRestIds::Next()
{
  // advance pointer
  ++(this->idIter);

  // Update personal model class
  if (this->idIter != this->ids.end())
  {
    std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
    ptr->id = *(this->idIter);
    ptr->id.SetServer(this->config);
    this->model = Model(ptr);
  }
  // TODO(nkoenig) request next page if api is paginated
}

//////////////////////////////////////////////////
bool IterRestIds::HasReachedEnd()
{
  return this->ids.empty() || this->idIter == this->ids.end();
}

//////////////////////////////////////////////////
ModelIter::ModelIter(std::unique_ptr<ModelIterPrivate> _dptr)
{
  this->dataPtr.reset(_dptr.release());
}

//////////////////////////////////////////////////
ModelIter::ModelIter(ModelIter && _old)
{
  this->dataPtr.reset(_old.dataPtr.release());
}

//////////////////////////////////////////////////
ModelIter::~ModelIter()
{
}

//////////////////////////////////////////////////
ModelIter::operator bool()
{
  return !this->dataPtr->HasReachedEnd();
}

//////////////////////////////////////////////////
ModelIter::operator bool() const
{
  return !this->dataPtr->HasReachedEnd();
}

//////////////////////////////////////////////////
ModelIter &ModelIter::operator++()
{
  // TODO(nkoenig) Request more data if there are more pages
  if (!this->dataPtr->HasReachedEnd())
  {
    this->dataPtr->Next();
  }
  return *this;
}

//////////////////////////////////////////////////
Model &ModelIter::operator*()
{
  return this->dataPtr->model;
}

//////////////////////////////////////////////////
Model *ModelIter::operator->()
{
  return &(this->dataPtr->model);
}

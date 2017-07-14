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

#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/fuel-tools/ClientConfig.hh>
#include <ignition/fuel-tools/JSONParser.hh>
#include <ignition/fuel-tools/ModelIter.hh>
#include <ignition/fuel-tools/ModelIterPrivate.hh>
#include <ignition/fuel-tools/ModelPrivate.hh>
#include <ignition/fuel-tools/REST.hh>

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create(std::vector<ModelIdentifier> _ids)
{
  std::unique_ptr<ModelIterPrivate> priv(new IterIds(_ids));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create()
{
  std::unique_ptr<ModelIterPrivate> priv(new IterIds({}));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create(std::vector<Model> _models)
{
  std::unique_ptr<ModelIterPrivate> priv(new IterModels(_models));
  return std::move(ModelIter(std::move(priv)));
}

//////////////////////////////////////////////////
ModelIter ModelIterFactory::Create(REST &_rest, ClientConfig &_conf,
    const std::string &_api)
{
  std::unique_ptr<ModelIterPrivate> priv(new IterRESTIds(&_rest, &_conf, _api));
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
IterRESTIds::~IterRESTIds()
{
}

//////////////////////////////////////////////////
IterRESTIds::IterRESTIds(REST *_rest, ClientConfig *_config,
    const std::string &_api)
{
  // TODO fetch from all servers and combine result, not just one server
  auto servers = this->config->Servers();
  if (servers.empty())
  {
    this->idIter = this->ids.end();
    return;
  }

  REST::Protocol protocol = REST::GET;
  this->serverURL = servers.front().URL();
  std::string path = _api;
  std::vector<std::string> headers =  {"Accept: application/json"};

  RESTResponse resp = this->rest->Request(
      protocol, this->serverURL, path, {}, headers, "");

  if (resp.statusCode != 200)
  {
    this->idIter = this->ids.end();
    return;
  }

  this->ids = JSONParser::ParseModels(resp.data);
  this->idIter = this->ids.begin();

  // make first model
  std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
  ptr->id = *(this->idIter);
  ptr->id.SourceURL(this->serverURL);
  this->model = Model(ptr);

  igndbg << "Got response [" << resp.data << "]\n";
}

//////////////////////////////////////////////////
void IterRESTIds::Next()
{
  // advance pointer
  ++(this->idIter);

  // Update personal model class
  if (this->idIter != this->ids.end())
  {
    std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
    ptr->id = *(this->idIter);
    ptr->id.SourceURL(this->serverURL);
    this->model = Model(ptr);
  }
  // TODO request next page if api is paginated
}

//////////////////////////////////////////////////
bool IterRESTIds::HasReachedEnd()
{
  return this->ids.empty() || this->idIter == this->ids.end();
}

//////////////////////////////////////////////////
ModelIter::ModelIter(std::unique_ptr<ModelIterPrivate> _dptr)
{
  this->dataPtr.reset(_dptr.release());
}

//////////////////////////////////////////////////
ModelIter::ModelIter(ModelIter &&_old)
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
ModelIter &ModelIter::operator++()
{
  // TODO Request more data if there are more pages
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

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
#include <gz/common/Console.hh>

#include "gz/fuel_tools/Model.hh"
#include "gz/fuel_tools/ModelIter.hh"

#include "ModelIterPrivate.hh"

using namespace gz;
using namespace fuel_tools;

//////////////////////////////////////////////////
class ModelIter::Implementation
{
  public: std::shared_ptr<ModelIterPrivate> impl{nullptr};
};

namespace {
/// \brief Create a model iterator from a vector of model identifiers
/// \param[in] _ids Model identifiers
/// \return Model iterator
ModelIter Create(const std::vector<ModelIdentifier> &_ids)
{
  ModelIter ret;
  ret.dataPtr->impl = std::make_shared<IterIds>(_ids);
  return ret;
}

/// \brief Create a model iterator from a vector of models
/// \param[in] _ids Models
/// \return Model iterator
ModelIter Create(const std::vector<Model> &_models)
{
  ModelIter ret;
  ret.dataPtr->impl = std::make_shared<IterModels>(_models);
  return ret;
}

/// \brief Create a model iter that will make Rest api calls
/// \param[in] _rest a Rest request
/// \param[in] _server The server to request the operation
/// \param[in] _api The path to request
ModelIter Create(const Rest &_rest,
                 const ServerConfig &_server,
                 const std::string &_api)
{
  ModelIter ret;
  ret.dataPtr->impl = std::make_shared<IterRestIds>(_rest, _server, _api);
  return ret;
}

/// \brief Create a model iterator that is empty
/// \return An empty iterator
ModelIter Create();
}  // namespace

//////////////////////////////////////////////////
ModelIter::ModelIter()
  : dataPtr(gz::utils::MakeImpl<Implementation>())
{
}

//////////////////////////////////////////////////
ModelIter::operator bool() const
{
  return this->dataPtr->impl && !this->dataPtr->impl->HasReachedEnd();
}

//////////////////////////////////////////////////
ModelIter &ModelIter::operator++()
{
  if (this->dataPtr->impl && !this->dataPtr->impl->HasReachedEnd())
  {
    this->dataPtr->impl->Next();
  }
  return *this;
}

//////////////////////////////////////////////////
Model &ModelIter::operator*()
{
  return this->dataPtr->impl->model;
}

//////////////////////////////////////////////////
Model *ModelIter::operator->()
{
  return &(this->dataPtr->impl->model);
}

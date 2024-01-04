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

#include "gz/fuel_tools/Model.hh"
#include "gz/fuel_tools/ModelIdentifier.hh"
#include "gz/fuel_tools/Result.hh"

#include "ModelPrivate.hh"

using namespace gz;
using namespace fuel_tools;

//////////////////////////////////////////////////
Model::Model()
: dataPtr(gz::utils::MakeImpl<ModelPrivate>())
{
}

//////////////////////////////////////////////////
Model::Model(const gz::fuel_tools::ModelIdentifier &_id,
             const std::string &_pathOnDisk)
: Model()
{
  this->dataPtr->isValid = true;
  this->dataPtr->id = _id;
  this->dataPtr->pathOnDisk = _pathOnDisk;
}

//////////////////////////////////////////////////
Model::Model(const std::string &_name, const std::string &_owner,
             const gz::fuel_tools::ServerConfig &_server)
: Model()
{
  this->dataPtr->isValid = true;
  this->dataPtr->id.SetName(_name);
  this->dataPtr->id.SetOwner(_owner);
  this->dataPtr->id.SetServer(_server);
}

//////////////////////////////////////////////////
Model::operator bool()
{
  return this->dataPtr->isValid;
}

//////////////////////////////////////////////////
Model::operator bool() const
{
  return this->dataPtr->isValid;
}

//////////////////////////////////////////////////
ModelIdentifier Model::Identification() const
{
  if (this->dataPtr->isValid)
    return this->dataPtr->id;
  return {};
}

//////////////////////////////////////////////////
Result Model::Fetch() const
{
  if (this->dataPtr->isValid)
  {
    if (this->PathToModel().empty())
    {
      // TODO(nkoenig) look for models on servers
      return Result(ResultType::FETCH_ERROR);
    }
    return Result(ResultType::FETCH_ALREADY_EXISTS);
  }
  return Result(ResultType::UNKNOWN);
}

//////////////////////////////////////////////////
std::string Model::PathToModel() const
{
  if (this->dataPtr->isValid)
  {
    if (this->dataPtr->pathOnDisk.empty())
    {
      // TODO(nkoenig) check if model is in cache, store path
    }
    return this->dataPtr->pathOnDisk;
  }
  return {};
}

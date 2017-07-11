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
#include <ignition/fuel-tools/ModelIter.hh>
#include <ignition/fuel-tools/ModelIterPrivate.hh>
#include <ignition/fuel-tools/ModelPrivate.hh>

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;

//////////////////////////////////////////////////
ModelIterPrivate::ModelIterPrivate(std::vector<ModelIdentifier> _ids)
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
ModelIterPrivate::ModelIterPrivate()
{
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
  return !this->dataPtr->ids.empty()
    && this->dataPtr->idIter != this->dataPtr->ids.end();
}

//////////////////////////////////////////////////
ModelIter &ModelIter::operator++()
{
  // TODO Request more data if there are more pages
  if (this->operator bool())
  {
    // advance pointer
    this->dataPtr->idIter++;

    // Update personal model class
    if (this->dataPtr->idIter != this->dataPtr->ids.end())
    {
      std::shared_ptr<ModelPrivate> ptr(new ModelPrivate);
      ptr->id = *(this->dataPtr->idIter);
      this->dataPtr->model = Model(ptr);
    }
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

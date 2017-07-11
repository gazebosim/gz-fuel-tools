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
#include <ignition/fuel-tools/ModelIdentifier.hh>

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


class ignft::ModelIdentifierPrivate
{
  /// \brief a name given to this model by a user
  public: std::string name;

  /// \brief a unique name for this model
  public: std::string uniqueName;
};


//////////////////////////////////////////////////
ModelIdentifier::ModelIdentifier() : dataPtr(new ModelIdentifierPrivate)
{
}

//////////////////////////////////////////////////
ModelIdentifier::ModelIdentifier(const ModelIdentifier &_orig)
{
  this->dataPtr.reset(new ModelIdentifierPrivate(*(_orig.dataPtr.get())));
}

//////////////////////////////////////////////////
void ModelIdentifier::operator=(const ModelIdentifier &_orig)
{
  this->dataPtr.reset(new ModelIdentifierPrivate(*(_orig.dataPtr.get())));
}

//////////////////////////////////////////////////
ModelIdentifier::~ModelIdentifier()
{
}

//////////////////////////////////////////////////
std::string ModelIdentifier::Name() const
{
  return this->dataPtr->name;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::UniqueName() const
{
  return this->dataPtr->uniqueName;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Name(const std::string &_name)
{
  this->dataPtr->name = _name;
  // TODO Useless return value?
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::UniqueName(const std::string &_name)
{
  this->dataPtr->uniqueName = _name;
  // TODO Useless return value?
  return true;
}

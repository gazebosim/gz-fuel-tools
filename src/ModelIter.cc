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

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


//////////////////////////////////////////////////
ModelIter::ModelIter(std::unique_ptr<ModelIterPrivate> _dptr)
{
}

//////////////////////////////////////////////////
ModelIter::ModelIter(ModelIter &&_old)
{
}

//////////////////////////////////////////////////
ModelIter::~ModelIter()
{
}

//////////////////////////////////////////////////
ModelIter::operator bool()
{
  return false;
}

//////////////////////////////////////////////////
ModelIter &ModelIter::operator++()
{
  // TODO increment iterator
  return *this;
}

//////////////////////////////////////////////////
Model &ModelIter::operator*()
{
}

//////////////////////////////////////////////////
Model *ModelIter::operator->()
{
}

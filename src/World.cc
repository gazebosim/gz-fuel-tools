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

#include <string>

#include "ignition/fuel_tools/World.hh"
#include "ignition/fuel_tools/WorldIdentifier.hh"
#include "ignition/fuel_tools/WorldPrivate.hh"

using namespace ignition;
using namespace fuel_tools;

//////////////////////////////////////////////////
World::World() : dataPtr(nullptr)
{
}

//////////////////////////////////////////////////
World::World(std::shared_ptr<WorldPrivate> _dptr)
{
  this->dataPtr = _dptr;
}

//////////////////////////////////////////////////
World::operator bool() const
{
  return this->dataPtr.get() != nullptr;
}

//////////////////////////////////////////////////
WorldIdentifier World::Identification() const
{
  if (this->dataPtr)
    return this->dataPtr->id;
  return WorldIdentifier();
}

//////////////////////////////////////////////////
std::string World::PathToWorld() const
{
  if (this->dataPtr)
  {
    if (this->dataPtr->pathOnDisk.empty())
    {
      // TODO check if world is in cache, store path
    }
    return this->dataPtr->pathOnDisk;
  }
  return "";
}

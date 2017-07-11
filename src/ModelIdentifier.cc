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

  /// \brief Description of this model
  public: std::string description;

  /// \brief FileSize of this model
  public: unsigned int fileSize = 0u;

  /// \brief Number of model downloads
  public: unsigned int downloads = 0u;

  /// \brief Number of likes
  public: unsigned int likes = 0u;

  /// \brief Model upload date
  public: std::time_t uploadDate;

  /// \brief Model last modified date
  public: std::time_t modifyDate;

  /// \brief Category of this model
  public: std::string category;

  /// \brief UUID of this model
  public: std::string uuid;
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
std::string ModelIdentifier::Description() const
{
  return this->dataPtr->description;
}

//////////////////////////////////////////////////
unsigned int ModelIdentifier::FileSize() const
{
  return this->dataPtr->fileSize;
}

//////////////////////////////////////////////////
unsigned int ModelIdentifier::Downloads() const
{
  return this->dataPtr->downloads;
}

//////////////////////////////////////////////////
unsigned int ModelIdentifier::Likes() const
{
  return this->dataPtr->likes;
}

//////////////////////////////////////////////////
std::time_t ModelIdentifier::UploadDate() const
{
  return this->dataPtr->uploadDate;
}

//////////////////////////////////////////////////
std::time_t ModelIdentifier::ModifyDate() const
{
  return this->dataPtr->modifyDate;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::Category() const
{
  return this->dataPtr->category;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::Uuid() const
{
  return this->dataPtr->uuid;
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

//////////////////////////////////////////////////
bool ModelIdentifier::Description(const std::string &_desc)
{
  this->dataPtr->description = _desc;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::FileSize(const unsigned int _fileSize)
{
  this->dataPtr->fileSize = _fileSize;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Downloads(const unsigned int _downloads)
{
  this->dataPtr->downloads = _downloads;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Likes(const unsigned int _likes)
{
  this->dataPtr->likes = _likes;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::ModifyDate(const std::time_t &_date)
{
  this->dataPtr->modifyDate = _date;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::UploadDate(const std::time_t &_date)
{
  this->dataPtr->uploadDate = _date;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Category(const std::string &_cat)
{
  this->dataPtr->category = _cat;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Uuid(const std::string &_uuid)
{
  this->dataPtr->uuid = _uuid;
  return true;
}

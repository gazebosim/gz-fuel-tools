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

#include <string>
#include <vector>

#include "ignition/fuel-tools/ModelIdentifier.hh"

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


class ignft::ModelIdentifierPrivate
{
  /// \brief returns true if name follows rules
  /// \param[in] _name Name to validate
  public: bool ValidName(const std::string &_name);

  /// \brief returns true if URL follows rules
  /// \param[in] _name Name to validate
  public: bool ValidURL(const std::string &_URL);

  /// \brief a name given to this model by a user
  public: std::string name;

  /// \brief owner who this model is attributed to
  public: std::string owner;

  /// \brief source of this model
  public: std::string source;

  /// \brief Description of this model
  public: std::string description;

  /// \brief FileSize of this model
  public: unsigned int fileSize = 0u;

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
bool ModelIdentifierPrivate::ValidName(const std::string &_name)
{
  // ToDo: Enable name validation when model names are prepared for this.
  return true;

  bool valid = true;
  if (_name.empty())
    valid = false;
  else
  {
    for (const char &ch : _name)
    {
      // [-_0-9a-z]+
      if (ch == '-'
          || ch == '_'
          || (ch >= 'a' && ch <= 'z')
          || (ch >= '0' && ch <= '9'))
      {
        continue;
      }
      valid = false;
      break;
    }
  }
  return valid;
}


//////////////////////////////////////////////////
bool ModelIdentifierPrivate::ValidURL(const std::string &_name)
{
  // TODO
  return !_name.empty();
}

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
bool ModelIdentifier::operator==(const ModelIdentifier &_rhs) const
{
  return this->UniqueName() == _rhs.UniqueName();
}

//////////////////////////////////////////////////
ModelIdentifier::~ModelIdentifier()
{
}

//////////////////////////////////////////////////
std::string ModelIdentifier::UniqueName() const
{
  return this->dataPtr->source
    + '/' + this->dataPtr->owner
    + '/' + this->dataPtr->name;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::Name() const
{
  return this->dataPtr->name;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Name(const std::string &_name)
{
  bool success = false;
  if (this->dataPtr->ValidName(_name))
  {
    success = true;
    this->dataPtr->name = _name;
  }
  return success;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Owner(const std::string &_name)
{
  bool success = false;
  if (this->dataPtr->ValidName(_name))
  {
    success = true;
    this->dataPtr->owner = _name;
  }
  return success;
}

//////////////////////////////////////////////////
bool ModelIdentifier::SourceURL(const std::string &_url_orig)
{
  std::string url(_url_orig);
  bool success = false;
  // Strip trailing slashes
  while(!url.empty() && url.back() == '/')
  {
    url.pop_back();
  }
  if (this->dataPtr->ValidURL(url))
  {
    success = true;
    this->dataPtr->source = url;
  }
  return success;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::Owner() const
{
  return this->dataPtr->owner;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::SourceURL() const
{
  return this->dataPtr->source;
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


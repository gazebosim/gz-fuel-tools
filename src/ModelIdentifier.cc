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

#include <sstream>
#include <string>
#include <vector>

#include <ignition/common/Filesystem.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/ModelIdentifier.hh"

using namespace ignition;
using namespace fuel_tools;

class ignition::fuel_tools::ModelIdentifierPrivate
{
  /// \brief returns true if name follows rules
  /// \param[in] _name Name to validate
  public: bool ValidName(const std::string &_name);

  /// \brief a name given to this model by a user
  public: std::string name;

  /// \brief owner who this model is attributed to
  public: std::string owner;

  /// \brief Server of this model
  public: ServerConfig server;

  /// \brief Description of this model
  public: std::string description;

  /// \brief FileSize of this model
  public: unsigned int fileSize{0u};

  /// \brief Model upload date
  public: std::time_t uploadDate{0};

  /// \brief Model last modified date
  public: std::time_t modifyDate{0};

  /// \brief Category of this model
  public: std::string category;

  /// \brief UUID of this model
  public: std::string uuid;

  /// \brief Number of "likes"
  public: uint32_t likes{0u};

  /// \brief Number of downloads
  public: uint32_t downloads{0u};

  /// \brief The license name
  public: std::string licenseName;

  /// \brief License URL
  public: common::URI licenseUrl;

  /// \brief License image URL
  public: common::URI licenseImageUrl;

  /// \brief Collection of tags
  public: std::vector<std::string> tags;

  /// \brief Model version. Valid versions start from 1, 0 means the tip.
  public: unsigned int version{0};
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
ModelIdentifier::ModelIdentifier()
  : dataPtr(new ModelIdentifierPrivate)
{
}

//////////////////////////////////////////////////
ModelIdentifier::ModelIdentifier(const ModelIdentifier &_orig)
{
  this->dataPtr.reset(new ModelIdentifierPrivate(*(_orig.dataPtr.get())));
}

//////////////////////////////////////////////////
ModelIdentifier &ModelIdentifier::operator=(const ModelIdentifier &_orig)
{
  this->dataPtr.reset(new ModelIdentifierPrivate(*(_orig.dataPtr.get())));
  return *this;
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
  return this->dataPtr->server.URL()     + "/"        +
         this->dataPtr->owner            + "/models/" +
         this->dataPtr->name;
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
bool ModelIdentifier::Server(const ServerConfig &_server)
{
  bool success = common::URI::Valid(_server.URL());
  if (success)
    this->dataPtr->server = _server;

  return success;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::Owner() const
{
  return this->dataPtr->owner;
}

//////////////////////////////////////////////////
ServerConfig &ModelIdentifier::Server() const
{
  return this->dataPtr->server;
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
uint32_t ModelIdentifier::Likes() const
{
  return this->dataPtr->likes;
}

//////////////////////////////////////////////////
uint32_t ModelIdentifier::Downloads() const
{
  return this->dataPtr->downloads;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::LicenseName() const
{
  return this->dataPtr->licenseName;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::LicenseURL() const
{
  return this->dataPtr->licenseUrl.Str();
}

//////////////////////////////////////////////////
std::string ModelIdentifier::LicenseImageURL() const
{
  return this->dataPtr->licenseImageUrl.Str();
}

//////////////////////////////////////////////////
std::vector<std::string> ModelIdentifier::Tags() const
{
  return this->dataPtr->tags;
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

//////////////////////////////////////////////////
bool ModelIdentifier::Likes(const uint32_t _likes)
{
  this->dataPtr->likes = _likes;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Downloads(const uint32_t _downloads)
{
  this->dataPtr->downloads = _downloads;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::LicenseName(const std::string &_name)
{
  this->dataPtr->licenseName = _name;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::LicenseURL(const std::string &_url)
{
  return this->dataPtr->licenseUrl.Parse(_url);
}

//////////////////////////////////////////////////
bool ModelIdentifier::LicenseImageURL(const std::string &_url)
{
  return this->dataPtr->licenseImageUrl.Parse(_url);
}

//////////////////////////////////////////////////
bool ModelIdentifier::Tags(const std::vector<std::string> &_tags)
{
  this->dataPtr->tags = _tags;
  return true;
}

//////////////////////////////////////////////////
unsigned int ModelIdentifier::Version() const
{
  return this->dataPtr->version;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::VersionStr() const
{
  std::string version = this->dataPtr->version == 0 ?
      "tip" : std::to_string(this->dataPtr->version);
  return version;
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetVersion(const unsigned int _version)
{
  this->dataPtr->version = _version;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetVersionStr(const std::string &_version)
{
  if (_version == "tip")
  {
    this->dataPtr->version = 0;
    return true;
  }

  try
  {
    this->dataPtr->version = std::stoi(_version);
  }
  catch(std::invalid_argument &_e)
  {
    return false;
  }

  return true;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::AsString(const std::string &_prefix) const
{
  std::stringstream out;
  out << _prefix << "Name: " << this->Name() << std::endl
      << _prefix << "Owner: " << this->Owner() << std::endl
      << _prefix << "Version: " << std::to_string(this->Version()) << std::endl
      << _prefix << "Unique name: " << this->UniqueName() << std::endl
      << _prefix << "Description: " << this->Description() << std::endl
      << _prefix << "File size: " << this->FileSize() << std::endl
      << _prefix << "Upload date: " << this->UploadDate() << std::endl
      << _prefix << "UUID: " << this->Uuid() << std::endl
      << _prefix << "Likes: " << this->Likes() << std::endl
      << _prefix << "Downloads: " << this->Downloads() << std::endl
      << _prefix << "License name: " << this->LicenseName() << std::endl
      << _prefix << "License URL: " << this->LicenseURL() << std::endl
      << _prefix << "License image URL: " << this->LicenseImageURL()
      << std::endl
      << _prefix << "Tags: " << std::endl;

  for (auto t : this->Tags())
    out << _prefix << "- " << t << std::endl;

  out << _prefix << "Server:" << std::endl
      << this->Server().AsString(_prefix + "  ");
  return out.str();
}

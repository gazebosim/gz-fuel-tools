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

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/ModelIdentifier.hh"

using namespace ignition;
using namespace fuel_tools;

class ignition::fuel_tools::ModelIdentifierPrivate
{
  /// \brief returns true if name follows rules
  /// \param[in] _name Name to validate
  public: bool ValidName(const std::string &_name);

  /// \brief returns true if URL follows rules
  /// \param[in] _name Name to validate
  /// \return True if the name is a valid url
  public: bool ValidUrl(const std::string &_url);

  /// \brief a name given to this model by a user
  public: std::string name;

  /// \brief owner who this model is attributed to
  public: std::string owner;

  /// \brief Server of this model
  public: ServerConfig server;

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

  /// \brief Number of "likes"
  public: uint32_t likeCount;

  /// \brief Number of downloads
  public: uint32_t downloads;

  /// \brief The license name
  public: std::string licenseName;

  /// \brief License URL
  public: std::string licenseUrl;

  /// \brief License image URL
  public: std::string licenseImageUrl;

  /// \brief Collection of tags
  public: std::vector<std::string> tags;
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
bool ModelIdentifierPrivate::ValidUrl(const std::string &_url)
{
  // TODO
  return !_url.empty();
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
  return this->dataPtr->server.Url()     + "/"        +
         this->dataPtr->server.Version() + "/"        +
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
  return this->SetName(_name);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetName(const std::string &_name)
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
  return this->SetOwner(_name);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetOwner(const std::string &_name)
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
  return this->SetServer(_server);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetServer(const ServerConfig &_server)
{
  bool success = this->dataPtr->ValidUrl(_server.Url());
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
  return this->LikeCount();
}

//////////////////////////////////////////////////
uint32_t ModelIdentifier::LikeCount() const
{
  return this->dataPtr->likeCount;
}

//////////////////////////////////////////////////
uint32_t ModelIdentifier::Downloads() const
{
  return this->DownloadCount();
}

//////////////////////////////////////////////////
uint32_t ModelIdentifier::DownloadCount() const
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
  return this->LicenseUrl();
}

//////////////////////////////////////////////////
std::string ModelIdentifier::LicenseUrl() const
{
  return this->dataPtr->licenseUrl;
}

//////////////////////////////////////////////////
std::string ModelIdentifier::LicenseImageURL() const
{
  return this->LicenseImageUrl();
}

//////////////////////////////////////////////////
std::string ModelIdentifier::LicenseImageUrl() const
{
  return this->dataPtr->licenseImageUrl;
}

//////////////////////////////////////////////////
std::vector<std::string> ModelIdentifier::Tags() const
{
  return this->dataPtr->tags;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Description(const std::string &_desc)
{
  return this->SetDescription(_desc);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetDescription(const std::string &_desc)
{
  this->dataPtr->description = _desc;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::FileSize(const unsigned int _fileSize)
{
  return this->SetFileSize(_fileSize);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetFileSize(const unsigned int _fileSize)
{
  this->dataPtr->fileSize = _fileSize;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::ModifyDate(const std::time_t &_date)
{
  return this->SetModifyDate(_date);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetModifyDate(const std::time_t &_date)
{
  this->dataPtr->modifyDate = _date;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::UploadDate(const std::time_t &_date)
{
  return this->SetUploadDate(_date);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetUploadDate(const std::time_t &_date)
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
  return this->SetUuid(_uuid);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetUuid(const std::string &_uuid)
{
  this->dataPtr->uuid = _uuid;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Likes(const uint32_t _likes)
{
  return this->SetLikeCount(_likes);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetLikeCount(const uint32_t _likes)
{
  this->dataPtr->likeCount = _likes;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Downloads(const uint32_t _downloads)
{
  return this->SetDownloadCount(_downloads);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetDownloadCount(const uint32_t _downloads)
{
  this->dataPtr->downloads = _downloads;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::LicenseName(const std::string &_name)
{
  return this->SetLicenseName(_name);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetLicenseName(const std::string &_name)
{
  this->dataPtr->licenseName = _name;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::LicenseURL(const std::string &_url)
{
  return this->SetLicenseUrl(_url);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetLicenseUrl(const std::string &_url)
{
  this->dataPtr->licenseUrl = _url;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::LicenseImageURL(const std::string &_url)
{
  return this->SetLicenseImageUrl(_url);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetLicenseImageUrl(const std::string &_url)
{
  this->dataPtr->licenseImageUrl = _url;
  return true;
}

//////////////////////////////////////////////////
bool ModelIdentifier::Tags(const std::vector<std::string> &_tags)
{
  return this->SetTags(_tags);
}

//////////////////////////////////////////////////
bool ModelIdentifier::SetTags(const std::vector<std::string> &_tags)
{
  this->dataPtr->tags = _tags;
  return true;
}

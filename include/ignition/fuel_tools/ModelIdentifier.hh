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

#ifndef IGNITION_FUEL_TOOLS_MODELIDENTIFIER_HH_
#define IGNITION_FUEL_TOOLS_MODELIDENTIFIER_HH_

// #include <array>
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "ignition/fuel_tools/Helpers.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Forward Declaration
    class ModelIdentifierPrivate;
    class ServerConfig;

    /// \brief Defines how to identify a model
    class IGNITION_FUEL_TOOLS_VISIBLE ModelIdentifier
    {
      /// \brief Constructor.
      public: ModelIdentifier();

      /// \brief Copy Constructor.
      /// \param[in] _orig ModelIdentifier to copy.
      public: ModelIdentifier(const ModelIdentifier &_orig);

      /// \brief Destructor.
      public: ~ModelIdentifier();

      /// \brief Assignment operator
      /// \param[in] _orig ModelIdentifier to copy.
      /// \return Reference to this object.
      public: ModelIdentifier &operator=(const ModelIdentifier &_orig);

      /// \brief Equality operator.
      /// \param[in] _rhs ModelIdentifier to compare.
      /// \return True if the ModelIdentifier names are equal.
      public: bool operator==(const ModelIdentifier &_rhs) const;

      /// \brief Returns just the last part of the model name.
      /// \return Model name.
      public: std::string Name() const;

      /// \brief Returns owner to attribute model to.
      /// \return Owner name.
      public: std::string Owner() const;

      /// \brief Returns server information to retrieve model from.
      /// \return Server information.
      public: ServerConfig &Server() const;

      /// \brief Returns a unique name for the model.
      /// \remarks this is Server/Owner/Name.
      /// \return Unique model name.
      public: std::string UniqueName() const;

      /// \brief set the name of the model.
      /// \param[in] _name The name to set. Must be ascii and pass [-_a-z0-9]+.
      /// \return true if successful.
      /// \deprecated See bool SetName(const std::string &_name).
      public: bool IGN_DEPRECATED(2.0) Name(const std::string &_name);

      /// \brief set the name of the model.
      /// \param[in] _name The name to set. Must be ascii and pass [-_a-z0-9]+.
      /// \return true if successful.
      public: bool SetName(const std::string &_name);

      /// \brief Set the owner of the model
      /// \param[in] _name The name to set. Must be ascii and pass [-_a-z0-9]+
      /// \return true if successful
      /// \deprecated See bool SetOwner(const std::string &_name).
      public: bool IGN_DEPRECATED(2.0) Owner(const std::string &_name);

      /// \brief Set the owner of the model
      /// \param[in] _name The name to set. Must be ascii and pass [-_a-z0-9]+
      /// \return true if successful
      public: bool SetOwner(const std::string &_name);

      /// \brief Set the server from which this model comes
      /// \param[in] _server The server to retrieve the model from
      /// \return true if successful
      /// \deprecated See bool SetServer(const ServerConfig &_server)
      public: bool IGN_DEPRECATED(2.0) Server(const ServerConfig &_server);

      /// \brief Set the server from which this model comes.
      /// \param[in] _server The server to retrieve the model from.
      /// \return True if successful.
      public: bool SetServer(const ServerConfig &_server);

      /// \brief Returns the description of the model.
      /// \return Model description.
      public: std::string Description() const;

      /// \brief Returns the file size of the model.
      /// \return Model file size.
      public: unsigned int FileSize() const;

      /// \brief Returns the upload date of the model.
      /// \return Model upload date.
      public: std::time_t UploadDate() const;

      /// \brief Returns the modify date of the model.
      /// \return Model modify date.
      public: std::time_t ModifyDate() const;

      /// \brief Returns the category of the model.
      /// \return Model category.
      public: std::string Category() const;

      /// \brief Returns the uuid of the model.
      /// \return Model uuid.
      public: std::string Uuid() const;

      /// \brief Returns the number of "likes" for the model.
      /// \return Number of "likes".
      public: uint32_t Likes() const;

      /// \brief Returns the number of downloads for the model.
      /// \return Number of downloads.
      public: uint32_t Downloads() const;

      /// \brief Returns the license name.
      /// \return License name.
      public: std::string LicenseName() const;

      /// \brief Returns the license URL.
      /// \return License URL.
      /// \deprecated See std::string LicenseUrl() const
      public: std::string IGN_DEPRECATED(2.0) LicenseURL() const;

      /// \brief Returns the license URL.
      /// \return License URL.
      public: std::string LicenseUrl() const;

      /// \brief Returns the license image URL.
      /// \return License image URL.
      /// \deprecated See std::string LicenseImageUrl() const
      public: std::string IGN_DEPRECATED(2.0) LicenseImageURL() const;

      /// \brief Returns the license image URL.
      /// \return License image URL.
      public: std::string LicenseImageUrl() const;

      /// \brief Returns the collection of tags.
      /// \return The collection of tags.
      public: std::vector<std::string> Tags() const;

      /// \brief Set the description of the model.
      /// \return True if successful.
      /// \deprecated See bool SetDescription(const std::string &_desc)
      public: bool IGN_DEPRECATED(2.0) Description(const std::string &_desc);

      /// \brief Set the description of the model.
      /// \param[in] _desc The description
      /// \return True if successful.
      public: bool SetDescription(const std::string &_desc);

      /// \brief Set the file size of the model.
      /// \return True if successful.
      /// \deprecated See bool SetFileSize(const unsigned int _filesize)
      public: bool IGN_DEPRECATED(2.0) FileSize(const unsigned int _filesize);

      /// \brief Set the file size of the model.
      /// \param[in] _filesize The model's file size.
      /// \return True if successful.
      public: bool SetFileSize(const unsigned int _filesize);

      /// \brief Set the upload date
      /// \return True if successful
      /// \deprecated See bool SetUploadDate(const std::time_t &_date)
      public: bool IGN_DEPRECATED(2.0) UploadDate(const std::time_t &_date);

      /// \brief Set the upload date.
      /// \param[in] _date Upload date.
      /// \return True if successful.
      public: bool SetUploadDate(const std::time_t &_date);

      /// \brief Set the modify date
      /// \return True if successful
      /// \deprecated See bool SetModifyDate(const std::time_t &_date);
      public: bool IGN_DEPRECATED(2.0) ModifyDate(const std::time_t &_date);

      /// \brief Set the modify date
      /// \return True if successful
      public: bool SetModifyDate(const std::time_t &_date);

      /// \brief Set the category of the model.
      /// \return True if successful.
      /// \deprecated See bool SetCategory(const std::string &_cat)
      public: bool IGN_DEPRECATED(2.0) Category(const std::string &_cat);

      /// \brief Set the category of the model.
      /// \param[in] _cat The model's category.
      /// \return True if successful.
      public: bool SetCategory(const std::string &_cat);

      /// \brief Set the uuid of the model.
      /// \return True if successful.
      /// \deprecated See bool SetUuid(const std::string &_uuid)
      public: bool IGN_DEPRECATED(2.0) Uuid(const std::string &_uuid);

      /// \brief Set the uuid of the model.
      /// \param[in] _uuid The model's uuid.
      /// \return True if successful.
      public: bool SetUuid(const std::string &_uuid);

      /// \brief Set the number of "likes" of the model.
      /// \param[in] _likes Number of "likes".
      /// \return True if successful.
      /// \deprecated See bool  SetLikes(const uint32_t _likes).
      public: bool IGN_DEPRECATED(2.0) Likes(const uint32_t _likes);

      /// \brief Set the number of "likes" of the model.
      /// \param[in] _likes Number of "likes".
      /// \return True if successful.
      public: bool SetLikes(const uint32_t _likes);

      /// \brief Set the number of downloads of the model.
      /// \param[in] _downloads Number of downloads.
      /// \return True if successful.
      /// \deprecated See bool SetDownloads(const uint32_t _downloads).
      public: bool IGN_DEPRECATED(2.0) Downloads(const uint32_t _downloads);

      /// \brief Set the number of downloads of the model.
      /// \param[in] _downloads Number of downloads.
      /// \return True if successful.
      public: bool SetDownloads(const uint32_t _downloads);

      /// \brief Set the license name of the model.
      /// \param[in] _name The name.
      /// \return True if successful.
      /// \deprecated See bool SetLicenseName(const std::string &_name).
      public: bool IGN_DEPRECATED(2.0) LicenseName(const std::string &_name);

      /// \brief Set the license name of the model.
      /// \param[in] _name The name.
      /// \return True if successful.
      public: bool SetLicenseName(const std::string &_name);

      /// \brief Set the license URL of the model.
      /// \param[in] _url The URL.
      /// \return True if successful.
      /// \deprecated See bool SetLicenseUrl(const std::string &_url).
      public: bool IGN_DEPRECATED(2.0) LicenseURL(const std::string &_url);

      /// \brief Set the license URL of the model.
      /// \param[in] _url The URL.
      /// \return True if successful.
      public: bool SetLicenseUrl(const std::string &_url);

      /// \brief Set the license image URL of the model.
      /// \param[in] _url The URL.
      /// \return True if successful.
      /// \deprecated bool SetLicenseImageUrl(const std::string &_url).
      public: bool IGN_DEPRECATED(2.0) LicenseImageURL(const std::string &_url);

      /// \brief Set the license image URL of the model.
      /// \param[in] _url The URL.
      /// \return True if successful.
      public: bool SetLicenseImageUrl(const std::string &_url);

      /// \brief Set the list of tags of the model.
      /// \param[in] _tags The tags.
      /// \return True if successful.
      /// \deprecated See bool SetTags(const std::vector<std::string> &_tags).
      public: bool IGN_DEPRECATED(2.0) Tags(
                  const std::vector<std::string> &_tags);

      /// \brief Set the list of tags of the model.
      /// \param[in] _tags The tags.
      /// \return True if successful.
      public: bool SetTags(const std::vector<std::string> &_tags);

      // /// \brief returns a SHA 2 256 hash of the model
      // /// \remarks fulfills versioning requirement
      // public: std::array<std::uint8_t, 32> SHA_256() const;

      // /// \brief Sets the SHA 2 256 hash of the model
      // /// \param[in] _hash a 256 bit SHA 2 hash
      // /// \returns true if successful
      // public: bool SHA_256(const std::array<std::uint8_t, 32> &_hash);

      /// \brief PIMPL
      private: std::unique_ptr<ModelIdentifierPrivate> dataPtr;
    };
  }
}

#endif

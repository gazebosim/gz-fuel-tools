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

#ifndef IGNITION_FUEL_TOOLS_CLIENTCONFIG_HH_
#define IGNITION_FUEL_TOOLS_CLIENTCONFIG_HH_

#include <memory>
#include <string>
#include <vector>

#include "ignition/fuel_tools/Export.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief forward declaration
    class ServerConfigPrivate;

    /// \brief Forward Declaration
    class ClientConfigPrivate;

    /// \brief Describes options needed for a server.
    class IGNITION_FUEL_TOOLS_VISIBLE ServerConfig
    {
      /// \brief Constructor.
      public: ServerConfig();

      /// \brief Copy constructor.
      /// \param[in] _orig The server config to copy.
      public: ServerConfig(const ServerConfig &_orig);

      /// \brief Assignment operator overload.
      /// \param[in] _orig The server config to copy.
      public: ServerConfig &operator=(const ServerConfig &_orig);

      /// \brief Destructor.
      public: ~ServerConfig();

      /// \brief Get the URL to access the server.
      /// \deperecated See std::string Url() const.
      public: std::string IGN_DEPRECATED(2.0) URL() const;

      /// \brief Get the URL to access the server.
      /// \return The URL of this server.
      public: std::string Url() const;

      /// \brief Set the URL of this server.
      /// \deprecated See void SetUrl(const std::string &).
      public: void IGN_DEPRECATED(2.0) URL(const std::string &_url);

      /// \brief Set the URL of this server.
      /// \param[in] _url URL of this server.
      public: void SetUrl(const std::string &_url);

      /// \brief Get folder name for server on disk.
      /// \return The local folder name.
      public: std::string LocalName() const;

      /// \brief Set folder name for server on disk.
      /// \param[in] _name Folder name.
      /// \deprecated See void SetLocalName(const std::string &).
      public: void IGN_DEPRECATED(2.0) LocalName(const std::string &_name);

      /// \brief Set folder name for server on disk.
      /// \param[in] _name Folder name.
      public: void SetLocalName(const std::string &_name);

      /// \brief Get the API key to auth with the server.
      /// \deprecated See std::string ApiKey() const.
      public: std::string IGN_DEPRECATED(2.0) APIKey() const;

      /// \brief Get the API key to auth with the server.
      /// \return The API key.
      public: std::string ApiKey() const;

      /// \brief Set the API key to auth with the server.
      /// \deprecated See void SetApiKey(const std::string &_key).
      public: void IGN_DEPRECATED(2.0) APIKey(const std::string &_key);

      /// \brief Set the API key to auth with the server.
      /// \param[in] _key The API key.
      public: void SetApiKey(const std::string &_key);

      /// \brief Get the protocol version used with this server.
      /// \return The version. E.g.: "1.0".
      public: std::string Version() const;

      /// \brief Set the protocol version used with this server.
      /// \param[in] _version The version. E.g.: "1.0".
      /// \deprecated See void SetVersion(const std::string &_version).
      public: void IGN_DEPRECATED(2.0) Version(const std::string &_version);

      /// \brief Set the protocol version used with this server.
      /// \param[in] _version The version. E.g.: "1.0".
      public: void SetVersion(const std::string &_version);

      /// \brief PIMPL
      private: std::unique_ptr<ServerConfigPrivate> dataPtr;
    };

    /// \brief High level interface to ignition fuel.
    ///
    class IGNITION_FUEL_TOOLS_VISIBLE ClientConfig
    {
      /// \brief Constructor.
      public: ClientConfig();

      /// \brief Copy constructor.
      /// \param[in] _copy ClientConfig to copy.
      public: ClientConfig(const ClientConfig &_copy);

      /// \brief Assignment operator overload.
      /// \param[in] _copy ClientConfig to copy.
      public: ClientConfig &operator=(const ClientConfig &_copy);

      /// \brief Destructor.
      public: ~ClientConfig();

      /// \brief Load the YAML configuration file.
      /// If SetConfigPath() is not used, this function will try to load the
      /// default configuration path.
      /// \return True if the configuration was loaded correctly.
      /// \sa SetConfigPath
      public: bool LoadConfig();

      /// \brief Set the location of the configuration file.
      /// \param[in] _path Path to the configuration file.
      /// \sa LoadConfig
      public: void SetConfigPath(const std::string &_path);

      /// \brief List of servers the client will connect to.
      /// \return The list of servers.
      public: std::vector<ServerConfig> Servers() const;

      /// \brief Add a server to the list.
      /// \param[in] _srv The server config.
      public: void AddServer(const ServerConfig &_srv);

      /// \brief Where are models and stuff stored locally?
      /// \return The location where assets are stored locally.
      public: std::string CacheLocation() const;

      /// \brief Set where models and stuff are saved.
      /// \param[in] _path path on disk where models are saved
      /// \deprecated See void CacheLocation(const std::string &_path)
      public: void IGN_DEPRECATED(2.0) CacheLocation(const std::string &_path);

      /// \brief Set where models and stuff are saved.
      /// \param[in] _path path on disk where models are saved.
      public: void SetCacheLocation(const std::string &_path);

      /// \brief PIMPL
      private: std::unique_ptr<ClientConfigPrivate> dataPtr;
    };
  }
}

#endif

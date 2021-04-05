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

#include <ignition/common/URI.hh>

#include "ignition/fuel_tools/Export.hh"

#ifdef _WIN32
// Disable warning C4251 which is triggered by
// std::unique_ptr
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

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

      /// \brief Clear the server config. This will set all values to empty
      /// strings, except the version string which will be set to its default
      /// value.
      public: void Clear();

      /// \brief Get the URL to access the server.
      /// \return The URL of this server.
      public: common::URI Url() const;

      /// \brief Set the URL of this server.
      /// \param[in] _url URL of this server.
      public: void SetUrl(const common::URI &_url);

      /// \brief Get the API key to auth with the server.
      /// \return The API key.
      public: std::string ApiKey() const;

      /// \brief Set the API key to auth with the server.
      /// \param[in] _key The API key.
      public: void SetApiKey(const std::string &_key);

      /// \brief Get the protocol version used with this server.
      /// \return The version. E.g.: "1.0".
      public: std::string Version() const;

      /// \brief Set the protocol version used with this server.
      /// \param[in] _version The version. E.g.: "1.0".
      public: void SetVersion(const std::string &_version);

      /// \brief Returns all the server information as a string.
      /// \param[in] _prefix Optional prefix for every line of the string.
      /// \return Server information string
      public: std::string AsString(const std::string &_prefix = "") const;

      /// \brief Returns all the available model information as a string using
      /// colors for better human parsing.
      /// \param[in] _prefix Optional prefix for every line of the string.
      /// \return Model information string
      public: std::string AsPrettyString(const std::string &_prefix = "") const;

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

      /// \brief Clear the client config. This will set all values to empty
      /// strings, except the user agent which will be set to its default
      /// value.
      public: void Clear();

      /// \brief Set the user agent name.
      /// \param[in] _agent User agent name.
      public: void SetUserAgent(const std::string &_agent);

      /// \brief Get the user agent name.
      /// \return Name of the user agent.
      public: const std::string &UserAgent() const;

      /// \brief Load a YAML configuration file.
      /// \param[in] _file Config file to load.
      /// \return True if the configuration was loaded correctly.
      /// \sa ConfigPath
      public: bool LoadConfig(const std::string &_file);

      /// \brief Get the location of the configuration file.
      /// \return Path to the configuration file, which is set via
      /// LoadConfig. The default return value is an empty string.
      /// \sa LoadConfig
      public: std::string ConfigPath() const;

      /// \brief List of servers the client will connect to.
      /// \return The list of servers.
      public: std::vector<ServerConfig> Servers() const;

      /// \brief List of servers the client will connect to.
      /// \return The list of servers.
      public: std::vector<ServerConfig> & MutableServers() const;

      /// \brief Add a server to the list.
      /// \param[in] _srv The server config.
      public: void AddServer(const ServerConfig &_srv);

      /// \brief Where are models and stuff stored locally?
      /// \return The location where assets are stored locally.
      public: std::string CacheLocation() const;

      /// \brief Set where models and stuff are saved.
      /// \param[in] _path path on disk where models are saved.
      public: void SetCacheLocation(const std::string &_path);

      /// \brief Returns all the client information as a string.
      /// \param[in] _prefix Optional prefix for every line of the string.
      /// \return Client information string
      public: std::string AsString(const std::string &_prefix = "") const;

      /// \brief PIMPL
      private: std::unique_ptr<ClientConfigPrivate> dataPtr;
    };
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

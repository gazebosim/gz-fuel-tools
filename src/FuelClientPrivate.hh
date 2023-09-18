/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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

#include <memory>
#include <regex>
#include <string>

#include "gz/fuel_tools/FuelClient.hh"

namespace gz::fuel_tools
{

/// \brief Private Implementation
class FuelClient::Implementation
{
  /// \brief A model URL,
  /// E.g.: https://fuel.gazebosim.org/1.0/caguero/models/Beer/2
  /// Where the API version and the model version are optional.
  public: const std::string kModelUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "models"
    "models\\/+"
    // Name
    "([^\\/]+)\\/*"
    // Version
    "([0-9]*|tip)"
    // Trailing /
    "/?"};

  /// \brief A world URL,
  /// E.g.: https://fuel.gazebosim.org/1.0/OpenRobotics/worlds/Empty/1
  /// Where the API version and the world version are optional.
  public: const std::string kWorldUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "worlds"
    "worlds\\/+"
    // Name
    "([^\\/]+)\\/*"
    // Version
    "([0-9]*|tip)"
    // Trailing /
    "/?"};

  /// \brief A model file URL,
  /// E.g.: https://server.org/1.0/owner/models/modelname/files/meshes/mesh.dae
  /// Where the API version is optional, but the model version is required.
  public: const std::string kModelFileUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "models"
    "models\\/+"
    // Model
    "([^\\/]+)\\/+"
    // Version
    "([0-9]*|tip)\\/+"
    // "files"
    "files\\/+"
    // File path
    "(.*)"
    // Trailing /
    "/?"};

  /// \brief A world file URL,
  /// E.g.: https://server.org/1.0/owner/worlds/worldname/files/meshes/mesh.dae
  /// Where the API version is optional, but the world version is required.
  public: const std::string kWorldFileUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "worlds"
    "worlds\\/+"
    // World
    "([^\\/]+)\\/+"
    // Version
    "([0-9]*|tip)\\/+"
    // "files"
    "files\\/+"
    // File path
    "(.*)"
    // Trailing /
    "/?"};

  /// \brief A collection URL,
  /// E.g.:
  /// https://fuel.gazebosim.org/1.0/OpenRobotics/collections/TestColl
  /// Where the API version is optional
  public: const std::string kCollectionUrlRegexStr{
    // Method
    "^([[:alnum:]\\.\\+\\-]+):\\/\\/"
    // Server
    "([^\\/\\s]+)\\/+"
    // API Version
    "([0-9]+[.][0-9]+)?\\/*"
    // Owner
    "([^\\/\\s]+)\\/+"
    // "collections"
    "collections\\/+"
    // Name
    "([^\\/]+)\\/*"};

  /// \brief Recursively get all the files in the given path.
  /// \param[in] _path Path to process.
  /// \param[out] _files All the files in the given _path.
  public: void AllFiles(const std::string &_path,
              std::vector<std::string> &_files) const;

  /// \brief Populate a model form such that it can be used during
  /// model creation and editing REST calls.
  /// \param[in] _pathToModelDir Path to the model directory.
  /// \param[in] _id Model identifier information.
  /// \param[in] _private True if this model should be private.
  /// \param[in] _owner Model owner name.
  /// \param[out] _form Form to fill.
  /// \return True if the operation completed successfully.
  public: bool FillModelForm(const std::string &_pathToModelDir,
              const ModelIdentifier &_id, bool _private,
              const std::string &_owner,
              std::multimap<std::string, std::string> &_form);

  /// \brief This function requests the available licenses from the
  ///  Fuel server and stores this information locally.
  ///
  /// The UploadModel function can use this information to set
  /// appropriate license information based on a model's metadata.pbtxt
  /// file. If license information is not available, then the
  /// UploadModel function will default to the
  /// "Creative Commons - Public Domain" license.
  /// \param[in] _server Information about the server that provides
  /// license information.
  public: void PopulateLicenses(const ServerConfig &_server);

  /// \brief Checks the provided URI for fuel.gazebosim.org, and
  /// prints a deprecation warning message if found.
  /// \param[in] _uri URI to check
  /// DEPRECATED/DEPRECATION: remove this function in Gazebo H.
  public: void CheckForDeprecatedUri(const common::URI &_uri);

  /// \brief Get zip data from a REST response. This is used by world and
  /// model download.
  public: void ZipFromResponse(const RestResponse &_resp,
              std::string &_zip);

  /// \brief Client configuration
  public: ClientConfig config;

  /// \brief RESTful client
  public: Rest rest;

  /// \brief Local Cache
  public: std::shared_ptr<LocalCache> cache;

  /// \brief Regex to parse Gazebo Fuel model URLs.
  public: std::unique_ptr<std::regex> urlModelRegex;

  /// \brief Regex to parse Gazebo Fuel world URLs.
  public: std::unique_ptr<std::regex> urlWorldRegex;

  /// \brief Regex to parse Gazebo Fuel model file URLs.
  public: std::unique_ptr<std::regex> urlModelFileRegex;

  /// \brief Regex to parse Gazebo Fuel world file URLs.
  public: std::unique_ptr<std::regex> urlWorldFileRegex;

  /// \brief Regex to parse Gazebo Fuel Collection URLs.
  public: std::unique_ptr<std::regex> urlCollectionRegex;

  /// \brief The set of licenses where the key is the name of the license
  /// and the value is the license ID on a Fuel server. See the
  /// PopulateLicenses function.
  public: std::map<std::string, unsigned int> licenses;
};
}  // namespace gz::fuel_tools

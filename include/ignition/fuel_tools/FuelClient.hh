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

#ifndef IGNITION_FUEL_TOOLS_FUELCLIENT_HH_
#define IGNITION_FUEL_TOOLS_FUELCLIENT_HH_

#include <memory>
#include <string>
#include <vector>
#include <ignition/common/URI.hh>

#include "ignition/fuel_tools/Helpers.hh"
#include "ignition/fuel_tools/ModelIter.hh"
#include "ignition/fuel_tools/RestClient.hh"
#include "ignition/fuel_tools/Result.hh"
#include "ignition/fuel_tools/WorldIter.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief Forward Declaration
    class ClientConfig;
    class FuelClientPrivate;
    class LocalCache;
    class ModelIdentifier;
    class ServerConfig;

    /// \brief High level interface to ignition fuel
    class IGNITION_FUEL_TOOLS_VISIBLE FuelClient
    {
      /// \brief Default constructor.
      public: FuelClient();

      /// \brief Constructor accepts server and auth configuration
      /// \param[in] _config configuration about servers to connect to
      /// \param[in] _rest A REST request.
      /// \param[in] _cache Test hook. Pointer to a local cache. The FuelClient
      ///            will take ownership of the pointer and free it when
      ///            destructed. If set to nullptr the client will instantiate
      ///            it's own cache.
      /// \remarks the client saves a copy of the config passed into it
      public: FuelClient(const ClientConfig &_config,
                         const Rest &_rest = Rest(),
                         LocalCache *_cache = nullptr);

      /// \brief Destructor
      public: ~FuelClient();

      /// \brief Get a mutable reference to the client configuration.
      /// \return Mutable reference to the client configuration.
      public: ClientConfig &Config();

      /// \brief Fetch the details of a model.
      /// \param[in] _id a partially filled out identifier used to fetch models
      /// \remarks Fulfills Get-One requirement
      /// \param[out] _model The requested model
      /// \return Result of the fetch operation.
      public: Result ModelDetails(const ModelIdentifier &_id,
                                  ModelIdentifier &_model) const;

      /// \brief Returns an iterator that can return names of models
      /// \remarks Fulfills Get-All requirement
      /// \remarks an iterator instead of a list of names is returned in case
      ///          the model names api supports pagination in the future. The
      ///          iterator may fetch more names if code continues to request
      ///          it. The initial API appears to return all of the models, so
      ///          right now this iterator stores a list of names internally.
      /// \param[in] _server The server to request the operation.
      /// \return A model iterator
      public: ModelIter Models(const ServerConfig &_server);

      /// \brief Returns an iterator that can return names of models
      /// \remarks Fulfills Get-All requirement
      /// \remarks an iterator instead of a list of names is returned in case
      ///          the model names api supports pagination in the future. The
      ///          iterator may fetch more names if code continues to request
      ///          it. The initial API appears to return all of the models, so
      ///          right now this iterator stores a list of names internally.
      /// \param[in] _server The server to request the operation.
      /// \return A model iterator
      public: ModelIter Models(const ServerConfig &_server) const;

      /// \brief Fetch the details of a world.
      /// \param[in] _id a partially filled out identifier used to fetch worlds
      /// \param[out] _world The requested world
      /// \return Result of the fetch operation.
      public: Result WorldDetails(const WorldIdentifier &_id,
                                  WorldIdentifier &_world) const;

      /// \brief Returns an iterator that can return information of worlds
      /// \remarks An iterator instead of a list of names, to be able to
      ///          handle pagination. The iterator may fetch more names if
      ///          code continues to request it.
      /// \param[in] _server The server to request the operation.
      /// \return A world iterator
      public: WorldIter Worlds(const ServerConfig &_server) const;

      /// \brief Returns models matching a given identifying criteria
      /// \param[in] _id a partially filled out identifier used to fetch models
      /// \remarks Fulfills Get-One requirement
      /// \remarks It's not yet clear if model names are unique, so this API
      ///          allows the posibility of getting multiple models with the
      ///          same name.
      /// \return An iterator of models with names matching the criteria
      public: ModelIter Models(const ModelIdentifier &_id);

      /// \brief Returns models matching a given identifying criteria
      /// \param[in] _id a partially filled out identifier used to fetch models
      /// \remarks Fulfills Get-One requirement
      /// \remarks It's not yet clear if model names are unique, so this API
      ///          allows the posibility of getting multiple models with the
      ///          same name.
      /// \return An iterator of models with names matching the criteria
      public: ModelIter Models(const ModelIdentifier &_id) const;

      /// \brief Returns worlds matching a given identifying criteria
      /// \param[in] _id A partially filled out identifier used to fetch worlds
      /// \return An iterator of worlds with names matching the criteria
      public: WorldIter Worlds(const WorldIdentifier &_id) const;

      /// \brief Upload a directory as a new model
      /// \param[in] _pathToModelDir a path to a directory containing a model
      /// \param[in] _id An identifier to assign to this new model
      /// \return Result of the upload operation
      public: Result UploadModel(const std::string &_pathToModelDir,
                                 const ModelIdentifier &_id);

      /// \brief Remove a model from ignition fuel
      /// \param[in] _id The model identifier.
      /// \return Result of the delete operation
      public: Result DeleteModel(const ModelIdentifier &_id);

      /// \brief Download a model from ignition fuel. This will override an
      /// existing local copy of the model.
      /// \param[in] _id The model identifier.
      /// \return Result of the download operation
      public: Result DownloadModel(const ModelIdentifier &_id);

      /// \brief Download a model from ignition fuel. This will override an
      /// existing local copy of the model.
      /// \param[in] _id The model identifier.
      /// \param[in] _headers Headers to set on the HTTP request.
      /// \return Result of the download operation
      public: Result DownloadModel(const ModelIdentifier &_id,
                  const std::vector<std::string> &_headers);

      /// \brief Download a world from Ignition Fuel. This will override an
      /// existing local copy of the world.
      /// \param[out] _id The world identifier, with local path updated.
      /// \return Result of the download operation
      public: Result DownloadWorld(WorldIdentifier &_id);

      /// \brief Download a model from ignition fuel. This will override an
      /// existing local copy of the model.
      /// \param[in] _modelUrl The unique URL of the model to download.
      /// E.g.: https://fuel.ignitionrobotics.org/1.0/caguero/models/Beer
      /// \param[out] _path Path where the model was downloaded.
      /// \return Result of the download operation.
      public: Result DownloadModel(const common::URI &_modelUrl,
                                   std::string &_path);

      /// \brief Download a world from ignition fuel. This will override an
      /// existing local copy of the world.
      /// \param[in] _worldUrl The unique URL of the world to download.
      /// E.g.: https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty
      /// \param[out] _path Path where the world was downloaded.
      /// \return Result of the download operation.
      public: Result DownloadWorld(const common::URI &_worldUrl,
                                   std::string &_path);

      /// \brief Check if a model is already present in the local cache.
      /// \param[in] _modelUrl The unique URL of the model on a Fuel server.
      /// E.g.: https://fuel.ignitionrobotics.org/1.0/caguero/models/Beer
      /// \param[out] _path Local path where the model can be found.
      /// \return FETCH_ERROR if not cached, FETCH_ALREADY_EXISTS if cached.
      public: Result CachedModel(const common::URI &_modelUrl,
                                 std::string &_path);

      /// \brief Check if a model exists in the cache.
      /// \param[in] _modelUrl The unique URL of the world on a Fuel server.
      /// E.g.: https://fuel.ignitionrobotics.org/1.0/caguero/models/Beer
      /// \return True if the model exists in the cache, false otherwise.
      public: bool CachedModel(const common::URI &_modelUrl);

      /// \brief Check if a world is already present in the local cache.
      /// \param[in] _worldUrl The unique URL of the world on a Fuel server.
      /// E.g.: https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty
      /// \param[out] _path Local path where the world can be found.
      /// \return FETCH_ERROR if not cached, FETCH_ALREADY_EXISTS if cached.
      public: Result CachedWorld(const common::URI &_worldUrl,
                                 std::string &_path);

      /// \brief Check if a world exists in the cache.
      /// \param[in] _worldUrl The unique URL of the world on a Fuel server.
      /// E.g.: https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty
      /// \return True if the world exists in the cache, false otherwise.
      public: bool CachedWorld(const common::URI &_worldUrl);

      /// \brief Check if a file belonging to a model is already present in the
      /// local cache.
      /// \param[in] _fileUrl The unique URL of the file on a Fuel server. E.g.:
      /// https://server.org/1.0/owner/models/model/files/meshes/mesh.dae
      /// \param[out] _path Local path where the file can be found.
      /// \return FETCH_ERROR if not cached, FETCH_ALREADY_EXISTS if cached.
      public: Result CachedModelFile(const common::URI &_fileUrl,
                                     std::string &_path);

      /// \brief Check if a file belonging to a world is already present in the
      /// local cache.
      /// \param[in] _fileUrl The unique URL of the file on a Fuel server. E.g.:
      /// https://server.org/1.0/owner/worlds/world/files/name.world
      /// \param[out] _path Local path where the file can be found.
      /// \return FETCH_ERROR if not cached, FETCH_ALREADY_EXISTS if cached.
      public: Result CachedWorldFile(const common::URI &_fileUrl,
                                     std::string &_path);

      /// \brief Parse model identifer from model URL or unique name.
      /// \param[in] _modelUrl The unique URL of a model. It may also be a
      /// unique name, which is a URL without the server version.
      /// \param[out] _id The model identifier. It may contain incomplete
      /// information based on the passed URL and the current client
      /// config.
      /// The server version will be overridden if that server is in the config
      /// file.
      /// \return True if parsed successfully.
      public: bool ParseModelUrl(const common::URI &_modelUrl,
                                 ModelIdentifier &_id);

      /// \brief Parse world identifer from world URL or unique name.
      /// \param[in] _worldUrl The unique URL of a world. It may also be a
      /// unique name, which is a URL without the server version.
      /// \param[out] _id The world identifier. It may contain incomplete
      /// information based on the passed URL and the current client
      /// config.
      /// The server version will be overridden if that server is in the config
      /// file.
      /// \return True if parsed successfully.
      public: bool ParseWorldUrl(const common::URI &_worldUrl,
                                 WorldIdentifier &_id);

      /// \brief Parse model file identifer from model file URL.
      /// \param[in] _modelFileUrl The unique URL of a model file. It may also
      /// be a unique name, which is a URL without the server version.
      /// \param[out] _id The model identifier. It may contain incomplete
      /// information based on the passed URL and the current client
      /// config.
      /// \param[out] _filePath Path to the file from the model's root
      /// directory, such as "meshes/mesh.dae" or "model.sdf".
      /// \return True if parsed successfully.
      public: bool ParseModelFileUrl(const common::URI &_modelFileUrl,
                                     ModelIdentifier &_id,
                                     std::string &_filePath);

      /// \brief Parse world file identifer from world file URL.
      /// \param[in] _worldFileUrl The unique URL of a world file. It may also
      /// be a unique name, which is a URL without the server version.
      /// \param[out] _id The world identifier. It may contain incomplete
      /// information based on the passed URL and the current client
      /// config.
      /// \param[out] _filePath Path to the file from the world's root
      /// directory, such as "name.world"
      /// \return True if parsed successfully.
      public: bool ParseWorldFileUrl(const common::URI &_worldFileUrl,
                                     WorldIdentifier &_id,
                                     std::string &_filePath);

      /// \brief PIMPL
      private: std::unique_ptr<FuelClientPrivate> dataPtr;
    };
  }
}

#endif

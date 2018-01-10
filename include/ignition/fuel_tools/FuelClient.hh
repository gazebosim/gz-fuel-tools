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

#include "ignition/fuel_tools/Helpers.hh"
#include "ignition/fuel_tools/ModelIter.hh"
#include "ignition/fuel_tools/REST.hh"
#include "ignition/fuel_tools/Result.hh"

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
      /// \param[in] _rest a REST request.
      /// \param[in] _cache Test hook. Pointer to a local cache. The FuelClient
      ///            will take ownership of the pointer and free it when
      ///            destructed. If set to nullptr the client will instantiate
      ///            it's own cache.
      /// \remarks the client saves a copy of the config passed into it
      public: FuelClient(const ClientConfig &_config,
                         const REST &_rest = REST(),
                         LocalCache *_cache = nullptr);

      /// \brief Destructor
      public: ~FuelClient();

      /// \brief Get a mutable reference to the client configuration.
      /// \return Mutable reference to the client configuration.
      public: ClientConfig &Config();

      /// \brief Fetch the details of a model.
      /// \param[in] _server The server to request the operation.
      /// \param[in] _id a partially filled out identifier used to fetch models
      /// \remarks Fulfills Get-One requirement
      /// \param[out] _model The requested model
      /// \return Result of the fetch operation.
      public: Result ModelDetails(const ServerConfig &_server,
                                  const ModelIdentifier &_id,
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

      /// \brief Returns models matching a given identifying criteria
      /// \param[in] _server The server to request the operation.
      /// \param[in] _id a partially filled out identifier used to fetch models
      /// \remarks Fulfills Get-One requirement
      /// \remarks It's not yet clear if model names are unique, so this API
      ///          allows the posibility of getting multiple models with the
      ///          same name.
      /// \return An iterator of models with names matching the criteria
      public: ModelIter Models(const ServerConfig &_server,
                               const ModelIdentifier &_id);

      /// \brief Upload a directory as a new model
      /// \param[in] _server The server to request the operation.
      /// \param[in] _pathToModelDir a path to a directory containing a model
      /// \param[in] _id An identifier to assign to this new model
      /// \return Result of the upload operation
      public: Result UploadModel(const ServerConfig &_server,
                                 const std::string &_pathToModelDir,
                                 const ModelIdentifier &_id);

      /// \brief Remove a model from ignition fuel
      /// \param[in] _server The server to request the operation.
      /// \param[in] _id The model identifier.
      /// \return Result of the delete operation
      public: Result DeleteModel(const ServerConfig &_server,
                                 const ModelIdentifier &_id);

      /// \brief Download a model from ignition fuel
      /// \param[in] _server The server to request the operation.
      /// \param[in] _id The model identifier.
      /// \return Result of the download operation
      public: Result DownloadModel(const ServerConfig &_server,
                                   const ModelIdentifier &_id);

      /// \brief Download a model from ignition fuel
      /// \param[in] _modelURL The unique URL of the model to download
      /// E.g.: https://api.ignitionfuel.org/1.0/caguero/models/Beer
      /// \param[out] _path Path where the model was downloaded
      /// \return Result of the download operation
      public: Result DownloadModel(const std::string &_modelURL,
                                   std::string &_path);

      /// \brief PIMPL
      private: std::unique_ptr<FuelClientPrivate> dataPtr;
    };
  }
}

#endif

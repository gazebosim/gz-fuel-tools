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

#ifndef IGNITION_FUEL_TOOLS_MODELITERPRIVATE_HH_
#define IGNITION_FUEL_TOOLS_MODELITERPRIVATE_HH_

#include <string>
#include <vector>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/Model.hh"
#include "ignition/fuel_tools/ModelIdentifier.hh"
#include "ignition/fuel_tools/REST.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief forward declaration
    class ModelIter;

    /// \brief Private class, do not include or instantiate
    class IGNITION_FUEL_TOOLS_VISIBLE ModelIterFactory
    {
      /// \brief Create a model iterator from a vector of model identifiers
      /// \param[in] _ids Model identifiers
      public: static ModelIter Create(const std::vector<ModelIdentifier> &_ids);

      /// \brief Create a model iterator from a vector of models
      /// \param[in] _ids Models
      public: static ModelIter Create(const std::vector<Model> &_models);

      /// \brief Create a model iter that will make REST api calls
      /// \param[in] _rest a REST request
      /// \param[in] _server The server to request the operation
      /// \param[in] _api The path to request
      public: static ModelIter Create(const REST &_rest,
                                      const ServerConfig &_server,
                                      const std::string &_api);

      /// \brief Create a model iterator that is empty
      public: static ModelIter Create();
    };

    /// \brief Private class, do not include or instantiate
    class IGNITION_FUEL_TOOLS_VISIBLE ModelIterPrivate
    {
      /// \brief destructor
      public: virtual ~ModelIterPrivate();

      /// \brief Advance iterator to next model
      public: virtual void Next() = 0;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() = 0;

      /// \brief current model for returning references
      public: Model model;
    };

    /// \brief class for iterating through model ids where all are known
    ///        in advance
    class IGNITION_FUEL_TOOLS_VISIBLE IterIds : public ModelIterPrivate
    {
      /// \brief constructor
      public: explicit IterIds(std::vector<ModelIdentifier> _ids);

      /// \brief destructor
      public: virtual ~IterIds();

      /// \brief Advance iterator to next model
      public: virtual void Next() override;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() override;

      /// \brief Model identifiers that have been requested
      protected: std::vector<ModelIdentifier> ids;

      /// \brief where the current iterator is in the list of ids
      protected: std::vector<ModelIdentifier>::iterator idIter;
    };

    /// \brief class for iterating through model ids where all are known
    ///        in advance
    class IGNITION_FUEL_TOOLS_VISIBLE IterModels: public ModelIterPrivate
    {
      /// \brief constructor
      public: explicit IterModels(std::vector<Model> _models);

      /// \brief destructor
      public: virtual ~IterModels();

      /// \brief Advance iterator to next model
      public: virtual void Next() override;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() override;

      /// \brief Models to iterator through
      protected: std::vector<Model> models;

      /// \brief where the current iterator is in the list of models
      protected: std::vector<Model>::iterator modelIter;
    };


    /// \brief class for iterating through model ids from a rest API
    class IGNITION_FUEL_TOOLS_VISIBLE IterRESTIds: public ModelIterPrivate
    {
      /// \brief constructor
      public: IterRESTIds(const REST &_rest,
                          const ServerConfig &_server,
                          const std::string &_api);

      /// \brief destructor
      public: virtual ~IterRESTIds();

      /// \brief Advance iterator to next model
      public: virtual void Next() override;

      /// \brief true if this iterator has reach the end
      public: virtual bool HasReachedEnd() override;

      /// \brief Client configuration
      public: ServerConfig config;

      /// \brief RESTful client
      public: REST rest;

      /// \brief Model identifiers in the current page
      protected: std::vector<ModelIdentifier> ids;

      /// \brief where the current iterator is in the list of ids
      protected: std::vector<ModelIdentifier>::iterator idIter;
    };
  }
}

#endif

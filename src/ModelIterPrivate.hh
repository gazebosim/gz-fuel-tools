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
#include "ignition/fuel_tools/RestClient.hh"

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
      /// \return Model iterator
      public: static ModelIter Create(const std::vector<ModelIdentifier> &_ids);

      /// \brief Create a model iterator from a vector of models
      /// \param[in] _ids Models
      /// \return Model iterator
      public: static ModelIter Create(const std::vector<Model> &_models);

      /// \brief Create a model iter that will make Rest api calls
      /// \param[in] _rest a Rest request
      /// \param[in] _server The server to request the operation
      /// \param[in] _api The path to request
      public: static ModelIter Create(const Rest &_rest,
                                      const ServerConfig &_server,
                                      const std::string &_api);

      /// \brief Create a model iterator that is empty
      /// \return An empty iterator
      public: static ModelIter Create();
    };

    /// \brief Private class, do not include or instantiate
    class IGNITION_FUEL_TOOLS_VISIBLE ModelIterPrivate
    {
      /// \brief Destructor
      public: virtual ~ModelIterPrivate();

      /// \brief Advance iterator to next model
      public: virtual void Next() = 0;

      /// \brief True if this iterator has reach the end
      /// \return True if reached end.
      public: virtual bool HasReachedEnd() = 0;

      /// \brief Current model for returning references
      public: Model model;
    };

    /// \brief class for iterating through model ids where all are known
    ///        in advance
    class IGNITION_FUEL_TOOLS_VISIBLE IterIds : public ModelIterPrivate
    {
      /// \brief Constructor
      public: explicit IterIds(std::vector<ModelIdentifier> _ids);

      /// \brief Destructor
      public: virtual ~IterIds();

      // Documentation inherited
      public: virtual void Next() override;

      // Documentation inherited
      public: virtual bool HasReachedEnd() override;

      /// \brief Model identifiers that have been requested
      protected: std::vector<ModelIdentifier> ids;

      /// \brief Where the current iterator is in the list of ids
      protected: std::vector<ModelIdentifier>::iterator idIter;
    };

    /// \brief class for iterating through model ids where all are known
    ///        in advance
    class IGNITION_FUEL_TOOLS_VISIBLE IterModels: public ModelIterPrivate
    {
      /// \brief Constructor
      public: explicit IterModels(std::vector<Model> _models);

      /// \brief Destructor
      public: virtual ~IterModels();

      // Documentation inherited
      public: virtual void Next() override;

      // Documentation inherited
      public: virtual bool HasReachedEnd() override;

      /// \brief Models to iterator through
      protected: std::vector<Model> models;

      /// \brief Where the current iterator is in the list of models
      protected: std::vector<Model>::iterator modelIter;
    };

    /// \brief class for iterating through model ids from a rest API
    class IGNITION_FUEL_TOOLS_VISIBLE IterRestIds: public ModelIterPrivate
    {
      /// \brief constructor
      public: IterRestIds(const Rest &_rest,
                          const ServerConfig &_server,
                          const std::string &_api);

      /// \brief destructor
      public: virtual ~IterRestIds();

      /// \brief Advance iterator to next model.
      public: virtual void Next() override;

      /// \brief True if this iterator has reach the end.
      /// \return True if iterator has reached the end.
      public: virtual bool HasReachedEnd() override;

      /// \brief Client configuration
      public: ServerConfig config;

      /// \brief RESTful client
      public: Rest rest;

      /// \brief Model identifiers in the current page
      protected: std::vector<ModelIdentifier> ids;

      /// \brief Where the current iterator is in the list of ids
      protected: std::vector<ModelIdentifier>::iterator idIter;
    };
  }
}

#endif

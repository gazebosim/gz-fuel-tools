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

#ifndef GZ_FUEL_TOOLS_MODELITERPRIVATE_HH_
#define GZ_FUEL_TOOLS_MODELITERPRIVATE_HH_

#include <string>
#include <vector>

#include "gz/fuel_tools/Model.hh"
#include "gz/fuel_tools/ModelIdentifier.hh"
#include "gz/fuel_tools/RestClient.hh"
#include "gz/fuel_tools/ServerConfig.hh"


namespace gz
{
  namespace fuel_tools
  {
    /// \brief Private class, do not include or instantiate
    class ModelIterPrivate
    {
      /// \brief Destructor
      public: virtual ~ModelIterPrivate();

      /// \brief Advance iterator to next model
      public: virtual void Next() = 0;

      /// \brief True if this iterator has reach the end
      /// \return True if reached end.
      public: virtual bool HasReachedEnd() const = 0;

      /// \brief Current model for returning references
      public: Model model;
    };

    /// \brief class for iterating through model ids where all are known
    ///        in advance
    class IterIds : public ModelIterPrivate
    {
      /// \brief Constructor
      public: explicit IterIds(const std::vector<ModelIdentifier> &_ids);

      /// \brief Destructor
      public: ~IterIds() override;

      // Documentation inherited
      public: void Next() override;

      // Documentation inherited
      public: bool HasReachedEnd() const override;

      /// \brief Model identifiers that have been requested
      protected: std::vector<ModelIdentifier> ids;

      /// \brief Where the current iterator is in the list of ids
      protected: std::vector<ModelIdentifier>::iterator idIter;
    };

    /// \brief class for iterating through model ids where all are known
    ///        in advance
    class IterModels: public ModelIterPrivate
    {
      /// \brief Constructor
      public: explicit IterModels(const std::vector<Model> &_models);

      /// \brief Destructor
      public: ~IterModels() override;

      // Documentation inherited
      public: void Next() override;

      // Documentation inherited
      public: bool HasReachedEnd() const override;

      /// \brief Models to iterator through
      protected: std::vector<Model> models;

      /// \brief Where the current iterator is in the list of models
      protected: std::vector<Model>::iterator modelIter;
    };

    /// \brief class for iterating through model ids from a rest API
    class IterRestIds: public ModelIterPrivate
    {
      /// \brief constructor
      public: IterRestIds(const Rest &_rest,
                          const ServerConfig &_server,
                          const std::string &_api);

      /// \brief destructor
      public: ~IterRestIds() override;

      /// \brief Advance iterator to next model.
      public: void Next() override;

      /// \brief True if this iterator has reach the end.
      /// \return True if iterator has reached the end.
      public: bool HasReachedEnd() const override;

      /// \brief Client configuration
      public: ServerConfig config;

      /// \brief RESTful client
      public: Rest rest;

      /// \brief The API (path) of the RESTful requests
      public: const std::string api;

      /// \brief Make a RESTful request for the given page
      /// \param[in] _page Page number to request
      /// \return Response from the request
      protected: RestResponse MakeRestRequest(std::size_t _page);

      /// \brief Parse model identifiers from a RESTful response
      /// \param[in] _resp RESTful response
      /// \return A vector of identifiers extracted from the response.
      protected: std::vector<ModelIdentifier> ParseIdsFromResponse(
          const RestResponse &_resp);

      /// \brief Model identifiers in the current page
      protected: std::vector<ModelIdentifier> ids;

      /// \brief Where the current iterator is in the list of ids
      protected: std::vector<ModelIdentifier>::iterator idIter;

      /// \brief Keep track of page number for pagination of response data from
      /// server.
      protected: std::size_t currentPage{0};
    };
  }
}

#endif

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

#ifndef IGNITION_FUEL_TOOLS_JSONPARSER_HH_
#define IGNITION_FUEL_TOOLS_JSONPARSER_HH_

#include <string>
#include <vector>

#include "ignition/fuel_tools/ModelIdentifier.hh"
#include "ignition/fuel_tools/ModelIter.hh"

namespace Json
{
  class Value;
}

namespace ignition
{
  namespace fuel_tools
  {
    class ServerConfig;

    /// \brief A helper class for making REST requests.
    class IGNITION_FUEL_TOOLS_VISIBLE JSONParser
    {
      /// \brief Parse a model JSON string and return a model identifier
      /// \param[in] _json JSON string containing a model.
      /// \param[in] _server The server sending the JSON.
      /// This information is populated into some fields of the model.
      /// \return a model identifier
      public: static ModelIdentifier ParseModel(
                  const std::string &_json,
                  const ServerConfig &_server);

      /// \brief Parse a model array JSON string and return a model iterator
      /// \param[in] _json JSON string containing an array of models
      /// \param[in] _server The server sending the JSON.
      /// This information is populated into some fields of the models.
      /// \return vector of model identifiers
      public: static std::vector<ModelIdentifier> ParseModels(
                  const std::string &_json,
                  const ServerConfig &_server);

      /// \brief Parse a json object as a model.
      /// \param[in] _json JSON object containing a single model
      /// \param[out] _model a model identifier after parsing the JSON
      /// \return True if the parsing succeed or false otherwise
      private: static bool ParseModelImpl(
                  const Json::Value &_json, ModelIdentifier &_model);

      /// \brief Parse the list of tags contained in a model.
      /// \param[in] _json JSON representation of the model.
      /// \return The list of tags.
      private: static std::vector<std::string> ParseTags(
                  const Json::Value &_json);

      /// \brief Build a model iterator from a JSON string
      /// \param[in] _modelIt A model iterator containing only one model
      /// \return A JSON string representing a single model
      public: static std::string BuildModel(ModelIter _modelIt);
    };
  }
}

#endif

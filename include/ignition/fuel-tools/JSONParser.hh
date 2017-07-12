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

#include "ignition/fuel-tools/ModelIter.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief A helper class for making REST requests.
    class IGNITION_FUEL_TOOLS_VISIBLE JSONParser
    {
      /// \brief Parse a model array JSON string and return a model iterator
      /// \param[in] _json JSON string containing an array of models
      /// \return Model iterator
      public: static ModelIter ParseModels(const std::string &_json);

      /// \brief Build a model iterator from a JSON string
      /// \param[in] _modelIt A model iterator containing only one model
      /// \return A JSON string representing a single model
      public: static std::string BuildModel(ModelIter _modelIt);
    };
  }
}

#endif

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

#ifndef IGNITION_FUEL_TOOLS_MODELITER_HH_
#define IGNITION_FUEL_TOOLS_MODELITER_HH_

#include <memory>

#include "ignition/fuel_tools/Helpers.hh"
#include "ignition/fuel_tools/Model.hh"

namespace ignition
{
  namespace fuel_tools
  {
    /// \brief forward declaration
    class FuelClient;
    class ModelIterPrivate;
    class ModelIterFactory;

    /// \brief class for iterating through models
    class IGNITION_FUEL_TOOLS_VISIBLE ModelIter
    {
      friend ModelIterFactory;

      /// \brief Construct an iterator with the data it needs to function
      protected: explicit ModelIter(std::unique_ptr<ModelIterPrivate> _dptr);

      /// \brief Move constructor
      public: ModelIter(ModelIter && _old);

      /// \brief Default destructor.
      public: ~ModelIter();

      /// \return false once the iterator is one past the end of the models
      public: operator bool();

      /// \return false once the iterator is one past the end of the models
      public: operator bool() const;

      /// \brief Prefix increment
      public: ModelIter &operator++();

      /// \brief Dereference operator
      public: Model &operator*();

      /// \brief -> operator
      public: Model *operator->();

      /// \brief PIMPL
      private: std::unique_ptr<ModelIterPrivate> dataPtr;
    };
  }
}

#endif

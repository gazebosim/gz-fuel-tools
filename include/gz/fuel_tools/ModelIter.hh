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

#ifndef GZ_FUEL_TOOLS_MODELITER_HH_
#define GZ_FUEL_TOOLS_MODELITER_HH_

#include <memory>

#include "gz/fuel_tools/Export.hh"
#include "gz/fuel_tools/Model.hh"

namespace gz
{
  namespace fuel_tools
  {
    /// \brief forward declaration
    class ModelIterFactory;

    /// \brief class for iterating through models
    class GZ_FUEL_TOOLS_VISIBLE ModelIter
    {
      friend ModelIterFactory;

      protected: ModelIter();

      /// \brief Conversion operator
      /// \return false once the iterator is one past the end of the models
      public: explicit operator bool() const;

      /// \brief Prefix increment
      /// \return Next iteration
      public: ModelIter &operator++();

      /// \brief Dereference operator
      /// \return Reference
      public: Model &operator*();

      /// \brief -> operator
      /// \return Internal world identifier
      public: Model *operator->();

      /// \brief Private data pointer.
      GZ_UTILS_IMPL_PTR(dataPtr)
    };
  }
}

#endif

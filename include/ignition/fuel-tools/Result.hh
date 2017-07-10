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

#ifndef IGNITION_FUEL_TOOLS_RESULT_HH_
#define IGNITION_FUEL_TOOLS_RESULT_HH_

#include <memory>


namespace ignition
{
  namespace fuel_tools
  {
    /// \brief base class describing a result of an operation
    class Result
    {
      /// \brief operator bool returns true if operation was successful
      virtual operator bool() const = 0;

      /// \brief Get human readable result string
      virtual std::string ReadableResult() const = 0;
    };
  }
}

#endif


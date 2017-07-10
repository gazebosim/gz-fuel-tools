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

#ifndef IGNITION_FUEL_TOOLS_DELETERESULT_HH_
#define IGNITION_FUEL_TOOLS_DELETERESULT_HH_

#include <memory>
#include <ignition/fuel-tools/Result.hh>


namespace ignition
{
  namespace fuel_tools
  {
    /// \brief forward declaration
    class DeleteResultPrivate;

    /// \brief forward declaration
    class Model;

    /// \brief Class describing result of a model delete
    class DeleteResult : public Result
    {
      friend Model;

      /// \brief protected constructor
      protected: DeleteResult(std::unique_ptr<DeleteResultPrivate> _dptr);

      /// \brief destructor
      public: ~DeleteResult();

      /// \brief operator bool
      /// \returns true if the model is on disk and ready to use
      virtual operator bool() const override;

      /// \brief Get human readable error string
      virtual std::string ReadableResult() const override;

      private: std::unique_ptr<DeleteResultPrivate> dataPtr;
    };
  }
}

#endif

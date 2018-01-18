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
#include <string>

#include "ignition/fuel_tools/Helpers.hh"

namespace ignition
{
  namespace fuel_tools
  {
    // forward declaration
    class ResultPrivate;
    class Model;

    /// \brief Class describing a result of an operation
    class IGNITION_FUEL_TOOLS_VISIBLE Result
    {
      friend Model;

      /// Result type
      public: enum ResultType
      {
        /// \brief Uninitialized type
        UNKNOWN = 0,

        /// \brief Delete successful.
        DELETE,

        /// \brief Model not found.
        DELETE_NOT_FOUND,

        /// \brief Delete failed. Other errors.
        /// \sa ReadableResult
        DELETE_ERROR,

        /// \brief Fetch successful.
        FETCH,

        /// \brief Model already exists.
        FETCH_ALREADY_EXISTS,

        /// \brief Model not found.
        FETCH_NOT_FOUND,

        /// \brief Fetch failed. Other errors.
        /// \sa ReadableResult
        FETCH_ERROR,

        /// \brief Upload successful.
        UPLOAD,

        /// \brief Model already exists.
        UPLOAD_ALREADY_EXISTS,

        /// \brief Upload failed. Other errors.
        /// \sa ReadableResult
        UPLOAD_ERROR
      };

      /// \brief Destructor
      public: ~Result();

      /// \brief Get the type of result
      public: ResultType Type() const;

      /// \brief protected constructor
      public: explicit Result(const ResultType _type);

      /// \brief copy constructor
      public: Result(const Result &_orig);

      /// \brief operator bool returns true if operation was successful
      public: virtual operator bool() const;

      /// \brief Get human readable result string
      public: virtual std::string ReadableResult() const;

      /// brief Pointer to private data
      private: std::unique_ptr<ResultPrivate> dataPtr;
    };
  }
}

#endif

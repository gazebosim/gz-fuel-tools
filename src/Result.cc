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

#include <vector>
#include <ignition/fuel-tools/Result.hh>

namespace ignft = ignition::fuel_tools;
using namespace ignition;
using namespace ignft;


class ignft::ResultPrivate
{
  /// \brief a type of result
  public: Result::ResultType type = Result::UNKNOWN;
};


//////////////////////////////////////////////////
Result::~Result()
{
}

//////////////////////////////////////////////////
Result::ResultType Result::Type() const
{
  return this->dataPtr->type;
}

//////////////////////////////////////////////////
Result::Result(const Result::ResultType _type) : dataPtr(new ResultPrivate)
{
  this->dataPtr->type = _type;
}

//////////////////////////////////////////////////
Result::operator bool() const
{
  switch(this->dataPtr->type)
  {
    case DELETE:
    case FETCH:
    case FETCH_ALREADY_EXISTS:
    case UPLOAD:
      return true;
    default:
      return false;
  }
}

//////////////////////////////////////////////////
std::string Result::ReadableResult() const
{
  switch (this->dataPtr->type)
  {
    case DELETE:
      return "Successfully deleted";
    case FETCH:
      return "Successfully fetched from server";
    case FETCH_ALREADY_EXISTS:
      return "Already in cache, did not fetch from server";
    case UPLOAD:
      return "Successfully uploaded to server";
    case DELETE_NOT_FOUND:
      return "Could not delete, model not found";
    case DELETE_ERROR:
      return "Delete failed. Other errors";
    case FETCH_NOT_FOUND:
        return "Model not found";
    case FETCH_ERROR:
        return "Fetch failed. Other errors";
    case UPLOAD_ALREADY_EXISTS:
        return "Model already exists";
    case UPLOAD_ERROR:
        return "Upload failed. Other errors";
    case UNKNOWN:
    default:
      return "Unknown result";
  }
}

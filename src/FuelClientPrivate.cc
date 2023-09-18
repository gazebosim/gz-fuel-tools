/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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

#include "FuelClientPrivate.hh"

namespace gz::fuel_tools
{

//////////////////////////////////////////////////
void FuelClient::Implementation::AllFiles(const std::string &_path,
    std::vector<std::string> &_files) const
{
  gz::common::DirIter dirIter(_path);
  gz::common::DirIter end;
  while (dirIter != end)
  {
    if (gz::common::isDirectory(*dirIter))
      this->AllFiles(*dirIter, _files);
    else
      _files.push_back(*dirIter);

    ++dirIter;
  }
}

//////////////////////////////////////////////////
bool FuelClient::Implementation::FillModelForm(const std::string &_pathToModelDir,
    const ModelIdentifier &_id, bool _private, const std::string &_owner,
    std::multimap<std::string, std::string> &_form)
{
  if (!common::exists(_pathToModelDir))
  {
    gzerr << "The model path[" << _pathToModelDir << "] doesn't exist.\n";
    return false;
  }

  gz::msgs::FuelMetadata meta;

  // Try the `metadata.pbtxt` file first since it contains more information
  // than `model.config`.
  if (common::exists(common::joinPaths(_pathToModelDir, "metadata.pbtxt")))
  {
    std::string filePath = common::joinPaths(_pathToModelDir, "metadata.pbtxt");

    gzdbg << "Parsing " << filePath  << std::endl;

    // Read the pbtxt file.
    std::ifstream inputFile(filePath);
    std::string inputStr((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());

    // Parse the file into the fuel metadata message
    google::protobuf::TextFormat::ParseFromString(inputStr, &meta);
  }
  else if (common::exists(common::joinPaths(_pathToModelDir, "model.config")))
  {
    std::string filePath = common::joinPaths(_pathToModelDir, "model.config");

    gzdbg << "Parsing " << filePath << std::endl;

    std::ifstream inputFile(filePath);
    std::string inputStr((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());

    if (!gz::msgs::ConvertFuelMetadata(inputStr, meta))
    {
      gzerr << "Unable to convert model config[" << _pathToModelDir << "].\n";
      return false;
    }
  }
  else
  {
    gzerr << "Provided model directory[" <<  _pathToModelDir
      << "] needs a metadata.pbtxt or a model.confg file.";
    return false;
  }

  _form =
  {
    {"name", meta.name()},
    {"description", meta.description()},
    {"private", _private ? "1" : "0"},
  };

  // Add owner if specified.
  if (!_owner.empty())
  {
    _form.emplace("owner", _owner);
  }

  // \todo(nkoenig) The gz-fuelserver expects an integer number for the
  // license information. The fuelserver should be modified to accept
  // a string. Otherwise, we have to bake into each client a mapping of
  // license name to integer.
  //
  // If we have legal, then attempt to fill in the correct license information.
  if (meta.has_legal())
  {
    // Attempt to retrieve the available licenses, if we have no available
    // licenses.
    if (this->licenses.empty())
    {
      this->PopulateLicenses(_id.Server());
      // Fail if a license has been requested, but we couldn't get the
      // available licenses.
      if (this->licenses.empty())
      {
        return false;
      }
    }

    // Find the license by name.
    std::map<std::string, unsigned int>::const_iterator licenseIt =
      this->licenses.find(meta.legal().license());
    if (licenseIt != this->licenses.end())
    {
      _form.emplace("license", std::to_string(licenseIt->second));
    }
    // No license found, print an error and return.
    else
    {
      std::string validLicenseNames;
      auto end = this->licenses.end();
      std::advance(end, -1);
      for (licenseIt = this->licenses.begin(); licenseIt != end; ++licenseIt)
      {
        validLicenseNames += "    " + licenseIt->first + "\n";
      }
      validLicenseNames += "    " + licenseIt->first;

      gzerr << "Invalid license[" << meta.legal().license() << "].\n"
             << "  Valid licenses include:\n"
             << validLicenseNames << std::endl;

      return false;
    }
  }
  // If there is no license information, then default to
  // "Creative Commons - Public Domain"
  else
  {
    _form.emplace("license", "1");
  }

  // Add tags
  std::string tags;
  for (int i = 0; i < meta.tags_size(); ++i)
    tags += meta.tags(i) + ",";
  if (!tags.empty())
    _form.emplace("tags", tags);

  // Add categories
  std::string categories;
  if (meta.has_categories())
  {
    // Add the first category, if present.
    if (!meta.categories().first().empty())
      categories = meta.categories().first();

    // Add the second category, if present.
    if (!meta.categories().second().empty())
    {
      // Add a comma separator if the first category was not empty.
      if (!categories.empty())
        categories += ", ";
      categories += meta.categories().second();
    }
  }
  if (!categories.empty())
    _form.emplace("categories", categories);

  // Add annotations as metadata.
  for (const auto &annotation : meta.annotations())
  {
    std::string formAnnotation = std::string("{\"key\":\"") +
      annotation.first + "\",\"value\":\"" + annotation.second + "\"}";
    _form.emplace("metadata", formAnnotation);
  }

  // Recursively get all the files.
  std::vector<std::string> files;
  this->AllFiles(_pathToModelDir, files);
  for (const std::string &file : files)
  {
    _form.emplace("file", std::string("@") + file + ";"
        + file.substr(_pathToModelDir.size()+1));
  }

  return true;
}

//////////////////////////////////////////////////
void FuelClient::Implementation::PopulateLicenses(const ServerConfig &_server)
{
  RestResponse resp;

  // Send the request.
  resp = this->rest.Request(HttpMethod::GET, _server.Url().Str(),
      _server.Version(), "licenses", {}, {}, "");
  if (resp.statusCode != 200)
  {
    gzerr << "Failed to get license information from "
      << _server.Url().Str() << "/" << _server.Version() << std::endl;
  }
  else if (!JSONParser::ParseLicenses(resp.data, this->licenses))
  {
    gzerr << "Failed to parse license information[" << resp.data << "]\n";
  }
}

}  // namespace gz::fuel_tools

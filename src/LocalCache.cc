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

#ifndef _WIN32
  #include <unistd.h>
#endif

#include <stdio.h>
#include <tinyxml2.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/StringUtils.hh>
#include <ignition/common/Util.hh>

#include "ignition/fuel_tools/ClientConfig.hh"
#include "ignition/fuel_tools/LocalCache.hh"
#include "ignition/fuel_tools/ModelIterPrivate.hh"
#include "ignition/fuel_tools/ModelPrivate.hh"
#include "ignition/fuel_tools/Zip.hh"
#include "ignition/fuel_tools/WorldIterPrivate.hh"

using namespace ignition;
using namespace fuel_tools;

class ignition::fuel_tools::LocalCachePrivate
{
  /// \brief return all models in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<Model> ModelsInServer(const std::string &_path) const;

  /// \brief Return all worlds in a given directory
  /// \param[in] _path A directory for the local server cache
  public: std::vector<WorldIdentifier> WorldsInServer(
      const std::string &_path) const;

  /// \brief return all models in a given Owner/models directory
  public: std::vector<Model> ModelsInPath(const std::string &_path);

  /// \brief Fix model:// URI paths
  /// \param[in] _modelVersionedDir Directory containing the model.
  public: bool FixPaths(const std::string &_modelVersionedDir);

  /// \brief Helper function to fix model:// URI paths in geometry elements.
  /// \param[in] _geomElem Pointer to the geometry element.
  /// \param[in] _modelVersionedDir Directory containing the model.
  public: void FixPathsInGeomElement(tinyxml2::XMLElement *_geomElem,
              const std::string &_modelVersionedDir);

  /// \brief client configuration
  public: const ClientConfig *config = nullptr;
};

//////////////////////////////////////////////////
std::vector<Model> LocalCachePrivate::ModelsInServer(
    const std::string &_path) const
{
  std::vector<Model> models;
  if (!common::isDirectory(_path))
  {
    ignwarn << "Server directory does not exist [" << _path << "]\n";
    return models;
  }

  common::DirIter end;
  common::DirIter ownIter(_path);
  while (ownIter != end)
  {
    if (!common::isDirectory(*ownIter))
    {
      ++ownIter;
      continue;
    }

    // This is an owner directory, look for models
    common::DirIter modIter(common::joinPaths(*ownIter, "models"));
    while (modIter != end)
    {
      if (!common::isDirectory(*modIter))
      {
        ++modIter;
        continue;
      }

      // Go through all versions
      common::DirIter versionIter(common::absPath(*modIter));
      while (versionIter != end)
      {
        if (!common::isDirectory(*versionIter))
        {
          ++versionIter;
          continue;
        }

        if (common::exists(common::joinPaths(*versionIter, "model.config")))
        {
          std::shared_ptr<ModelPrivate> modPriv(new ModelPrivate);
          modPriv->id.SetName(common::basename(*modIter));
          modPriv->id.SetOwner(common::basename(*ownIter));
          modPriv->id.SetVersionStr(common::basename(*versionIter));
          modPriv->pathOnDisk = common::absPath(*versionIter);
          Model model(modPriv);
          models.push_back(model);
        }
        ++versionIter;
      }
      ++modIter;
    }
    ++ownIter;
  }
  return models;
}

//////////////////////////////////////////////////
std::vector<WorldIdentifier> LocalCachePrivate::WorldsInServer(
    const std::string &_path) const
{
  std::vector<WorldIdentifier> worldIds;
  if (!common::isDirectory(_path))
  {
    ignwarn << "Server directory does not exist [" << _path << "]\n";
    return worldIds;
  }

  common::DirIter end;
  common::DirIter ownIter(_path);
  while (ownIter != end)
  {
    if (!common::isDirectory(*ownIter))
    {
      ++ownIter;
      continue;
    }

    // This is an owner directory, look for worlds
    common::DirIter worldIter(common::joinPaths(*ownIter, "worlds"));
    while (worldIter != end)
    {
      if (!common::isDirectory(*worldIter))
      {
        ++worldIter;
        continue;
      }

      // Go through all versions
      common::DirIter versionIter(common::absPath(*worldIter));
      while (versionIter != end)
      {
        if (!common::isDirectory(*versionIter))
        {
          ++versionIter;
          continue;
        }

        WorldIdentifier id;
        id.SetName(common::basename(*worldIter));
        id.SetOwner(common::basename(*ownIter));
        id.SetVersionStr(common::basename(*versionIter));
        id.SetLocalPath(common::absPath(*versionIter));
        worldIds.push_back(id);

        ++versionIter;
      }
      ++worldIter;
    }
    ++ownIter;
  }
  return worldIds;
}

//////////////////////////////////////////////////
LocalCache::LocalCache(const ClientConfig *_config)
  : dataPtr(new LocalCachePrivate)
{
  this->dataPtr->config = _config;
}

//////////////////////////////////////////////////
LocalCache::~LocalCache()
{
}

//////////////////////////////////////////////////
ModelIter LocalCache::AllModels()
{
  std::vector<Model> models;
  if (this->dataPtr->config)
  {
    for (auto &server : this->dataPtr->config->Servers())
    {
      std::string path = common::joinPaths(
          this->dataPtr->config->CacheLocation(), server.Url().Path().Str());
      auto srvModels = this->dataPtr->ModelsInServer(path);
      for (auto &mod : srvModels)
      {
        mod.dataPtr->id.SetServer(server);
      }
      models.insert(models.end(), srvModels.begin(), srvModels.end());
    }
  }

  return ModelIterFactory::Create(models);
}

//////////////////////////////////////////////////
WorldIter LocalCache::AllWorlds() const
{
  std::vector<WorldIdentifier> worldIds;
  if (this->dataPtr->config)
  {
    // Iterate over servers
    for (auto &server : this->dataPtr->config->Servers())
    {
      std::string path = common::joinPaths(
          this->dataPtr->config->CacheLocation(), server.Url().Path().Str());

      // Make sure the server info is correct
      auto srvWorlds = this->dataPtr->WorldsInServer(path);
      for (auto &world : srvWorlds)
      {
        WorldIdentifier id = world;
        id.SetServer(server);

        worldIds.push_back(id);
      }
    }
  }

  return WorldIterFactory::Create(worldIds);
}

//////////////////////////////////////////////////
Model LocalCache::MatchingModel(const ModelIdentifier &_id)
{
  // For the tip, we have to find the highest version
  bool tip = (_id.Version() == 0);
  Model tipModel;

  for (ModelIter iter = this->AllModels(); iter; ++iter)
  {
    ModelIdentifier id = iter->Identification();
    if (_id == id)
    {
      if (_id.Version() == id.Version())
        return *iter;
      else if (tip && id.Version() > tipModel.Identification().Version())
        tipModel = *iter;
    }
  }

  return tipModel;
}

//////////////////////////////////////////////////
bool LocalCache::MatchingWorld(WorldIdentifier &_id) const
{
  // For the tip, we have to find the highest version
  bool tip = (_id.Version() == 0);
  WorldIdentifier tipWorld;

  for (auto id = this->AllWorlds(); id; ++id)
  {
    if (_id == id)
    {
      if (_id.Version() == id->Version())
      {
        _id = id;
        return true;
      }
      else if (tip && id->Version() > tipWorld.Version())
      {
        tipWorld = id;
      }
    }
  }

  auto foundTip = !(tipWorld == WorldIdentifier());
  if (foundTip)
  {
    _id = tipWorld;
  }

  return foundTip;
}

//////////////////////////////////////////////////
ModelIter LocalCache::MatchingModels(const ModelIdentifier &_id)
{
  if (_id.Name().empty() && !_id.Server().Url().Valid() && _id.Owner().empty())
    return ModelIterFactory::Create();

  std::vector<Model> models;
  for (auto iter = this->AllModels(); iter; ++iter)
  {
    bool matches = true;
    if (!_id.Name().empty() && _id.Name() != iter->Identification().Name())
      matches = false;
    if (!_id.Owner().empty() && _id.Owner() != iter->Identification().Owner())
      matches = false;
    if (_id.Server().Url().Valid() &&
        _id.Server().Url().Str() != iter->Identification().Server().Url().Str())
      matches = false;
    if (matches)
      models.push_back(*iter);
  }
  return ModelIterFactory::Create(models);
}

//////////////////////////////////////////////////
WorldIter LocalCache::MatchingWorlds(const WorldIdentifier &_id) const
{
  if (_id.Name().empty() && !_id.Server().Url().Valid() && _id.Owner().empty())
    return WorldIterFactory::Create();

  std::vector<WorldIdentifier> worldIds;
  for (auto iter = this->AllWorlds(); iter; ++iter)
  {
    bool matches = true;
    if (!_id.Name().empty() && _id.Name() != iter->Name())
      matches = false;
    if (!_id.Owner().empty() && _id.Owner() != iter->Owner())
      matches = false;
    if (_id.Server().Url().Valid() &&
        _id.Server().Url().Str() != iter->Server().Url().Str())
      matches = false;
    if (matches)
      worldIds.push_back(iter);
  }
  return WorldIterFactory::Create(worldIds);
}

//////////////////////////////////////////////////
bool LocalCache::SaveModel(
  const ModelIdentifier &_id, const std::string &_data, const bool _overwrite)
{
  if (_id.Server().Url().Str().empty() || _id.Owner().empty() ||
      _id.Name().empty() || _id.Version() == 0)
  {
    ignerr << "Incomplete model identifier, failed to save model." << std::endl
           << _id.AsString();
    return false;
  }

  std::string cacheLocation = this->dataPtr->config->CacheLocation();

  std::string modelRootDir = common::joinPaths(cacheLocation,
      _id.Server().Url().Path().Str(), _id.Owner(), "models", _id.Name());
  std::string modelVersionedDir =
    common::joinPaths(modelRootDir, _id.VersionStr());

  // Is it already in the cache?
  if (common::isDirectory(modelVersionedDir) && !_overwrite)
  {
    ignerr << "Directory [" << modelVersionedDir << "] already exists"
           << std::endl;
    return false;
  }

  // Create the model directory.
  if (!common::createDirectories(modelVersionedDir))
  {
    ignerr << "Unable to create directory [" << modelVersionedDir << "]"
           << std::endl;
  }

  auto zipFile = common::joinPaths(modelVersionedDir, _id.Name() + ".zip");
  std::ofstream ofs(zipFile, std::ofstream::out);
  ofs << _data;
  ofs.close();

  if (!Zip::Extract(zipFile, modelVersionedDir))
  {
    ignerr << "Unable to unzip [" << zipFile << "]" << std::endl;
    return false;
  }

  // Convert model:// URIs to locations on disk.
  this->dataPtr->FixPaths(modelVersionedDir);

  // Cleanup the zip file.
  if (!common::removeDirectoryOrFile(zipFile))
  {
    ignwarn << "Unable to remove [" << zipFile << "]" << std::endl;
  }

  ignmsg << "Saved model at:" << std::endl
         << "  " << modelVersionedDir << std::endl;

  return true;
}

//////////////////////////////////////////////////
bool LocalCachePrivate::FixPaths(const std::string &_modelVersionedDir)
{
  // Get model.config
  std::string modelConfigPath = common::joinPaths(
      _modelVersionedDir, "model.config");

  // Make sure the model config file exits.
  if (common::exists(modelConfigPath))
  {
    // Load the model config into tinyxml
    tinyxml2::XMLDocument modelConfigDoc;
    if (modelConfigDoc.LoadFile(modelConfigPath.c_str()) !=
        tinyxml2::XML_SUCCESS)
    {
      ignerr << "Unable to load model.config file[" << modelConfigPath << "]\n";
      return false;
    }

    // Get the first <model> element. There really should only be one, but
    // we are not being strict.
    tinyxml2::XMLElement *modelElement = modelConfigDoc.FirstChildElement(
        "model");

    // Get the <sdf> element with the highest (most recent) version.
    tinyxml2::XMLElement *sdfElementLatest = nullptr;
    double maxVersion = 0.0;
    tinyxml2::XMLElement *sdfElement = modelElement->FirstChildElement("sdf");
    while (sdfElement)
    {
      double version = std::stod(sdfElement->Attribute("version"));
      if (version > maxVersion)
      {
        maxVersion = version;
        sdfElementLatest = sdfElement;
      }

      sdfElement = sdfElement->NextSiblingElement("sdf");
    }

    // Get name of the model SDF file.
    std::string modelSdfFilePath = common::joinPaths(_modelVersionedDir,
        sdfElementLatest->GetText());

    // Load the model SDF file
    tinyxml2::XMLDocument modelSdfDoc;
    if (modelSdfDoc.LoadFile(modelSdfFilePath.c_str()) !=
        tinyxml2::XML_SUCCESS)
    {
      ignerr << "Unable to load SDF file[" << modelSdfFilePath << "]\n";
      return false;
    }

    // Process each <model>
    tinyxml2::XMLElement *modelElem =
      modelSdfDoc.RootElement()->FirstChildElement("model");
    while (modelElem)
    {
      // Process each <link>
      tinyxml2::XMLElement *linkElem = modelElem->FirstChildElement("link");
      while (linkElem)
      {
        // Process each <collision>
        tinyxml2::XMLElement *collisionElem =
          linkElem->FirstChildElement("collision");
        while (collisionElem)
        {
          this->FixPathsInGeomElement(
              collisionElem->FirstChildElement("geometry"), _modelVersionedDir);
          // Next collision element.
          collisionElem = collisionElem->NextSiblingElement("collision");
        }

        // Process each <visual>
        tinyxml2::XMLElement *visualElem =
          linkElem->FirstChildElement("visual");
        while (visualElem)
        {
          this->FixPathsInGeomElement(
              visualElem->FirstChildElement("geometry"), _modelVersionedDir);
          visualElem = visualElem->NextSiblingElement("visual");
        }
        linkElem = linkElem->NextSiblingElement("link");
      }
      modelElem = modelElem->NextSiblingElement("model");
    }
    modelSdfDoc.SaveFile(modelSdfFilePath.c_str());
  }
  else
  {
    ignerr << "model.config file does not exist in ["
      << _modelVersionedDir << ".\n";
    return false;
  }

  return true;
}

//////////////////////////////////////////////////
void LocalCachePrivate::FixPathsInGeomElement(tinyxml2::XMLElement *_geomElem,
    const std::string &_modelVersionedDir)
{
  if (!_geomElem)
    return;

  // Get the mesh element, if present.
  tinyxml2::XMLElement *meshElem = _geomElem->FirstChildElement("mesh");

  if (meshElem)
  {
    tinyxml2::XMLElement *uriElem = meshElem->FirstChildElement("uri");

    // Convert the "model://" URI pattern to file://
    if (uriElem)
    {
      std::string uri = uriElem->GetText();
      std::string prefix =  "model://";

      // Make sure the URI is of the form model://
      if (uri.find(prefix) != std::string::npos)
      {
        int firstSlash = uri.find('/', prefix.size()+1);
        std::string suffix = uri.substr(firstSlash);

        // Convert the model:// to point to an actual file.
        // \todo(nkoenig) Handle URIs to other models. For
        // example, ModelA may use something from ModelB.
        std::string diskPath = common::joinPaths("file:/",
            _modelVersionedDir, suffix);

        uriElem->SetText(diskPath.c_str());
      }
    }
  }
}

//////////////////////////////////////////////////
bool LocalCache::SaveWorld(
  WorldIdentifier &_id, const std::string &_data, const bool _overwrite)
{
  if (!_id.Server().Url().Valid() || _id.Owner().empty() ||
      _id.Name().empty() || _id.Version() == 0)
  {
    ignerr << "Incomplete world identifier, failed to save world." << std::endl
           << _id.AsString();
    return false;
  }

  auto cacheLocation = this->dataPtr->config->CacheLocation();

  auto worldRootDir = common::joinPaths(cacheLocation, _id.UniqueName());
  auto worldVersionedDir = common::joinPaths(worldRootDir, _id.VersionStr());

  // Is it already in the cache?
  if (common::isDirectory(worldVersionedDir) && !_overwrite)
  {
    ignerr << "Directory [" << worldVersionedDir << "] already exists"
           << std::endl;
    return false;
  }

  // Create the world directory.
  if (!common::createDirectories(worldVersionedDir))
  {
    ignerr << "Unable to create directory [" << worldVersionedDir << "]"
           << std::endl;
  }

  auto zipFile = common::joinPaths(worldVersionedDir, _id.Name() + ".zip");
  std::ofstream ofs(zipFile, std::ofstream::out);
  ofs << _data;
  ofs.close();

  if (!Zip::Extract(zipFile, worldVersionedDir))
  {
    ignerr << "Unable to unzip [" << zipFile << "]" << std::endl;
    return false;
  }

  if (!common::removeDirectoryOrFile(zipFile))
  {
    ignwarn << "Unable to remove [" << zipFile << "]" << std::endl;
  }

  _id.SetLocalPath(worldVersionedDir);
  ignmsg << "Saved world at:" << std::endl
         << "  " << worldVersionedDir << std::endl;

  return true;
}


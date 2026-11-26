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

#include <gtest/gtest.h>
#include <string>
#include <gz/common/Console.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "gz/fuel_tools/ClientConfig.hh"
#include "gz/fuel_tools/ModelIdentifier.hh"

using namespace gz;
using namespace fuel_tools;

/////////////////////////////////////////////////
/// \brief Fields can be set
TEST(ModelIdentifier, SetFields)
{
  ModelIdentifier id;
  id.SetName("hello");
  id.SetOwner("acai");
  id.SetVersion(6);
  id.SetFileSize(2048u);
  std::time_t d1;
  std::time(&d1);
  id.SetModifyDate(d1);
  std::time_t d2;
  std::time(&d2);
  id.SetUploadDate(d2);

  EXPECT_FALSE(id.Private());
  id.SetPrivate(true);
  EXPECT_TRUE(id.Private());

  EXPECT_EQ(std::string("hello"), id.Name());
  EXPECT_EQ(std::string("acai"), id.Owner());
  EXPECT_EQ(6u, id.Version());
  EXPECT_EQ(2048u, id.FileSize());
  EXPECT_EQ(d1, id.ModifyDate());
  EXPECT_EQ(d2, id.UploadDate());
}

/////////////////////////////////////////////////
/// \brief Unique Name
TEST(ModelIdentifier, UniqueName)
{
  gz::fuel_tools::ServerConfig srv1;
  srv1.SetUrl(common::URI("https://localhost:8001", true));

  gz::fuel_tools::ServerConfig srv2;
  srv2.SetUrl(common::URI("https://localhost:8002", true));

  gz::fuel_tools::ServerConfig srv3;
  srv3.SetUrl(common::URI("https://localhost:8003", true));

  ModelIdentifier id;
  id.SetName("hello");
  id.SetOwner("alice");
  id.SetServer(srv1);
  EXPECT_EQ("localhost%3A8001/alice/models/hello", id.UniqueName());

  id.SetServer(srv2);
  EXPECT_EQ("localhost%3A8002/alice/models/hello", id.UniqueName());

  id.SetServer(srv3);
  EXPECT_EQ("localhost%3A8003/alice/models/hello", id.UniqueName());
}

/////////////////////////////////////////////////
/// \brief Url
TEST(ModelIdentifier, Url)
{
  gz::fuel_tools::ServerConfig srv1;
  srv1.SetUrl(common::URI("https://localhost:8001", true));

  gz::fuel_tools::ServerConfig srv2;
  srv2.SetUrl(common::URI("https://localhost:8002", true));

  gz::fuel_tools::ServerConfig srv3;
  srv3.SetUrl(common::URI("https://localhost:8003", true));

  ModelIdentifier id;
  id.SetName("hello");
  id.SetOwner("alice");
  id.SetServer(srv1);
  EXPECT_EQ("https://localhost:8001/alice/models/hello", id.Url().Str());

  id.SetServer(srv2);
  EXPECT_EQ("https://localhost:8002/alice/models/hello", id.Url().Str());

  id.SetServer(srv3);
  EXPECT_EQ("https://localhost:8003/alice/models/hello", id.Url().Str());
}

/////////////////////////////////////////////////
/// \brief Thumbnail URL
TEST(ModelIdentifier, ThumbnailUrl)
{
  gz::fuel_tools::ServerConfig srv;
  srv.SetUrl(common::URI("https://fuel.gazebosim.org", true));

  ModelIdentifier id;
  id.SetServer(srv);

  // No thumbnail
  EXPECT_TRUE(id.ThumbnailUrl().Str().empty());

  // Fuel returns the URL relative to the server and its API version
  EXPECT_TRUE(id.SetThumbnailUrl(
      "/OpenRobotics/models/Chair/tip/files/thumbnails/1.png"));
  EXPECT_EQ("https://fuel.gazebosim.org/1.0/OpenRobotics/models/Chair/tip/"
            "files/thumbnails/1.png", id.ThumbnailUrl().Str());

  // Percent-encoded characters are kept
  id.SetThumbnailUrl(
      "/OpenRobotics/models/Dining%20Chair/tip/files/thumbnails/a.png");
  EXPECT_EQ("https://fuel.gazebosim.org/1.0/OpenRobotics/models/"
            "Dining%20Chair/tip/files/thumbnails/a.png",
            id.ThumbnailUrl().Str());

  // Spaces in file names are kept
  id.SetThumbnailUrl(
      "/alice/models/cone/tip/files/thumbnails/Screenshot from 2025.png");
  EXPECT_EQ("https://fuel.gazebosim.org/1.0/alice/models/cone/tip/files/"
            "thumbnails/Screenshot from 2025.png", id.ThumbnailUrl().Str());

  // Relative path without a leading slash
  id.SetThumbnailUrl("alice/models/hello/tip/files/thumbnails/0.jpg");
  EXPECT_EQ("https://fuel.gazebosim.org/1.0/alice/models/hello/tip/files/"
            "thumbnails/0.jpg", id.ThumbnailUrl().Str());

  // The server's API version and a trailing slash in its URL
  srv.SetUrl(common::URI("https://localhost:8001/", true));
  srv.SetVersion("2.0");
  id.SetServer(srv);
  EXPECT_EQ("https://localhost:8001/2.0/alice/models/hello/tip/files/"
            "thumbnails/0.jpg", id.ThumbnailUrl().Str());

  // Absolute URLs are used as they are
  id.SetThumbnailUrl("https://example.org/thumbnails/1.jpg");
  EXPECT_EQ("https://example.org/thumbnails/1.jpg", id.ThumbnailUrl().Str());

  // Clearing it
  id.SetThumbnailUrl("");
  EXPECT_TRUE(id.ThumbnailUrl().Str().empty());
}

/////////////////////////////////////////////////
/// \brief Copy constructor deep copies
TEST(ModelIdentifier, CopyConstructorDeepCopy)
{
  ModelIdentifier id;
  id.SetName("hello");
  id.SetOwner("watermelon");
  id.SetVersionStr("");
  id.SetFileSize(2048u);
  std::time_t d1;
  std::time(&d1);
  id.SetModifyDate(d1);
  std::time_t d2;
  std::time(&d2);
  id.SetUploadDate(d2);

  ModelIdentifier id2(id);
  EXPECT_EQ(std::string("hello"), id2.Name());
  EXPECT_EQ(std::string("watermelon"), id2.Owner());
  EXPECT_EQ("tip", id2.VersionStr());
  EXPECT_EQ(2048u, id2.FileSize());
  EXPECT_EQ(d1, id2.ModifyDate());
  EXPECT_EQ(d2, id2.UploadDate());
  EXPECT_EQ(id, id2);

  id2.SetName("hello2");
  EXPECT_EQ(std::string("hello"), id.Name());
  EXPECT_EQ(std::string("hello2"), id2.Name());
  EXPECT_NE(id, id2);
}

/////////////////////////////////////////////////
/// \brief assignment operator deep copies
TEST(ModelIdentifier, AssignmentOperatorDeepCopy)
{
  ModelIdentifier id;
  id.SetName("hello");
  id.SetOwner("pineapple");
  id.SetVersionStr("tip");
  id.SetFileSize(2048u);
  std::time_t d1;
  std::time(&d1);
  id.SetModifyDate(d1);
  std::time_t d2;
  std::time(&d2);
  id.SetUploadDate(d2);

  ModelIdentifier id2(id);
  id2 = id;
  EXPECT_EQ(std::string("hello"), id2.Name());
  EXPECT_EQ(std::string("pineapple"), id2.Owner());
  EXPECT_EQ(0u, id2.Version());
  EXPECT_EQ(2048u, id2.FileSize());
  EXPECT_EQ(d1, id2.ModifyDate());
  EXPECT_EQ(d2, id2.UploadDate());


  id2.SetName("hello2");
  EXPECT_EQ(std::string("hello"), id.Name());
  EXPECT_EQ(std::string("hello2"), id2.Name());
}

/////////////////////////////////////////////////
TEST(ModelIdentifier, AsString)
{
  common::Console::SetVerbosity(4);
  {
    ModelIdentifier id;
    std::string str =
        "Name: \n"\
        "Owner: \n"\
        "Version: tip\n"\
        "Unique name: fuel.gazebosim.org/models/\n"
        "Description: \n"
        "File size: 0\n"
        "Upload date: 0\n"
        "Likes: 0\n"
        "Downloads: 0\n"
        "License name: \n"
        "License URL: \n"
        "License image URL: \n"
        "Tags: \n"
        "Server:\n"
        "  URL: https://fuel.gazebosim.org\n"
        "  Version: 1.0\n"
        "  API key: \n";
    EXPECT_EQ(str, id.AsString());
  }

  {
    ModelIdentifier id;
    id.SetName("hello");
    id.SetOwner("raspberry");
    id.SetVersionStr("55");
    id.SetFileSize(2048u);
    std::time_t d1;
    std::time(&d1);
    id.SetModifyDate(d1);
    std::time_t d2;
    std::time(&d2);
    id.SetUploadDate(d2);

    auto str = id.AsString();
    gzdbg << str << std::endl;

    EXPECT_NE(str.find("hello"), std::string::npos);
    EXPECT_NE(str.find("raspberry"), std::string::npos);
    EXPECT_NE(str.find("55"), std::string::npos);
    EXPECT_NE(str.find("2048"), std::string::npos);
  }
}

/////////////////////////////////////////////////
TEST(ModelIdentifier, AsPrettyString)
{
  common::Console::SetVerbosity(4);
  {
    ModelIdentifier id;
    std::string str =
      "\x1B[96m\x1B[1mServer:\x1B[0m\n  "
      "\x1B[96m\x1B[1mURL: \x1B[0m\x1B[37mhttps://fuel.gazebosim.org"
      "\x1B[0m\n  \x1B[96m\x1B[1mVersion: \x1B[0m\x1B[37m1.0\x1B[0m\n";
    EXPECT_EQ(str, id.AsPrettyString());
  }

  {
    ModelIdentifier id;
    id.SetName("hello");
    id.SetOwner("raspberry");
    id.SetVersionStr("55");
    id.SetFileSize(2048u);
    std::time_t d1;
    std::time(&d1);
    id.SetModifyDate(d1);
    std::time_t d2;
    std::time(&d2);
    id.SetUploadDate(d2);

    auto str = id.AsPrettyString();
    gzdbg << str << std::endl;

    EXPECT_NE(str.find("hello"), std::string::npos);
    EXPECT_NE(str.find("raspberry"), std::string::npos);
    EXPECT_NE(str.find("55"), std::string::npos);
    EXPECT_NE(str.find("2048"), std::string::npos);
  }
}

# Gazebo Fuel Tools: Classes and tools for interacting with Gazebo Fuel

**Maintainer:** nate AT openrobotics DOT org

[![GitHub open issues](https://img.shields.io/github/issues-raw/gazebosim/gz-fuel-tools.svg)](https://github.com/gazebosim/gz-fuel-tools/issues)
[![GitHub open pull requests](https://img.shields.io/github/issues-pr-raw/gazebosim/gz-fuel-tools.svg)](https://github.com/gazebosim/gz-fuel-tools/pulls)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/gh/gazebosim/gz-fuel-tools/branch/main/graph/badge.svg)](https://codecov.io/gh/gazebosim/gz-fuel-tools/branch/main)
Ubuntu Jammy  | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=gz_fuel_tools-ci-gz-fuel-tools9-jammy-amd64)](https://build.osrfoundation.org/job/gz_fuel_tools-ci-gz-fuel-tools9-jammy-amd64)
Homebrew      | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=gz_fuel_tools-ci-gz-fuel-tools9-homebrew-amd64)](https://build.osrfoundation.org/job/gz_fuel_tools-ci-gz-fuel-tools9-homebrew-amd64)
Windows       | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=gz_fuel_tools-9-win)](https://build.osrfoundation.org/job/gz_fuel_tools-9-win/)

Gazebo Fuel Tools is composed by a client library and command line tools for
interacting with Gazebo Fuel servers.

  [http://github.com/gazebosim/gz-fuel-tools](http://github.com/gazebosim/gz-fuel-tools)

# Building and installing

See the [installation tutorial](https://gazebosim.org/api/fuel_tools/9/install.html).

Make sure `GZ_CONFIG_PATH` is set to the right install location so that `gz fuel` will work.
Default is `/usr/local/share/gz`.

## Examples

For a complete list of commands run `gz fuel -h` on the command line.

**List all models**
```{.sh}
$ gz fuel list -t model -r | head
https://fuel.gazebosim.org/anonymous/test_model_595389531
https://fuel.gazebosim.org/anonymous/test_model_122023392
https://fuel.gazebosim.org/anonymous/test_model_429486665
https://fuel.gazebosim.org/anonymous/test_model_887243621
https://fuel.gazebosim.org/anonymous/test_model_084900530
https://fuel.gazebosim.org/anonymous/test_model_240061059
https://fuel.gazebosim.org/anonymous/test_model_464734097
https://fuel.gazebosim.org/anonymous/test_model_658598990
https://fuel.gazebosim.org/anonymous/test_model_834617935
https://fuel.gazebosim.org/anonymous/test_model_380348669
```

**Download a model**
```{.sh}
$ gz fuel download -u https://fuel.gazebosim.org/1.0/OpenRobotics/models/Ambulance -v 4
Downloading model:
  Name: Ambulance
  Owner: openrobotics
  Server:
    URL: https://fuel.gazebosim.org
    Version: 1.0

Download succeeded.
```

**C++ Get List models**
```cpp
  // Create a client (uses https://fuel.gazebosim.org by default)
  gz::fuel_tools::ClientConfig conf;
  gz::fuel_tools::FuelClient client(conf);
  gz::fuel_tools::ModelIter iter = client.Models();
  while (iter)
  {
    std::cout << "Got model: " << iter->Identification().Name() << "\n";
  }
```

**Upload a model**

Create an account on
[https://app.gazebosim.org/](https://app.gazebosim.org/) and log
in.

Create a private token at
[https://app.gazebosim.org/settings#access_tokens](https://app.gazebosim.org/settings#access_tokens). Store the generated token someplace safe.

The private token can then used to upload a model:
```{.sh}
gz fuel upload -m ~/path_to_model --header 'Private-token: <TOKEN>'
```

## Dependencies
On ubuntu run
```{.sh}
sudo apt install ruby-ffi libzip-dev libcurl-dev libjsoncpp-dev
```

## Roadmap

* Create the notion of "asset repository" or similar. An asset repository abstracts an entity that can store assets. It can be local or remote. This is the interface for "asset repository":
    * List(category).
        E.g.: localRepository.List("models")
        remote1Repository.List("models")
    * Details(assetIdentifier).
        E.g.: Modeldentifier model;
        model.Owner("the_owner");
        model.Name("the_name");
        localRepository.Details(model)
        remote1Repository.Details(model)
    * Create(assetIdentifier, path_to_the_asset).
        E.g.: Modeldentifier model;
        model.Owner("the_owner");
        model.Name("the_name");
        localRepository.Create(model, path_to_the_asset)
        remote1Repository.Create(model, path_to_the_asset)
    * Delete(assetIdentifier).
        E.g.: Modeldentifier model;
        model.Owner("the_owner");
        model.Name("the_name");
        localRepository.Delete(model)
        remote1Repository.Delete(model)
     * CopyTo(assetIdentifier, dst_repository).
        E.g.: Modeldentifier model;
        model.Owner("the_owner");
        model.Name("the_name");
        localRepository.CopyTo(model, remote1Repository)
        remote1Repository.CopyTo(model, localRepository)
    * "LocalRepository" and "RemoteRepository" should implement this interface.
    (Most of the pieces are there, we just need to refactor the code a bit).

* Think about how to detect when new versions of remote models have been uploaded.
    * Idea of a hash.

* Add Gazebo Fuel command line utilities for:
    * detail
    * create
    * delete
    * copyTo

* How to test the client library:
    * Directly against the real backend (staging?)
    * Clone, and compile a local backend?
    * Mocking the backend has the problem of not being in sync with the real backend and missing potential issues.

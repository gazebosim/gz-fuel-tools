# Gazebo Fuel Tools: Classes and tools for interacting with Gazebo Fuel

**Maintainer:** nate AT openrobotics DOT org

[![GitHub open issues](https://img.shields.io/github/issues-raw/gazebosim/gz-fuel-tools.svg)](https://github.com/gazebosim/gz-fuel-tools/issues)
[![GitHub open pull requests](https://img.shields.io/github/issues-pr-raw/gazebosim/gz-fuel-tools.svg)](https://github.com/gazebosim/gz-fuel-tools/pulls)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/gh/gazebosim/gz-fuel-tools/branch/ign-fuel-tools4/graph/badge.svg)](https://codecov.io/gh/gazebosim/gz-fuel-tools/branch/ign-fuel-tools4)
Ubuntu Bionic | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_fuel-tools-ci-ign-fuel-tools4-bionic-amd64)](https://build.osrfoundation.org/job/ignition_fuel-tools-ci-ign-fuel-tools4-bionic-amd64)
Homebrew      | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_fuel-tools-ci-ign-fuel-tools4-homebrew-amd64)](https://build.osrfoundation.org/job/ignition_fuel-tools-ci-ign-fuel-tools4-homebrew-amd64)
Windows       | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_fuel-tools-ci-ign-fuel-tools4-windows7-amd64)](https://build.osrfoundation.org/job/ignition_fuel-tools-ci-ign-fuel-tools4-windows7-amd64)

Gazebo Fuel Tools is composed by a client library and command line tools for
interacting with Gazebo Fuel servers.

  [http://github.com/gazebosim/gz-fuel-tools](http://github.com/gazebosim/gz-fuel-tools)

# Building and installing

See [http://github.com/gazebosim/gz-fuel-tools](http://github.com/gazebosim/gz-fuel-tools/blob/ign-fuel-tools4/tutorials/01_installation.md).

Make sure `IGN_CONFIG_PATH` is set to the right install location so that `ign fuel` will work.
Default is `/usr/local/share/ignition`.

## Examples

For a complete list of commands run `ign fuel -h` on the command line.

** List all models **
```
$ ign fuel list -t model -r | head
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

** Download a model **
```
$ ign fuel download -u https://fuel.gazebosim.org/1.0/OpenRobotics/models/Ambulance -v 4
Downloading model:
  Name: Ambulance
  Owner: openrobotics
  Server:
    URL: https://fuel.gazebosim.org
    Version: 1.0

Download succeeded.
```

** C++ Get List models **
```
  // Create a client (uses https://fuel.gazebosim.org by default)
  gz::fuel_tools::ClientConfig conf;
  gz::fuel_tools::FuelClient client(conf);
  gz::fuel_tools::ModelIter iter = client.Models();
  while (iter)
  {
    std::cout << "Got model: " << iter->Identification().Name() << "\n";
  }
```

** Upload a model **

Create an account on
[https://app.gazebosim.org/](https://app.gazebosim.org/) and log
in.

While logged in, obtain the JWT token of the account from the browser.
In Chrome and Firefox, this can be done by opening Developer Tools (
`Ctrl+Shift+I`). Click on the Console tab, and type in
```
localStorage.id_token
```
This will print out the token.

The token can also be obtained through the Developer Tools GUI.
Click on the Application tab in Chrome (or Storage tab in Firefox), and expand
the Local Storage item.
In Firefox versions < 71.0, this may be under Web Developer, then Storage
Inspector.
Click on the URL displayed, and select Key `id_token`.
Its Value can be copied.

The JWT token can then used to upload the model:
```
ign fuel upload -m ~/path_to_model --header 'authorization: Bearer <JWT TOKEN>'
```

Note that the `upload` command only works for models currently, not worlds.
** Upload a model **

## TODO

See issues beginning with [Fuel backend] in the title. Here are two examples.

** TODO: Find a model on disk **
```
$ ign fuel locate --name am1
/home/developer/.ignition/fuel/fuel.gazebosim.org/alice/am1
```

## Dependencies
On ubuntu run
```
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

* Add gazebo fuel command line utilities for:
    * detail
    * create
    * delete
    * copyTo

* How to test the client library:
    * Directly against the real backend (staging?)
    * Clone, and compile a local backend?
    * Mocking the backend has the problem of not being in sync with the real backend and missing potential issues.

# Known issue of command line tools

In the event that the installation is a mix of Debian and from source, command
line tools from `ign-tools` may not work correctly.

A workaround for a single package is to define the environment variable
`IGN_CONFIG_PATH` to point to the location of the Gazebo library installation,
where the YAML file for the package is found, such as
```
export IGN_CONFIG_PATH=/usr/local/share/ignition
```

However, that environment variable only takes a single path, which means if the
installations from source are in different locations, only one can be specified.

Another workaround for working with multiple Gazebo libraries on the command
line is using symbolic links to each library's YAML file.
```
mkdir ~/.ignition/tools/configs -p
cd ~/.ignition/tools/configs/
ln -s /usr/local/share/ignition/fuel4.yaml .
ln -s /usr/local/share/ignition/transport7.yaml .
ln -s /usr/local/share/ignition/transportlog7.yaml .
...
export IGN_CONFIG_PATH=$HOME/.ignition/tools/configs
```

This issue is tracked [here](https://github.com/gazebosim/gz-tools/issues/8).

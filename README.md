# Ignition Fuel Tools

** Classes and tools for interacting with Ignition Fuel **

Ignition Fuel Tools is composed by a client library and command line tools for
interacting with Ignition Fuel servers.

  [http://bitbucket.org/ignitionrobotics/ign-fuel-tools](http://bitbucket.org/ignitionrobotics/ign-fuel-tools)

Test coverage reports are available at Codecov:

[![codecov](https://codecov.io/bb/ignitionrobotics/ign-fuel-tools/branch/default/graph/badge.svg)](https://codecov.io/bb/ignitionrobotics/ign-fuel-tools)

# Building and installing

```
cd ign-fuel-tools
mkdir build
cd build
cmake ../
make
make test
make install
```

Make sure `IGN_CONFIG_PATH` is set to the right install location`ign fuel` will work.
Default is `/usr/local/share/ignition`.


## Examples

** List all models **
```
$ ign fuel list -t model -r | head
https://fuel.ignitionrobotics.org/anonymous/test_model_595389531
https://fuel.ignitionrobotics.org/anonymous/test_model_122023392
https://fuel.ignitionrobotics.org/anonymous/test_model_429486665
https://fuel.ignitionrobotics.org/anonymous/test_model_887243621
https://fuel.ignitionrobotics.org/anonymous/test_model_084900530
https://fuel.ignitionrobotics.org/anonymous/test_model_240061059
https://fuel.ignitionrobotics.org/anonymous/test_model_464734097
https://fuel.ignitionrobotics.org/anonymous/test_model_658598990
https://fuel.ignitionrobotics.org/anonymous/test_model_834617935
https://fuel.ignitionrobotics.org/anonymous/test_model_380348669

** Download a model **
```
$ ign fuel download -u https://fuel.ignitionrobotics.org/1.0/openrobotics/models/Ambulance -v 4
Downloading model:
  Name: Ambulance
  Owner: openrobotics
  Server:
    URL: https://fuel.ignitionrobotics.org
    Version: 1.0

Download succeeded.
```

** C++ Get List models **
```
  // Create a client (uses https://fuel.ignitionrobotics.org by default)
  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::FuelClient client(conf);
  ignition::fuel_tools::ModelIter iter = client.Models();
  while (iter)
  {
    std::cout << "Got model: " << iter->Identification().Name() << "\n";
  }
```

## TODO

See issues beginning with [Fuel backend] in the title. Here are two examples.

** TODO: Upload **
```
$ ign fuel push --owner trudy --name car --url https://fuel.ignitionrobotics.org/ --path models/car
TODO Upload a model
```

** TODO: Find a model on disk **
```
$ ign fuel locate --name am1
/home/developer/.ignition/fuel/fuel.ignitionrobotics.org/alice/am1
```

## Dependencies
On ubuntu run
```
sudo apt install ruby-ffi libzip-dev libcurl-dev libjsoncpp-dev
```

## Continuous integration

Please refer to the [Bitbucket Pipelines](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/addon/pipelines/home#!/).

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

* Add ignition fuel command line utilities for:
    * detail
    * create
    * delete
    * copyTo

* How to test the client library:
    * Directly against the real backend (staging?)
    * Clone, and compile a local backend?
    * Mocking the backend has the problem of not being in sync with the real backend and missing potential issues.

# Ignition Fuel Tools

** Classes and tools for interacting with Ignition Fuel **

Ignition Fuel Tools is composed by a client library and command line tools for
interacting with Ignition Fuel servers.

  [http://bitbucket.org/ignitionrobotics/ign-fuel-tools](http://bitbucket.org/ignitionrobotics/ign-fuel-tools)

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

## Examples

** List all models **
```
$ ign fuel list | head
https://ignitionfuel.org/anonymous/test_model_595389531
https://ignitionfuel.org/anonymous/test_model_122023392
https://ignitionfuel.org/anonymous/test_model_429486665
https://ignitionfuel.org/anonymous/test_model_887243621
https://ignitionfuel.org/anonymous/test_model_084900530
https://ignitionfuel.org/anonymous/test_model_240061059
https://ignitionfuel.org/anonymous/test_model_464734097
https://ignitionfuel.org/anonymous/test_model_658598990
https://ignitionfuel.org/anonymous/test_model_834617935
https://ignitionfuel.org/anonymous/test_model_380348669
```

** Find a model on disk **
```
$ ign fuel locate --name am1
/home/developer/.ignition/fuel/staging_ignitionfuel/alice/am1
```

** C++ Get List models **
```
  // Create a ClientConfig, TODO create this from a yaml file
  ignition::fuel_tools::ClientConfig conf;
  ignition::fuel_tools::ServerConfig srv;
  srv.URL("https://ignitionfuel.org/");
  srv.LocalName("ignitionfuel");
  conf.AddServer(srv);
  
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
$ ign fuel push --owner trudy --name car --url https://ignitionfuel.org/ --path models/car
TODO Upload a model
```

** TODO: Download**
```
$ ign fuel pull --owner bob --name traffic_signal
TODO Download a model
```

## Dependencies
On ubuntu run
```
sudo apt install ruby-ffi libzip-dev libcurl-dev libjsoncpp-dev
```

## Continuous integration

Please refer to the [Bitbucket Pipelines](https://bitbucket.org/ignitionrobotics/ign-fuel-tools/addon/pipelines/home#!/).


## Documentation

Check [here](http://ignition-fuel-tools.readthedocs.io/en/default/).

[![Documentation Status](https://readthedocs.org/projects/ignition-fuel-tools/badge/?version=default)](https://readthedocs.org/projects/ignition-fuel-tools/?badge=default)

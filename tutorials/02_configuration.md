Configuration
=============

Some aspects of Ignition Fuel Tools can be configured according to the needs of
the library users. This configuration can be done via a YAML configuration file
or programatically.

Ignition Fuel Tools accepts a YAML file with the following syntax:

```
---
# The list of servers.
servers:
  -
    url: https://api.ignitionfuel.org

  # -
    # url: https://myserver

# Where are the assets stored in disk.
# cache:
#   path: /tmp/ignition/fuel
```

The `servers` section specifies all Fuel servers to interact with.
For each server, you must specify the URL to send the HTTP requests.

The `cache` section captures options related with the local storage of the
assets. `path` specifies the local directory where all assets will be
downloaded. If not used, all assets are stored under `$HOME/.ignition/fuel`.

## Custom configuration file path

Ignition Fuel's default configuration file is stored under
`$HOME/.ignition/fuel/config.yaml`, but it is possible to load a configuration
file from a custom path programmatically. Let's see how.

Create a file `/tmp/my_config.yaml` with the following content:

```
---
# The list of servers.
servers:
  -
    url: https://api.ignitionfuel.org

# Where are the assets stored in disk.
cache:
  path: /tmp/ignition/fuel
```

Now, let's use a program that downloads a resource from a server in the custom
configuration file. Let's start by creating a directory for storing all files:

```
mkdir /tmp/conf_tutorial && cd /tmp/conf_tutorial
```

Download the file `download.cc` and save it under `/tmp/conf_tutorial`:

```
wget https://github.com/ignitionrobotics/ign-fuel-tools/raw/ign-fuel-tools1/example/download.cc
```

Also, download `CMakeLists.txt` for compiling the example:

```
wget https://github.com/ignitionrobotics/ign-fuel-tools/raw/ign-fuel-tools1/example/CMakeLists.txt
```

Let's compile the example:

```
mkdir build && cd build
cmake ..
make
```

And now the fun part, execute it:

```
./download -c /tmp/my_config.yaml -o caguero -n Beer -t model
```

Verify that you have the model in
`/tmp/ignition/fuel/api.ignitionfuel.org/caguero/models/Beer`,
as you configured in your YAML file.

## Walkthrough

Let's jump to the interesting parts of the program:

```
// Setup ClientConfig.
ignition::fuel_tools::ClientConfig conf;

if (FLAGS_s != "")
{
  // The user specified a Fuel server via command line.
  ignition::fuel_tools::ServerConfig srv;
  srv.SetUrl(ignition::common::URI(FLAGS_s));

  // Add the extra Fuel server.
  conf.AddServer(srv);
}
```

In the previous block, we can see how a `ClientConfig` is instantiated. This
class lets you specify different options to be customized by the client using
the library.

Next, we check if `-s` (server) has been used when invoking our program.
If that's the case, we instantiate an object of `ServerConfig` and we fill the
URL with the value of the `-s` flag.

Next, we use `AddServer()` to register the new server. The previous block shows
an example of a programmatic way of configuring a Fuel server. Let's focus on
the next interesting piece of code:

```
if (FLAGS_c != "")
    conf.SetConfigPath(FLAGS_c);

if (!conf.LoadConfig())
{
  std::cerr << "Error loading configuration file [" << FLAGS_c << "]"
            << std::endl;
  return -1;
}

// Instantiate the FuelClient object with the configuration.
ignition::fuel_tools::FuelClient client(conf);
```

Here, we check if the user specified a `-c` (config) option. If so, we need to
tell `conf` about the path where the configuration file is located. For this
purpose we use `SetConfigPath()`. As we're interested in using a configuration
file, we need to call `LoadConfig()`. It's important to note that if we call
`LoadConfig()` without calling `SetConfigPath()` beforehand, a default
configuration file will be loaded (and created if it doesn't already exist under
`$HOME/.ignition/fuel/config.yaml`). If the user doesn't call `LoadConfig()`, no
configuration file will be used at all.

Once we have all our configuration ready and captured in the `conf` object,
we can instantiate the `FuelClient` object (client). This is the class that
performs the main operations, such as the model download described in this
example.

```
// Set the properties of the resource that we want to download.
ignition::fuel_tools::ModelIdentifier modelIdentifier;
ignition::fuel_tools::WorldIdentifier worldIdentifier;

if (FLAGS_t == "model")
{
  modelIdentifier.SetOwner(FLAGS_o);
  modelIdentifier.SetName(FLAGS_n);
}
else if (FLAGS_t == "world")
{
  worldIdentifier.SetOwner(FLAGS_o);
  worldIdentifier.SetName(FLAGS_n);
}

// Fetch the resource.
for (const auto &server : client.Config().Servers())
{
  if (FLAGS_t == "model")
  {
    // Set server
    auto id = modelIdentifier;
    id.SetServer(server);

    // Download
    if (client.DownloadModel(server, id))
      return 0;
  }
  else if (FLAGS_t == "world")
  {
    // Set server
    auto id = worldIdentifier;
    id.SetServer(server);

    // Download
    if (client.DownloadWorld(id))
      return 0;
  }
}
```

The code above sets the owner and name of the requested resource. Finally, it
iterates over all available Fuel servers and tries to download it.


\page cppapi C++ API

## Overview 

Gazebo Fuel Tools lets you perform a set of operations over the resources
hosted in any Gazebo Fuel server. These operations allow you to list all
resources of a particular server, get the details of a resource or download it.

We're going to run a few examples, so let's start by creating a directory for
this tutorial:

```
mkdir /tmp/fuel_tutorial && cd /tmp/fuel_tutorial
```

Download the files `list.cc`, `details.cc`, `download.cc`,
`CMakeLists.txt`, and save them under `/tmp/fuel_tutorial`:

```
wget https://github.com/gazebosim/gz-fuel-tools/raw/gz-fuel-tools4/example/list.cc
wget https://github.com/gazebosim/gz-fuel-tools/raw/gz-fuel-tools4/example/details.cc
wget https://github.com/gazebosim/gz-fuel-tools/raw/gz-fuel-tools4/example/download.cc
wget https://github.com/gazebosim/gz-fuel-tools/raw/gz-fuel-tools4/example/CMakeLists.txt
```

Let's start by compiling the examples:

```
mkdir build && cd build
cmake ..
make
```

## List resources from a Fuel server

Run the following example to see the list of models hosted in the default
server:

```
./list -t model
```

You should see the name of the server followed by its list of models. Here's an
example:

```
[https://fuel.gazebosim.org]

  Beer
  Wastebasket
  ambulance3
  ambulance2
  appartment3
  hammer
  another test model
  Test Model
  Bowl
  Cardboard Box
  Cessna C-172
  Fire station (collapsed)
  Beer
  Apartment
  Ambulance
```

### Walkthrough

This example contains some boilerplate to parse command line arguments and load
some configuration. Refer to the [configuration tutorial](configuration.html)
for details about this part of the code. Let's focus on the relevant code for
listing resources:

```
for (const auto &server : client.Config().Servers())
{
  std::cout << "[" << server.Url().Str() << "]\n\n";
  if (FLAGS_t == "model")
  {
    for (auto iter = client.Models(server); iter; ++iter)
      std::cout << "  " << iter->Identification().Name() << "\n";
  }
  else if (FLAGS_t == "world")
  {
    for (auto iter = client.Worlds(server); iter; ++iter)
      std::cout << "  " << iter->Name() << "\n";
  }
  std::cout << std::endl;
}
```

As you can see, we're iterating over the list of servers. For each server, we
use the `Models()` or the `Worlds()` method to retrieve all the relevant
resources from that server. Then, iterate over the resulting resources and print
their names.

## Get details of a resource

Run the following example to see the details of a model hosted in the default
server:

```
./details -o caguero -n Beer -t model
```

You should see the details of the model.
Here's an example:

```
Name: Beer
Owner: caguero
Version: 2
Description: A beer can
File size: 2969297
Upload date: 1516833583
Downloads: 1573
License name: Creative Commons - Attribution
License URL: http://creativecommons.org/licenses/by/4.0
License image URL: https://i.creativecommons.org/l/by/4.0/88x31.png
Server:
  URL: https://fuel.gazebosim.org
  Version: 1.0
```

### Walkthrough

This example contains some boilerplate to parse command line arguments and load
some configuration. Refer to the [configuration tutorial](configuration.html)
for details about this part of the code. Let's focus on the relevant code for
getting details of a resource:

```
// Fetch the details.
for (const auto &server : client.Config().Servers())
{
  if (FLAGS_t == "model")
  {
    // Set server
    auto id = modelIdentifier;
    id.SetServer(server);

    gz::fuel_tools::ModelIdentifier model;
    if (!client.ModelDetails(server, id, model))
      continue;

    std::cout << model.AsPrettyString() << std::endl;
  }
  else if (FLAGS_t == "world")
  {
    // Set server
    auto id = worldIdentifier;
    id.SetServer(server);

    gz::fuel_tools::WorldIdentifier world;
    if (!client.WorldDetails(id, world))
      continue;

    std::cout << world.AsPrettyString() << std::endl;
  }
}
```

As usual, we're iterating over the list of servers. For each server, we
use the `ModelDetails()` or `WorldDetails()` methods to retrieve most of the
properties of the resource. Then if the resource is retrieved successfully, we
use the `AsPrettyString()` method to print its information to the screen.


## Download a resource

Refer to the
[configuration tutorial](configuration.html)
to see an example of how to download resources programmatically.


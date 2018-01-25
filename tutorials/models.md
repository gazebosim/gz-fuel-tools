Models
======

A common asset stored in an Ignition Fuel server is a model. A model represents
an object in simulation. A model has a name and a description of its visual and
physical properties. Models can be as simple as a sphere or complicated as a
highly articulated humanoid robot.

Ignition Fuel Tools lets you perform a set of operations over the models
hosted in any Ignition Fuel server. These operations allow you to list all
models of a particular server, get the details of a model or download a model.

We're going to run a few examples, so let's start by creating a directory for
this tutorial:

```
mkdir /tmp/models_tutorial && cd /tmp/models_tutorial
```

Download the files `modelList.cc`, `modelDetails.cc`, `ModelDownload.cc`,
`CMakeLists.txt`, and save them under `/tmp/models_tutorial`:

```
wget https://bitbucket.org/ignitionrobotics/ign-fuel-tools/raw/ign-fuel-tools1/example/modelList.cc
wget https://bitbucket.org/ignitionrobotics/ign-fuel-tools/raw/ign-fuel-tools1/example/modelDetails.cc
wget https://bitbucket.org/ignitionrobotics/ign-fuel-tools/raw/ign-fuel-tools1/example/modelDownload.cc
wget https://bitbucket.org/ignitionrobotics/ign-fuel-tools/raw/ign-fuel-tools1/example/CMakeLists.txt
```

Let's start by compiling the examples:

```
mkdir build && cd build
cmake ..
make
```

## List models from a Fuel server

Run the following example to see the list of models hosted in the default
server:

```
./modelList
```

You should see the name of the server followed by its list of models. Here's an
example:

```
[https://api.ignitionfuel.org]

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
some configuration. Refer to the [configuration tutorial](https://ignitionrobotics.org/tutorials/fuel-tools/1.0/md__data_ignition_ign-fuel-tools_tutorials_configuration.html)
for details about this part of the code. Let's focus on the relevant code for
listing models:

```
for (const auto &server : client.Config().Servers())
{
  std::cout << "[" << server.URL() << "]\n\n";
  for (auto iter = client.Models(server); iter; ++iter)
    std::cout << "  " << iter->Identification().Name() << "\n";
  std::cout << std::endl;
}
```

As you can see, we're iterating over the list of servers. For each server, we
use the `Models()` method to retrieve all models of that server. Then, we go
over each model using an iterator and call the `Identification()` method from
the `Model` class. `Identification()` returns an object of type
`ModelIdentifier()` containing most of the properties of the model. In our
case, we just show the name.

## Get details of a model

Run the following example to see the details of a model hosted in the default
server:

```
./modelDetails -o caguero -m Beer
```

You should see the name of the server followed by the details of the model.
Here's an example:

```
[https://api.ignitionfuel.org]

  Name: Beer
  Source URL: https://api.ignitionfuel.org
  Unique name: https://api.ignitionfuel.org/1.0/caguero/models/Beer
  Owner: caguero
  Description: A beer can
  Likes: 0
  Downloads: 3
  License name: Creative Commons - Attribution
  License URL: http://creativecommons.org/licenses/by/4.0/
  License image URL: https://i.creativecommons.org/l/by/4.0/88x31.png
  Tags:
```

### Walkthrough

This example contains some boilerplate to parse command line arguments and load
some configuration. Refer to the [configuration tutorial](https://ignitionrobotics.org/tutorials/fuel-tools/1.0/md__data_ignition_ign-fuel-tools_tutorials_configuration.html)
for details about this part of the code. Let's focus on the relevant code for
getting details of a model:

```
// Fetch the model details.
for (const auto &server : client.Config().Servers())
{
  ignition::fuel_tools::ModelIdentifier model;
  if (!client.ModelDetails(server, modelIdentifier, model))
    continue;

  // Show server.
  std::cout << "[" << server.URL() << "]\n\n";

  // Show model details.
  std::cout << "  Name: " << model.Name() << std::endl;
  std::cout << "  Source URL: " << model.Server().URL() << std::endl;
  std::cout << "  Unique name: " << model.UniqueName() << std::endl;
  std::cout << "  Owner: " << model.Owner() << std::endl;
  std::cout << "  Description: " << model.Description() << std::endl;
  std::cout << "  Likes: " << model.Likes() << std::endl;
  std::cout << "  Downloads: " << model.Downloads() << std::endl;
  std::cout << "  License name: " << model.LicenseName() << std::endl;
  std::cout << "  License URL: " << model.LicenseURL() << std::endl;
  std::cout << "  License image URL: " << model.LicenseImageURL() << std::endl;
  std::cout << "  Tags: " << std::endl;
  for (auto const &tag : model.Tags())
    std::cout << "  " << tag << std::endl;
}
```

As usual, we're iterating over the list of servers. For each server, we
use the `ModelDetails()` method to retrieve most of the properties of the model.
Note that the result of `ModelDetails` is set in `model`, which is an
object of type `ModelIdentifier`. If the model is found in the server, we use
`model` to show most of its relevant fields.

## Download a model

Run the following example to download a model from the default server:

```
./modelDownload -o caguero -m Beer
```

Verify that you have the model in `$HOME/.ignition/fuel/models/`.

### Walkthrough

This example contains some boilerplate to parse command line arguments and load
some configuration. Refer to the [configuration tutorial](https://ignitionrobotics.org/tutorials/fuel-tools/1.0/md__data_ignition_ign-fuel-tools_tutorials_configuration.html)
for details about this part of the code. Let's focus on the relevant code for
download a model:

```
// Fetch the model.
for (const auto &server : client.Config().Servers())
{
  if (client.DownloadModel(server, modelIdentifier))
    return 0;
}
```

The previous block code iterates over all available Fuel servers and tries to
download the requested model using the `DownloadModel()` method.

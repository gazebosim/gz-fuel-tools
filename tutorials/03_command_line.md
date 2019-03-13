\page cmdline Command Line Tools

Next Tutorial: \ref cppapi

## Overview

The `ign fuel` command line tool provides a convenient way to handle Ignition
Fuel resources from a terminal.

## List resources

Let's use the command line to list resources available in a web server.
We can do that by running the `ign fuel list` command and pass it the resource
type, such as `model` or `world` as follows:

`ign fuel list -t world`

You should see a list such as:

```
Fetching world list from https://fuel.ignitionrobotics.org...
Received world list (took 350ms).
https://fuel.ignitionrobotics.org
├── openrobotics
│   ├── Empty
│   └── Shapes
└── chapulina
    └── Shapes copy
```

By default, Fuel will list resources from all the servers listed in your
`config.yaml` file. See the
[configuration tutorial](https://ignitionrobotics.org/tutorials/fuel_tools/1.0/md__data_ignition_ign-fuel-tools_tutorials_02_configuration.html)
for more details.

> **Tip**: If you want to see resources from a different Fuel server, add it to
`config.yaml` to and re-run the list command to see resources from both servers!

There are a few other options on the command, run the help to see all of them:

`ign fuel list --help`

### Raw output

One of them is the `--raw` option, which tells the command to print the output in
a way that's easier for scripts to parse. For example, try:

`ign fuel list -t world -r`

And you'll get a list of the world URLs similar to the one below:

```
https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty
https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Shapes
https://fuel.ignitionrobotics.org/1.0/chapulina/worlds/Shapes%20copy
```

### By owner

It's also possible to only list resources belonging to a given user, using the
`--owner` flag. Try for example:

`ign fuel list -t model -o openrobotics`

## Download resources

The command line tool also allows downloading resources from a web server to your
computer. We use the `ign fuel download` tool for this.

We learned above how to get resource URLs. Now we can use these URLs to download
them. For example, try:

`ign fuel download -v 4 -u https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty`

Note that we passed the `-v 4` option so we get a verbose output. You should see something like:

```
Downloading world:
  Name: Empty
  Owner: openrobotics
  Server:
    URL: https://fuel.ignitionrobotics.org
    Version: 1.0

[Msg] Saved world at:
  /home/louise/.ignition/fuel/fuel.ignitionrobotics.org/chapulina/worlds/Empty/1
Download succeeded.
```

Worlds downloaded with the tool get conveniently organized into the same
directory, which we call the "local cache". The path is broken down as follows:

`<local cache path>/<server URL>/<owner name>/<resource type>/<resource name>/<version number>`

> **Tip**: You can change the local cache path on `config.yaml`.

> **Tip**: You can also use other tools such as `wget` to download a zipped file of a world, just add `.zip` to the end of the URL, for example: `wget https://fuel.ignitionrobotics.org/1.0/openrobotics/worlds/Empty.zip`.


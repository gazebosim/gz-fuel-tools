\page install Installation

Next Tutorial: \ref configuration

## Overview

Instructions to install Ignition Fuel Tools on all the platforms
supported: major Linux distributions, Mac OS X and Windows.

## Ubuntu Linux

Setup your computer to accept software from
*packages.osrfoundation.org*:

```{.sh}
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
```

Setup keys:

```
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
```

Install Ignition Fuel Tools:

```
sudo apt-get update
sudo apt-get install libignition-fuel-tools-dev
```

## Mac OS X

Ignition Fuel Tools and several of its dependencies can be compiled on OS
X with [Homebrew](http://brew.sh/) using the [osrf/simulation
tap](https://github.com/osrf/homebrew-simulation). Ignition Fuel Tools can
be installed on Mac OS X 10.11 (El Capitan) or higher.
Installation on older versions requires changing the default standard
library and rebuilding dependencies due to the use of c++11. For
purposes of this documentation, I will assume OS X 10.11 or greater is in use.
Here are the instructions:

Install homebrew, which should also prompt you to install the XCode
command-line tools:

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Run the following commands:

```
brew tap osrf/simulation
brew install ignition-fuel-tools1
```

## Windows

At this moment, Windows instructions are not available.

## Install from sources (Ubuntu Linux)

For compiling the latest version of Ignition Fuel Tools you will need an
Ubuntu distribution equal to 16.04 (Xenial) or newer.

Make sure you have removed the Ubuntu pre-compiled binaries before
installing from source:

```
sudo apt-get remove libignition-fuel-tools-dev
```

Install prerequisites. A clean Ubuntu system will need:

```
sudo apt-get install mercurial cmake pkg-config python ruby-ronn libignition-cmake-dev libignition-common-dev libzip-dev libjsoncpp-dev libcurl4-openssl-dev libyaml-dev
```

Clone the repository into a directory and go into it:

```
hg clone https://bitbucket.org/ignitionrobotics/ign-fuel-tools /tmp/ign-fuel-tools
cd /tmp/ign-fuel-tools
```

Create a build directory and go there:

```
mkdir build
cd build
```

Configure Ignition Fuel Tools (choose either method a or b below):

> A.  Release mode: This will generate optimized code, but will not have
>     debug symbols. Use this mode if you don't need to use GDB.
>
> ```
> cmake ../
> ```
>
> Note: You can use a custom install path to make it easier to switch
> between source and debian installs:
>
> ```
> cmake -DCMAKE_INSTALL_PREFIX=/home/$USER/local ../
> ```
>
> B. Debug mode: This will generate code with debug symbols. Ignition
> Fuel Tools will run slower, but you'll be able to use GDB.
>
> ```
> cmake -DCMAKE_BUILD_TYPE=Debug ../
> ```

The output from `cmake ../` may generate a number of errors and warnings
about missing packages. You must install the missing packages that have
errors and re-run `cmake ../`. Make sure all the build errors are
resolved before continuing (they should be there from the earlier step
in which you installed prerequisites).

Make note of your install path, which is output from cmake and should
look something like:

```
-- Install path: /home/$USER/local
```

Build Ignition Fuel Tools:

```
make -j4
```

Install Ignition Fuel Tools:

```
sudo make install
```

If you decide to install the library in a local directory you'll need to
modify your `LD_LIBRARY_PATH`:

```
echo "export LD_LIBRARY_PATH=<install_path>/local/lib:$LD_LIBRARY_PATH" >> ~/.bashrc
```

### Uninstalling Source-based Install

If you need to uninstall Ignition Fuel Tools or switch back to a
debian-based install when you currently have installed the library from
source, navigate to your source code directory's build folders and run
`make uninstall`:

\code
cd /tmp/ign-fuel-tools/build
sudo make uninstall
\endcode

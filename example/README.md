# Examples

These examples demonstrate various Ignition Fuel tools features.

### Dependencies

Remember to install the `gflags` dependency:

#### Ubuntu

```bash
sudo apt-get install libgflags-dev
```

#### Windows
```bash
conda install gflags  --channel conda-forge
```

## Build

After installing Ignition Fuel tools, from source or from binaries, build with:

```
git clone https://github.com/ignitionrobotics/ign-fuel-tools/
cd ign-fuel-tools/example
mkdir build
cd build
cmake ..
```

### Ubuntu and MacOS

```bash
make
```

### Windows

```bash
cmake ..
cmake --build . --config Release
```

## Run

Several executables are created inside the `build` folder for each example.

You can run each executable from the build folder with `./executablename`. For example:

### Ubuntu and MacOS

`./list -t world`

### Windows

`.\Release\list.exe -t world`

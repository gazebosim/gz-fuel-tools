# Examples

These examples demonstrate various Ignition Fuel tools features.

 conda install gflags  --channel conda-forge

## Build

After installing Ignition Fuel tools, from source or from binaries, build with:

```
git clone https://github.com/ignitionrobotics/ign-fuel-tools/
cd ign-fuel-tools/examples
mkdir build
cd build
cmake ..
```

### Ubuntu and MacOS

```bash
make
```

### Windows

Remember to install the `gflags` dependency:

```bash
conda install gflags  --channel conda-forge
```

Then

```bash
cmake ..
cmake --build . --config Release
cmake --install . --config Release
```

## Run

One executable is created inside the `build` folder for each example.

You can run each executable from the build folder with `./executablename`. For example:

### Ubuntu and MacOS

`./list -t world`

### Windows

`.\Release\list.exe -t world`

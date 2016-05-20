# PFPSim [![Build Status](https://travis-ci.org/pfpsim/PFPSim.svg?branch=master)](https://travis-ci.org/pfpsim/PFPSim)

This repository contains the core PFPSim library of common utilities, base classes, and SystemC support infrastructure which PFPSim-based projects depend on. Additionally, it links to
a fork of [P4, an emerging programming language for packet processing applications](http://p4.org), which can be
used to program the applications that run on PFPSim models.

The PFPSim methodology and workflow is
[described in detail here](https://github.com/pfpsim/pfpgen#pfpsim-methodology--workflow).

### Contents
- [Installation](#installation)
  - [Using the PFPSim GUI Installer](#using-the-pfpsim-gui-installer)
  - [Using Prebuilt Package](#using-prebuilt-package)
  - [Building from Source](#building-from-source)
    - [Dependencies](#dependencies)
    - [Building](#building)
    - [Installing](#installing)
    - [Building Package](#building-package)
    - [Building API Docs](#building-api-docs)
- [Running Tests](#running-tests)
- [Support](#support)
- [Contributing](#contributing)
- [License](#license)
- [Authors](#authors)

# Installation

## Using the PFPSim GUI Installer
We provide [a text-based GUI installer](https://github.com/pfpsim/pfpsim-installer)
for the PFPSim library, compiler, debugger, and required dependencies. This is
the recommended method for installing this library.

## Using Prebuilt Package
We provide pre-built deb packages on [our releases page](https://github.com/pfpsim/PFPSim/releases). These packages
contain both the PFPSim core library, and the PFP P4 fork. They can be installed on Debian-based distributions using

```
sudo dpkg -i pfpsim-X.Y.Z-Linux-COMPILER.deb
```

Currently our automated releases upload packages tagged with the name of the compiler used to build them.
Both the `clang (clang++)` and `g (g++)` packages are built against `libstdc++-4.8` and both should work
regardless of which compiler you use, but we have not tested this extensively.

To use the installed library, it's still necessary to install most of the [dependencies mentioned below](#dependencies).
The build-time dependencies (flexc++, bisonc++, protobuf-compiler) are no longer needed, but all the rest are.

## Building from Source

### Dependencies

You will need CMake and a C++11 compliant compiler to build PFPSim. More information about CMake and build systems
that we like and how to install them
[can be found on our wiki](https://github.com/pfpsim/PFPSim/wiki/Build-managment-[CMAKE-Make-Ninja]).

The following required dependencies can be installed using `apt-get` under Debian or Ubuntu:

- libprotobuf-dev
- bisonc++
- flexc++
- protobuf-compiler

Additionally SystemC and nanomsg must be installed from source:

Due to SystemC's licensing terms, it must be downloaded manually. Download it from
[this page](http://accellera.org/downloads/standards/systemc). Follow the link for "SystemC 2.3.1 (Includes TLM) |	Core SystemC Language and Examples".

Once you have the archive downloaded, SystemC can be built and installed as follows:

```sh
tar -xzf systemc-2.3.1.tgz
cd systemc-2.3.1
./configure --prefix=/usr --with-unix-layout
make
sudo make install
```

Next we need to install [nanomsg](http://nanomsg.org/). It can be downloaded, built and installed as follows

```sh
wget http://download.nanomsg.org/nanomsg-0.5-beta.tar.gz
tar -xzf nanomsg-0.5-beta.tar.gz
cd nanomsg-0.5-beta
./configure
make
sudo make install
```

If you also plan on installing the P4 library, there are some additional required dependencies. More information
is available [in the `p4-behavioral-model` repository](https://github.com/pfpsim/p4-behavioral-model#dependencies).

### Building

Once all of the dependencies are installed, building from source is straightforward.

```
git clone https://github.com/pfpsim/PFPSim.git
mkdir PFPSim/build
cd PFPSim/build
cmake ..
make
```

By default, both the PFPSim Core library, and the PFPSim P4 fork are built. To select which components to build, the
following flags may be passed to cmake:

- `-DPFPSIMCORE=ON` or `-DPFPSIMCORE=OFF` to enable or disable building PFPSim core
- `-DPFP-P4=ON` or `-DPFP-P4=OFF` to enable or disable building the PFPSim P4 fork

### Installing

Once the project is built, it can be installed with `sudo make install`. In order to selectively install
components, they should be enabled or disabled during the build step as specified above.

### Building Package

[CPack](https://cmake.org/Wiki/CMake:Packaging_With_CPack) is used to generated various source/pre-built
packages which make it possible to install PFPSim without building from source.

Once the project is built, packages can be generated with `make package`. Because we use Travis CI to
automate building and deploying releases, the version number of the generated package is controlled
by the `TRAVIS_TAG` environment variable. If you wish to build a package with a specific version number,
then in the build step above, replace the `cmake` command with `TRAVIS_TAG=vX.Y.Z cmake .. ` where X.Y.Z
is your desired version number.

### Building API Docs

We use [doxygen](http://www.stack.nl/~dimitri/doxygen/) to generate API docs. The following dependencies
can be installed from `apt-get`

- doxygen
- graphviz

Then docs can be built with `make doc`

# Running Tests

To validate your installation, we provide a very simple test project. It can be built as follows:

```sh
cd tests/simple-pfp/simple/
mkdir build
cd build
cmake ../src
make
```

Running the simulation model with `./simple-sim -v debug` should then produce the following output:


```
        SystemC 2.3.1-Accellera --- Feb 17 2016 16:19:54
        Copyright (c) 1996-2014 by all Contributors,
        ALL RIGHTS RESERVED
config root is: ./Configs/
Verbostiy Level is: debug
Output dir is: ./
0.000 ns @  Got: 0
10.000 ns @  Got: 1
20.000 ns @  Got: 2
30.000 ns @  Got: 3
40.000 ns @  Got: 4
50.000 ns @  Got: 5
60.000 ns @  Got: 6
70.000 ns @  Got: 7
80.000 ns @  Got: 8
90.000 ns @  Got: 9
```

# Support

If you need help using the PFPSim Framework, please
[send us an email at `pfpsim.help@gmail.com`](mailto:pfpsim.help@gmail.com) - we'd be happy to hear from you!

If you think you've found a bug, or would like to request a new feature,
[please open an issue using github](https://github.com/pfpsim/PFPSim/issues) - we're always trying to improve PFPSim!

# Contributing
If you'd like to contribute code back to PFPSim, please fork this github repository and send us a pull request!
Please make sure that your contribution passes our [continuous integration build](https://travis-ci.org/pfpsim/PFPSim).

Any contribution to the C++ core code must respect the coding guidelines as stated on the wiki. We rely heavily on the
Google C++ Style Guide, with some differences listed in this repository's wiki. Our Travis builds include running
[`cpplint`](https://github.com/google/styleguide/tree/gh-pages/cpplint) to ensure correct style and formatting.

# License

This project is licensed under the GPLv2 - see [LICENSE](LICENSE) for details

# Authors
Copyright (C) 2016 Concordia Univ., Montreal
 - Samar Abdi
 - Umair Aftab
 - Gordon Bailey
 - Faras Dewal
 - Shafigh Parsazad
 - Eric Tremblay

Copyright (C) 2016 Ericsson
- Bochra Boughzala

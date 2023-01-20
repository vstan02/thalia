# Thalia
> A fast, statically typed, general-purpose, procedural programming language

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](http://www.gnu.org/licenses/lgpl-3.0)

## Contents
- [Building and Installation](#building-and-installation)
- [License](#license)
- [Contributing](#contributing)

## Building and Installation

### Dependencies
- C++ compiler
- Build system generator: cmake
- Package manager: conan

These packages can usually be installed through your distributions package manager.

### Building the project
To build the project, we first need to create a separate build directory and `cd` into it:
```
mkdir build
cd build
```

Now that we've created our build directory (assuming it's created in the project root), we can install all dependencies with `conan` and run `cmake` (we need to pass the parent directory path to them, which is where the `CMakeLists.txt` file is located):
```
conan install .. --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Once `cmake` is done generating makefiles, we can build the app by running `make` inside our build directory:
```
make
```

### Running the program
If everything went well with the compilation we can run the executable:
```
./bin/thalia
```

### Installing
To install the app run:
```
sudo make install
```
By default the app will be installed in `/usr/local/bin/` directory.

## License
Thalia is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
For more details, see [LICENSE](https://github.com/vstan02/thalia/blob/master/LICENSE) file.

## Contributing
Contributions are welcome.
If you have a feature request, or have found a bug, feel free to open a [new issue](https://github.com/vstan02/thalia/issues/new).
If you wish to contribute code, see [CONTRIBUTING.md](https://github.com/vstan02/thalia/blob/master/CONTRIBUTING.md) for more details.

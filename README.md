repo for EEG-project

# Dependencies
For the software several different libraries have been used as dependencies. To build and use the software all of them have to be installed. Currently the program will only work on a linux based system.

The list of dependencies:
* [ROOT](https://root.cern.ch/)
* gengetopt
* fftw3
* gcc
* cmake
* doxygen
* catch2
* sfml
* kicad
* esp-idf

To install the needed dependencies on Arch use the following commands:
```bash
sudo pacman -S gengetopt fftw base-devel cmake doxygen catch2 sfml kicad kicad-library

# With the AUR helper of your choice:
yay -S root
```

For the ESP32 building and flashing:
[Espressif Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/linux-setup.html)

# Building
For the building we have chosen to use cmake. To build the pc-parser use the following commands from the root of this repository:
```bash
# Create the build directory
cd src/pc-parser
mkdir build
cd build

# Generate build files and compile
cmake ..
make
```

# Documentation
Run the following commant in pc-parser build directory to generate Doxygen documentation.
```
make docs
```
The generated files will be placed in the docs directory of the pc-parser directory

# Running the pc-parser
From within the build directory the pc-parser can be run with `./pc-parser <arguments>`. To get a list of available arguments to change the data input / display output / filtering of the program the argument `-h` or `--help` can be used.

# Repository structure
The `src` directory contains the various code directories and scripts.
The `docs` directory contains all the files to produce our research paper and documents.
The `circuits` directory contains the KiCad projects for our circuitry.

# Future code changes
The current source files of the `pc-parser` are seperated into different source files. If new source files are added they need to be put in the list of `SOURCE_FILES` in the `CMakeLists.txt` file in the pc-parser directory so they get compiled and linked into the program.

Currently the code base only supports linux. In the `display.cc` of the `pc-parser` there's some linux-only code for making the p300 display appear on a different monitor than where the program started on / where root starts it's display. The commandline options `gengetopt` is also a linux only library that is used for the commandline option parsing. And lastly the serial port uses the linux library for setting up the serial connection which would be different on windows.

# Building Openkit-C

## Build requirements

### For Windows builds

* CMake
* Visual Studio 2017

### For Linux builds

* CMake
* gcc/clang

## Build dependencies

openkit-c has the following dependencies:

* libcurl
* zlib
* Google Test

It is necessary to build the dependencies according to instructions in the file <ROOT>/docs/building-dependencies.md
Prior to building the 3rdparty dependencies have to be available in the project subfolder named '3rdparty'

## Fetch sources

The sources of openkit-c can be obtained from the GitHub repo

```
git clone git@github.com:Dynatrace/openkit-c.git
```

## Compile for Windows in Visual Studio

* Open Visual Studio 2017

* Open -> CMake

* Navigate to the top level CMakeList.txt file in the previously checked out repository

* After operation finished project is available in Visual Studio, there is a new Tab especially for the CMake operations. Build the project by clicking on 'CMake -> Build All'

* Both Debug and Release builds are available

## Compile for Windows on Command Prompt

* Open "Developer Command Prompt for VS 2017" or a Windows Command Shell having access to "cl.exe" and "cmake.exe"

* Create a build folder and change directory into it

```
mkdir build & cd build
mkdir x86-Release & cd x86-Release
```

* Create the (NMake) makefiles for the Release build and provide a relative path to the install via the 'CMAKE_INSTALL_PREFIX'.
  Typically you want to set the parameter 'OPENKIT_FORCE_SHARED_CRT' to 'ON', e.g. if other libraries (such as Gtest) use a shared version of the run-time library.

```
cmake -G "NMake Makefiles" -DCMAKE_BUILDTYPE=Release -DCMAKE_INSTALL_PREFIX=../../../install -DOPENKIT_FORCE_SHARED_CRT=ON ../..
```

* Compile the OpenKit with NMake

```
nmake
```

* Among other files, this step will build the unittest.exe file, located in the 'bin' subfolder.
  Verify the build by executing the unittests.exe

```
bin\unittests.exe
```

* Install the OpenKit with NMake. This will create the library 'OpenKit.lib' and the required header files in the 'install' directory provided via the 'CMAKE_INSTALL_PREFIX' parameter.

```
nmake install
```

## Compile for Linux

* Open bash

* Navigate to location where the repository was previously checked out to

* Create a build folder

```
mkdir -p  build/linux-x86-release
```

* Go to new folder

```
cd build/linux-x86-release
```

* Create the makefiles for the Release build and provide a relative path to the install via the 'CMAKE_INSTALL_PREFIX'.
  Typically you want to set the parameter 'OPENKIT_FORCE_SHARED_CRT' to 'ON', e.g. if other libraries (such as Gtest) use a shared version of the run-time library.

```
cmake -G "Unix Makefiles" -DCMAKE_BUILDTYPE=Release -DCMAKE_INSTALL_PREFIX=../../../install -DOPENKIT_FORCE_SHARED_CRT=ON ../..
```

* Compile the OpenKit with make

```
make
```

* Among other files, this step will build the (executable) unittest file, located in the 'bin' subfolder.
  Verify the build by executing the unittests.

```
bin/unittests
```

* Install the OpenKit with make. This will create the library 'libOpenKit.a' and the required header files in the 'install' directory provided via the 'CMAKE_INSTALL_PREFIX' parameter.

```
make install
```

## Build a shared library

It is recommended to build the OpenKit as a static library (e.g. 'OpenKit.lib" under Windows or 'libOpenKit.a' under Linux). However, the OpenKit can also be built as a shared library (e.g. 'OpenKit.dll' under Windows or 'libOpenKit.so' under Linux).
To do so, the following steps are required (in this example, they are based on Windows, but can similar be applied to Linux):

```
cmake -G "NMake Makefiles" -DCMAKE_BUILDTYPE=Release -DCMAKE_INSTALL_PREFIX=../../../install-dll -DBUILD_SHARED_LIBS=ON ../..
```

* Compile the OpenKit with NMake

```
nmake
```

* Install the OpenKit with NMake. This will create the library 'OpenKit.dll', the respective '.lib' and the required header files in the 'install' directory provided via the 'CMAKE_INSTALL_PREFIX' parameter.

```
nmake install
```

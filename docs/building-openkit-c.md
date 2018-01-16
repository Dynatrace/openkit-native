#Building Openkit-C

##Build requirements

### For Windows builds

* Cmake
* Visual Studio 2017

### For Linux builds

* Cmake
* gcc/clang
* ninja (optional dependency)

##Build dependencies

openkit-c has the following dependencies:

* libcurl
* zlib
* googletest

It is necessary to build the dependencies according to instructions in the file <ROOT>/docs/building-dependencies.md
Prior to building the 3rdparty dependencies have to be available in the project subfolder named '3rdparty'

##Fetch sources

The sources of openkit-c can be obtained from the GitHub repo

```
git clone git@github.com:Dynatrace/openkit-c.git
```

##Compile for Windows

* Open Visual Studio 2017

* Open -> CMake

* Navigate to the top level CMakeList.txt file in the previously checked out repository

* After operation finished project is available in Visual Studio, there is a new Tab especially for the cmake operations. Build the project by clicking on 'CMake -> Build All'

* Both Debug and Release builds are available

##Compile for Linux

* Open bash

* Navigate to location where the repository was previously checked out to

* Create a build folder

```
mkdir -p  build/linux-x86
```

* Go to new folder

```
cd build/linux-x86
```

* Invoke cmake

```
cmake -G"Unix Makefiles" ../..
```

* Invoke make

```
make
```
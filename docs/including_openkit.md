# Including OpenKit C/C++ in your application build

## Prerequisites

It is assumed that you successfully built and installed OpenKit as described in [Building OpenKit C/C++][building].

## Using OpenKit with CMake

Assuming your project is also using CMake, then it's fairly easy getting OpenKit into your application.  
You only need to set or add the OpenKit's install location to `CMAKE_PREFIX_PATH`. It should be the same
location that has been used for `-DCMAKE_INSTALL_PREFIX=` as described in [Building OpenKit C/C++][building].

In your `CMakeLists.txt` you can simply use the following snippet
```cmake
# instruct CMake to search for OpenKit
find_package(OpenKit REQUIRED NO_MODULE)

# add Dynatrace::OpenKit as dependency to your own target
target_link_libraries(${TARGET_USING_OPENKIT} Dynatrace::OpenKit)
```

Adding OpenKit to your target using CMake will add necessary preprocessor, compiler and linker
directives.

### Using OpenKit features requiring CURL

Certain features, e.g. providing a custom `ISSLTrustManager` implementation require using
CURL functions in your own code. By default OpenKit only adds transitive dependencies
as link dependencies if absolutely necessary, e.g. if OpenKit is built as static library.

If your own code is using CURL features you have to add it to your own project. It's 
highly desireable to use the same CURL version as OpenKit does.

Change the snippet from above to the one below and CURL is added to your project as well.

```cmake
# force OpenKitConfig.cmake to search for the CURL dependency
set(USE_OPENKIT_CURL ON) 

# instruct CMake to search for OpenKit
find_package(OpenKit REQUIRED NO_MODULE)

# add Dynatrace::OpenKit and CURL::libcurl as dependency to your own target
target_link_libraries(${TARGET_USING_OPENKIT} Dynatrace::OpenKit)
target_link_libraries(${TARGET_USING_OPENKIT} CURL::libcurl)
```

## Using OpenKit without CMake

If your project is not using CMake it's still straightforward to add OpenKit and dependencies to your
project. Assuming you installed OpenKit to `~/OpenKit/install` then you need to add
`~/OpenKit/install/include` to your include path, and add the OpenKit library/archive located in
`~/OpenKit/install/lib` as link dependency.  
If OpenKit is built as static lib, CURL and zlib libraries need to be added as well. Furthermore,
the preprocessor directive `OPENKIT_STATIC_DEFINE` needs to be defined.

[building]: ./docs/building-openkit.md

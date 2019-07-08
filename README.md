# Dynatrace OpenKit - C/C++ Reference Implementation

 [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Build Status

| Operating System | Status  |
| ---------------- |:-------:|
| Windows          |[![Build status](https://ci.appveyor.com/api/projects/status/jdpv1bvje56f9na3/branch/master?svg=true)](https://ci.appveyor.com/project/openkitdt/openkit-native/branch/master) |
| Linux            |[![Build Status](https://travis-ci.org/Dynatrace/openkit-native.svg?branch=master)](https://travis-ci.org/Dynatrace/openkit-native) |

## What is the OpenKit?

The OpenKit provides an easy and lightweight way to get insights into applications with Dynatrace/AppMon by instrumenting the source code of those applications.

It is best suited for applications running separated from their backend and communicating via HTTP, like rich-client-applications, embedded devices, terminals, and so on.

The big advantages of the OpenKit are that it's designed to
* be as easy-to-use as possible
* be as dependency-free as possible, except for an SSL library (no Dynatrace/AppMon Agent needed)
* be easily portable to other languages and platforms

This repository contains the reference implementation in C and C++. Other implementations are listed as follows:
* Java: https://github.com/Dynatrace/openkit-java/
* .NET: https://github.com/Dynatrace/openkit-dotnet/
* JavaScript: https://github.com/Dynatrace/openkit-js/

## What you can do with the OpenKit
* Create Sessions and User Actions
* Report values, events, errors and crashes
* Trace web requests to server-side PurePaths
* Tag Sessions with a user tag
* Use it together with Dynatrace or AppMon

## What you cannot do with the OpenKit
* Create server-side PurePaths (this functionality is provided by [OneAgent SDKs](https://github.com/Dynatrace/OneAgent-SDK))
* Create metrics (use the [Custom network devices & metrics API](https://www.dynatrace.com/support/help/dynatrace-api/timeseries/what-does-the-custom-network-devices-and-metrics-api-provide/) to report metrics)

## Design Principles
* API should be as simple and easy-to-understand as possible
* Incorrect usage of the OpenKit should still lead to valid results, if possible
* In case of errors, the API should not throw exceptions, but only log those errors (in verbose mode)
* All third-party libraries, except for an SSL library, are included as source and built together with OpenKit
* Avoid usage of C++ standard newer than C++11, older compilers should work as well
* Avoid usage of compiler specific extensions to support a broad variety of C++ compilers
* Design reentrant APIs and document them

## General Remarks

* All non binary files within the repository (except for 3rd party files) are formatted with unix style (LF) line endings.

## Prerequisites

### Running the OpenKit
* Microsoft Windows
* GNU/Linux
  * OpenSSL needs to be installed

### Building the Source
* CMake 3.0 or above
* A C++11 compliant compiler
  * Visual Studio 2015/2017
  * GCC (>= 5.0.0)
  * Clang (>= 3.8.0)
* An SSL library needs to be available
  * Secure Channel (aka. Schannel) for Microsoft Windows 
  * OpenSSL for GNU/Linux

### Generating API documentation
* Doxygen needs to be available

### (Unit-)Testing the OpenKit
* Google Test and Google Mock are distributed with OpenKit

## Building the Source/Generating the Doxygen API documentation

OpenKit uses CMake (https://cmake.org) as build system.  
Using CMake it's possible to have a common build script for a broad variety
of compilers, IDEs and build tools.
A detailed description of various ways can be found in [docs/building-openkit.md](docs/building-openkit.md)

## General Concepts

In this part the concepts used throughout OpenKit are explained. A short sample how to use OpenKit is
also provided. For detailed code samples have a look into [example.md](docs/example.md).

### String Handling in OpenKit

All strings to OpenKit's C and C++ API are passed as NULL terminated `const char*`.  
Internally OpenKit makes a copy of the passed parameters. Therefore developers using
OpenKit C/C++ do not need to care about the lifetime of the passed arguments.

The passed `const char*` can be encoded in UTF-8 if characters outside the ASCII range are needed.  
Using UTF-8 encoding has the advantage that all ASCII strings can be used without encoding them.
If characters outside the ASCII range are used (e.g. German Umlaut characters), the
developer using OpenKit's API is responsible for appropriate encoding.  
When OpenKit encounters invalid characters in the passed `const char*` they are
replaced using the Unicode Replacement Character (`U+FFFD`) in the internal copy.

### DynatraceOpenKitBuilder / AppMonOpenKitBuilder
A `DynatraceOpenKitBuilder`/`AppMonOpenKitBuilder` instance is responsible for setting 
application relevant information, e.g. the application's version and device specific information, and to create
an `IOpenKit` instance.

### OpenKit
 
An `IOpenKit` instance is responsible for creating user sessions (see `ISession`).
  
Although it would be possible to have multiple `IOpenKit` instances connected to the same endpoint
(Dynatrace/AppMon) within one process, there should be one unique instance. `IOpenKit` is designed to be
thread safe and therefore the instance can be shared among threads.  

On application shutdown, `shutdown()` needs to be called on the `IOpenKit` instance.

### Session

An `ISession` represents kind of a user session, similar to a browser session in a web application.
However the application developer is free to choose how to treat an `ISession`.  
The `ISession` is used to create `IRootAction` instances, report application crashes and can be used for tracing web requests.

When an `ISession` is no longer required, it's highly recommended to end it, using the `ISession::end()` method. 

### RootAction and Action

The `IRootAction` and `IAction` are named hierarchical nodes for timing and attaching further details.
An `IRootAction` is created from the `ISession` and it can create `IAction` instances.
Both, `IRootAction` and `IAction`, provide the possibility to attach key-value pairs, named events and errors, and can be used 
for tracing web requests.

When an `IRootAction` or `IAction` is no longer required, it's highly recommended to close it, using the `IAction::leaveAction()` or
`IRootAction::leaveAction()` method.

### WebRequestTracer

When the application developer wants to trace a web request, which is served by a service 
instrumented by Dynatrace, an `IWebRequestTracer` should be used, which can be
requested from an `ISession` or an `IAction`.

### Named Events

A named `Event` is attached to an `IAction` and contains a name.

### Key-Value Pairs

For an `IAction` key-value pairs can also be reported. The key is always a UTF-8 encoded String (`const char*`)
and the value may be an Integer (`int32_t`), a floating point (`double`) or a UTF-8 encoded String (`const char*`).

### Errors & Crashes

Errors are a way to report an erroneous condition on an `IAction`.  
Crashes are used to report (unhandled) exceptions on an `ISession`.

### Identify Users

OpenKit enables you to tag sessions with unique user tags. The user tag is an UTF-8 encoded String 
(`const char*`) that allows to uniquely identify a single user.

### GDPR Compliance

When creating an `OpenKit` instance, it is also possible to set the GDPR compliant mode
where you can specify which data is collected.
For detailed description and samples refer to [example.md](docs/example.md).
Getting user consent must be handled within the application itself.

## Example

This small example provides a rough overview how OpenKit can be used.  
Detailed explanation is available in [example.md](docs/example.md).

```c++
const char* applicationName = "My OpenKit application";
const char* applicationID = "application-id";
uint64_t deviceID = 42;
const char* endpointURL = "https://tenantid.beaconurl.com/mbeacon";

std::shared_ptr<openkit::IOpenKit> openKit = 
    DynatraceOpenKitBuilder(endpointURL, applicationID, deviceID)
    .withApplicationName(applicationName)
    .withApplicationVersion("1.0.0.0")
    .withOperatingSystem("Windows 10")
    .withManufacturer("MyCompany")
    .withModelID("MyModelID")
    .build();

const char* clientIP = "8.8.8.8";
std::shared_ptr<openkit::ISession> session = openKit->createSession(clientIP);

session->identifyUser("jane.doe@example.com");

const char* rootActionName = "rootActionName";
RootAction rootAction = session->enterAction(rootActionName);

const char* childActionName = "childAction";
std::shared_ptr<IAction> childAction = rootAction->enterAction(childActionName);

childAction->leaveAction();
rootAction->leaveAction();
session->end();
openKit->shutdown();
``` 

## Necessary defines when using OpenKit

When building an application which is using OpenKit, it's necessary
to set the following preprocessor definitions if OpenKit was previously built
as static library.
* CURL_STATICLIB
* OPENKIT_STATIC_DEFINE

If OpenKit is used as shared library (DLL/SO), those defines must not be set,
otherwise the required import directives will not be used.

## Included 3rd party libraries
OpenKit includes the following 3rd party libraries
* zlib 1.2.11 [3rdparty/zlib-1.2.11/README](3rdparty/zlib-1.2.11/README)
* libcurl 7.59.0 [3rdparty/curl-7.59.0/README](3rdparty/curl-7.59.0/README)
* Google Test & Google Mock 1.8.0 [3rdparty/googletest-release-1.8.0/README.md](3rdparty/googletest-release-1.8.0/README.md)
 

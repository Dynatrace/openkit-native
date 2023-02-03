# Dynatrace OpenKit - C/C++ Reference Implementation

 [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
 ![Build Status](https://github.com/Dynatrace/openkit-native/actions/workflows/windows.yml/badge.svg)
 ![Build Status](https://github.com/Dynatrace/openkit-native/actions/workflows/linux.yml/badge.svg)

:information_source: We changed the default branch name to `main`. You can find the necessary steps to update your local clone on [Scott Hanselman's Blog](https://www.hanselman.com/blog/EasilyRenameYourGitDefaultBranchFromMasterToMain.aspx).  
We encourage you to rename the default branch in your forks too.

## What is the OpenKit?

The OpenKit provides an easy and lightweight way to get insights into applications with Dynatrace by instrumenting the source code of those applications.

It is best suited for applications running separated from their backend and communicating via HTTP, like rich-client-applications, embedded devices, terminals, and so on.

The big advantages of the OpenKit are that it's designed to
* be as easy-to-use as possible
* be as dependency-free as possible, except for an SSL library (no Dynatrace Agent needed)
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
* Use it together with Dynatrace

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

## Getting started
* [Installing and updating OpenKit C/C++][installing]
* [Building OpenKit C/C++][building]
* [Including OpenKit C/C++ in your application build][including_openkit]
* [General concepts][concepts]
* [Detailed example][example]
* [Contributing to OpenKit C/C++][contributing]
* [Supported versions][supported_versions]
* [Upgrade guide][upgrade_guide]

## Advanced topics
* [OpenKit C/C++ internals][internals]

## Included 3rd party libraries
OpenKit includes the following 3rd party libraries.

| Library                     | Version | License                        | Purpose                                        |
|-----------------------------|---------|--------------------------------|------------------------------------------------|
| [zlib][zlib-readme]         | 1.2.11  | [zlib][zlib-readme]            | Used for compressing transmitted data          |
| [libcurl][curl-readme]      | 7.75.0  | [curl][curl-license]           | Used for communicating with Dynatrace   |
| [Google Test][gtest-readme] | 1.10.0   | [BSD 3-clauses][gtest-license] | Unit testing framework, not used in production |
| [Google Mock][gmock-readme] | 1.10.0   | [BSD 3-clauses][gmock-license] | Mocking framework, not used in production      |

[installing]: ./docs/installing.md
[concepts]: ./docs/concepts.md
[example]: ./docs/example.md
[contributing]: ./CONTRIBUTING.md
[supported_versions]: ./docs/supported_versions.md
[upgrade_guide]: ./docs/upgrade_guide.md
[including_openkit]: ./docs/including_openkit.md
[building]: ./docs/building-openkit.md
[internals]: ./docs/internals.md
[zlib-readme]: ./3rdparty/zlib-1.2.11/README
[curl-readme]: ./3rdparty/curl-7.59.0/README
[curl-license]: ./3rdparty/curl-7.59.0/COPYING
[gtest-readme]: ./3rdparty/googletest-release-1.10.0/googletest/README.md
[gtest-license]: ./3rdparty/googletest-release-1.10.0/googletest/LICENSE
[gmock-readme]: ./3rdparty/googletest-release-1.10.0/googlemock/README.md
[gmock-license]: ./3rdparty/googletest-release-1.10.0/googlemock/LICENSE

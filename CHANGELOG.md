# OpenKit Native ( C/C++ ) Changelog

## [Unreleased](https://github.com/Dynatrace/openkit-native/compare/v1.1.0...HEAD)

### Changed
- Response code is now a parameter of IWebRequestTracer's stop function
  (stopWebRequestWithResponseCode in C API).
  Existing functions for stopping and setting the response code have been deprecated.
- Fixed Unix compile issue regarding zlib.h in CMakeLists.txt file
- Sleep calls in BeaconSender are interruptible to ensure OpenKit can be shutdown in time
- OpenKit version is parsed from version.properties file
- Application ID and Device ID are correctly encoded for special characters
  The encoding is a percent-encoding based on RFC 3986 with additional encoding of underscore characters.
- Remove "Enable minimal rebuild" compiler flag (MSVC only)
  In recent MSVC compilers this flag got marked as deprecated
- BeaconSender thread was stopped too early and did not flush sessions
- Fixed problem with infinite time sync requests
  This problem occurred mainly in AppMon settings.
- OpenKit::createSession method accepts nullptr as IP address
  In case nullptr is passed, the IP is determined on the server side.
- Reduce warnings when building on Linux

## 1.1.0 [Release date: 2018-10-25]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.1.0)

### Added
- Device ID can be specified as String in addition to long
  This allows to send UUIDs or other specifiers
- Standalone web request tagging
  A WebRequestTracer can also be obtained from a Session

## 1.0.0 [Release date: 2018-10-25]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.0)

### Initial public release
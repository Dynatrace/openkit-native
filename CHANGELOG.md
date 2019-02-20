# OpenKit Native ( C/C++ ) Changelog

## [Unreleased](https://github.com/Dynatrace/openkit-native/compare/v1.1.3...release/1.1)

## 1.1.3 [Release date: 2018-12-20]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.1.3)

### Changed
- Fixed problem with infinite time sync requests  
  This problem occurred mainly in AppMon settings.

## 1.1.2 [Release date: 2018-12-20]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.1.2)

### Changed
- Application ID and Device ID are correctly encoded for special characters
  The encoding is a percent-encoding based on RFC 3986 with additional encoding of underscore characters.
- Remove "Enable minimal rebuild" compiler flag (MSVC only)
  In recent MSVC compilers this flag got marked as deprecated
- BeaconSender thread was stopped too early and did not flush sessions

## 1.1.1 [Release date: 2018-11-12]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.1.1)

### Changed
- Sleep calls in BeaconSender are interruptible to ensure OpenKit can be shutdown in time
- OpenKit version is parsed from version.properties file

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
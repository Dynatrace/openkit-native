# OpenKit Native ( C/C++ ) Changelog

## [Unreleased](https://github.com/Dynatrace/openkit-native/compare/v1.0.5...release/1.0)

## 1.0.5 [Release date: 2020-04-06]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.5)

### Changed
- Fix incorrect check in C-API's useBeaconCacheBehaviorForConfiguration function when setting the
  upper memory bound.
- Fix compiler errors for certain Visual Studio versions
- Fix `leaveAction` in `RootAction` so that the `mEndTime` and `mEndSequenceNumber` is set before 
  the root action is added to the `Beacon`.
- Fix `enterAction` for a ended/left `RootAction` to return a `NullAction` with the `RootAction` as parent instead of
  a `NullAction` without parent.
- Fixed issue with reporting localized beacon data  
  Integer and floating point numbers might have been reported with the global locale
  if `std::locale::global` was called before. This might have an impact on
  session properties.

## 1.0.4 [Release date: 2019-03-14]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.4)

### Changed
- OpenKit::createSession method accepts nullptr as IP address
  In case nullptr is passed, the IP is determined on the server side.
- Reduce warnings when building on Linux

## 1.0.3 [Release date: 2019-02-20]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.3)

### Changed
- Fixed problem with infinite time sync requests
  This problem occurred mainly in AppMon settings.

## 1.0.2 [Release date: 2018-12-20]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.2)

### Changed
- Application ID and Device ID are correctly encoded for special characters
  The encoding is a percent-encoding based on RFC 3986 with additional encoding of underscore characters.
- Remove "Enable minimal rebuild" compiler flag (MSVC only)
  In recent MSVC compilers this flag got marked as deprecated
- BeaconSender thread was stopped too early and did not flush sessions


## 1.0.1 [Release date: 2018-11-12]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.1)

### Changed
- Sleep calls in BeaconSender are interruptible to ensure OpenKit can be shutdown in time
- OpenKit version is parsed from version.properties file

## 1.0.0 [Release date: 2018-10-25]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v1.0.0)

### Initial public release
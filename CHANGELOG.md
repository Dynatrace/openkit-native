# OpenKit Native ( C/C++ ) Changelog

## [Unreleased](https://github.com/Dynatrace/openkit-native/compare/v2.1.0...HEAD)

## 2.1.0 [Release date: 2021-10-07]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v2.1.0)

### Added
- Overloaded `IAction::reportError(const char*, int32_t)` method for reporting an integer error code without reason.  
  The old `IAction::reportError(const char*, int32_t, const char*)` has been deprecated in favor of the new one.
- Overloaded `IRootAction::reportError(const char*, int32_t)` method for reporting an integer error code without reason.  
  The old `IRootAction::reportError(const char*, int32_t, const char*)` has been deprecated in favor of the new one.
- Overloaded `IAction::reportError(const char*, const char* , const char*, const char*)` method for reporting generic errors.
- Overloaded `IRootAction::reportError(const char*, const char* , const char*, const char*)` method for reporting generic errors.
- Added function `reportErrorCodeOnRootAction(struct RootActionHandle*, const char*, int32_t)` for reporting an integer error code without reason.  
  The old function `reportErrorOnRootAction(struct RootActionHandle*, const char*, int32_t, const char*)` has been deprecated in favor of the new one.
- Added function `reportErrorCodeOnAction(struct ActionHandle*, const char*, int32_t)` for reporting an integer error code without reason.  
  The old function `reportErrorOnAction(struct ActionHandle*, const char*, int32_t, const char*)` has been deprecated in favor of the new one.
- Added new functions
  * `reportErrorCauseOnRootAction(struct RootActionHandle*, const char*, const char*, const char*, const char*)`
  * `reportErrorCauseOnAction(struct ActionHandle*, const char*, const char*, const char*, const char*)`
  for reporting generic errors on root actions and actions.
- Support cost control configured in the Dynatrace UI.
- Add libatomic as additional link library if required.
- Requests to Dynatrace can be intercepted to add custom HTTP headers.
- It is possible to get the duration from an `IAction` and `IRootAction`.
- `IAction` and `IRootAction` can be canceled.  
  Canceling an `IAction` or `IRootAction` is similar to leaving it, without reporting it.

### Changed
- Fix issue in JSON parser for compilers/platforms with unsigned char.

## 2.0.0 [Release date: 2021-05-19]
[GitHub Releases](https://github.com/Dynatrace/openkit-native/releases/tag/v2.0.0)

### Added
- Support for parsing JSON
- Technology type support for errors and crashes
- Support for session splitting. Sessions are split transparently after either the maximum session duration,
  the idle timeout or the number of top level actions are exceeded. Session splitting is only applicable,
  if it is also supported by Dynatrace. The internal details are described [here](./docs/internals.md#session-splitting).
- Re-apply user tag on split sessions.
- Support for reporting 64-bit integer values.

### Security
- Support for modified UTF-8 terminated strings.

### Changed
- On OpenKitBuilder creation device ID is parsed from the given string. Non-numeric
  device IDs are hashed to a corresponding numeric value. Internally a numeric
  type is used for the device ID.
- Fix incorrect check in C-API's useBeaconCacheBehaviorForConfiguration function when setting the
  upper memory bound.
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
- Fix compiler errors for certain Visual Studio versions
- Fix `leaveAction` in `RootAction` so that the `mEndTime` and `mEndSequenceNumber` is set before 
  the root action is added to the `Beacon`.
- Add IOpenKit::createSession overload without IP address parameter.  
  The IP address is determined in this case on the server side.
- Reporting a crash causes a session split, which is transparently handled.
- Provide a more reliable way to determine monotonic timestamps.
- Only send end session events when sessions are explicitly ended via API method `ISession::end()`.
- Fix potential endless loop in beacon sending, when lots of data
  is generated in a short period of time.
- Minimum required CMake version pumped to 3.4.0
- Improved CMake install target
  It is possible to use CMake's `find_package` to search for OpenKit.

### Improvements
- Reformatted text files to unix style line endings (excluding 3rd party files)

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
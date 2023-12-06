# Upgrade guide for OpenKit C/C++

## ## OpenKit .C/C++ 3.1 to 3.2
Be aware that some requirements (CMake, GCC, Clang) for the build have been changed, see [required programs][programs].

## ## OpenKit .C/C++ 3.0 to 3.1
There are no breaking API changes and upgrading is straightforward, by [updating][update] the library
to the latest 3.1 release.

## ## OpenKit .C/C++ 2.1 to 3.0
Appmon has been removed from OpenKit C/C++. If you don't want to replace your AppMon related code stay on the latest 2.1.x release.

### Removed API
* `AbstractOpenKitBuilder` has been removed as it was not needed anymore due to AppMon removal. All functionalities have been consolidated into the `DynatraceOpenKitBuilder`.
* `IWebRequestTracer::setResponseCode(int32_t responseCodee)` and `IWebRequestTracer::stop()`  
  Use `IWebRequestTracer::stop(int32_t responseCode)` instead as replacement.
* `setResponseCode(struct WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)` and `stopWebRequest(struct WebRequestTracerHandle* webRequestTracerHandle)`  
  Use `stopWebRequest(struct WebRequestTracerHandle*, int32_t)` instead as replacement.
* `IAction::reportError(const char* errorName, int32_t errorCode, const char* reason)`  
  Use `IAction::reportError(const char* errorName, int32_t errorCode)` instead, since reason is unhandled.
* `reportErrorOnAction(struct ActionHandle* actionHandle, const char* errorName, int32_t errorCode, const char* reason)`  
  Use `reportErrorCodeOnAction(struct RootActionHandle*, const char*, int32_t)` instead, since reason is unhandled.
* `IRootAction::reportError(const char* errorName, int32_t errorCode, const char* reason)`  
  Use `IRootAction::reportError(const char* errorName, int32_t errorCode)` instead, since reason is unhandled.
* `reportErrorOnRootAction(struct RootActionHandle* rootActionHandle, const char* errorName, int32_t errorCode, const char* reason)`  
  Use `reportErrorCodeOnRootAction(struct RootActionHandle*, const char*, int32_t)` instead, since reason is unhandled.

## ## OpenKit .C/C++ 2.0 to 2.1
There are no breaking API changes and upgrading is straightforward, by [updating][update] the library
to the latest 2.1 release.

### Deprecated API
* `IAction::reportError(const char* errorName, int32_t errorCode, const char* reason)`  
  Use `IAction::reportError(const char* errorName, int32_t errorCode)` instead, since reason is unhandled.
* `IRootAction::reportError(const char* errorName, int32_t errorCode, const char* reason)`  
  Use `IRootAction::reportError(const char* errorName, int32_t errorCode)` instead, since reason is unhandled.

## ## OpenKit .C/C++ 1.1 to 2.0
There are breaking changes. All OpenKit headers have been moved into one
single place, `include/OpenKit` to be exact. It might be necessary to adjust the
include path in your build scripts and `#include` directives in your source code.  
Besides that, the API method signature did not changed.  
The minimum required CMake version has been raised to 3.4.0.

### Deprecated API
* `IWebRequestTracer::setResponseCode(int32_t responseCodee)` and `IWebRequestTracer::stop()`  
  Use `IWebRequestTracer::stop(int32_t responseCode)` instead as replacement.
* `DynatraceOpenKitBuilder::withApplicationName(const char* applicationName)`  
  The application name is configured in Dynatrace Web UI.
* `AbstractOpenKitBuilder::enableVerbose()`  
  Use `AbstractOpenKitBuilder::withLogLevel(LogLevel::LOG_LEVEL_DEBUG)` instead.
* `void stopWebRequest(struct WebRequestTracerHandle* webRequestTracerHandle)` and `void setResponseCode(struct WebRequestTracerHandle* webRequestTracerHandle, int32_t responseCode)`  
  Use `void stopWebRequestWithResponseCode(struct WebRequestTracerHandle* webRequestTracerhandle, int32_t responseCode)` instead.

## OpenKit .C/C++ 1.0 to 1.1
There are no breaking API changes and upgrading is straightforward, by [updating][update] the library
to the latest 1.1 release.

### Deprecated API
* `DynatraceOpenKitBuilder::DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, const char* deviceID)`  
   Use `DynatraceOpenKitBuilder::DynatraceOpenKitBuilder(const char* endpointURL, const char* applicationID, int64_t deviceID)` instead.

[update]: ./installing.md#obtaining-and-updating-openkit-native
[programs]: ./building-openkit.md#required-programs

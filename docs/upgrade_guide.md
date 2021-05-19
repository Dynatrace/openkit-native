# Upgrade guide for OpenKit C/C++

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
* `AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, const char* deviceID)`  
   Use `AppMonOpenKitBuilder::AppMonOpenKitBuilder(const char* endpointURL, const char* applicationName, int64_t deviceID)` instead.

[update]: ./installing.md#obtaining-and-updating-openkit-native

# Dynatrace OpenKit - C/C++ Example

The following document provides an in depth overview, how OpenKit can be used from
developer's point of view. It explains the usage of all the C and C++ API methods.

:grey_exclamation: Be aware that mixing the C and C++ API is not possible.
The developer is supposed to stick to one of the two APIs depending on the requirements.

## Obtaining an OpenKit Instance

Depending on the backend a new OpenKit instance can be obtained by using either `DynatraceOpenKitBuilder` 
or `AppMonOpenKitBuilder` when using the C++ API. Despite from this, the developer does not need to distinguish between 
different backend systems.

When using the OpenKit C API a new OpenKit instance can be obtained by calling either the `createDynatraceOpenKit` 
or the `createAppMonOpenKit` function. Despite from this, the developer does not need to distinguish between 
different backend systems.

### Dynatrace

For Dynatrace SaaS and Dynatrace Managed the `DynatraceOpenKitBuilder` (C++ API) is used to build new OpenKit instances.

```c++
// C++ API
const char* applicationID = "application-id";
uint64_t deviceID = 42;
const char* endpointURL = "https://tenantid.beaconurl.com/mbeacon";

std::shared_ptr<openkit::IOpenKit> openKit = DynatraceOpenKitBuilder(endpointURL, applicationID, deviceID).build();
```

* The `endpointURL` denotes the Dynatrace endpoint OpenKit communicates with and 
  is shown when creating the application in Dynatrace. The endpoint URL can be found 
  in the settings page of the custom application in Dynatrace.
* The `applicationID` parameter is the unique identifier of the application in Dynatrace Saas. The
  application's id can be found in the settings page of the custom application in Dynatrace.
* The `deviceID` is a unique identifier, which might be used to uniquely identify a device.

When using the OpenKit C API the three parameters described above are used to create an `OpenKitConfiguration` handle.
The DynatraceOpenKit is built using this handle. All parameters described in the Optional Configuration
can be applied to the configuration. The OpenKitConfiguration handle is owned by the caller and needs to be cleared after
creating the DyntraceOpenKit.

```c
// C API
const char* applicationID = "application-id";
int64_t deviceID = 42;
const char* endpointURL = "https://tenantid.beaconurl.com/mbeacon";

struct OpenKitConfigurationHandle* configurationHandle = createOpenKitConfiguration(beaconURL, applicationID, serverID);

struct OpenKitHandle* openKitHandle = createDynatraceOpenKit(configurationHandle);
destroyOpenKitConfiguration(configurationHandle);

```

:grey_exclamation: For Dynatrace Managed the endpoint URL looks a bit different.


### AppMon

An OpenKit instance for AppMon can be obtained by using the `AppMonOpenKitBuilder` (C++ API).

```c++
const char* applicationName = "My OpenKit application";
int64_t deviceID = 42;
const char* endpointURL = "https://beaconurl.com/dynaTraceMonitor";

std::shared_ptr<openkit::IOpenKit> openKit = AppMonOpenKitBuilder(endpointURL, applicationName, deviceID).build();
```

* The `endpointURL` denotes the AppMon endpoint OpenKit communicates with.
* The `applicationName` parameter is the application's name in AppMon and is also used as the application's id.
* The `deviceID` is a unique identifier, which might be used to uniquely identify a device.

When using the OpenKit C API the three parameters described above are used to create an `OpenKitConfiguration` handle.
The AppMonOpenKit is built using this handle. All parameters described in the Optional Configuration
can be applied to the configuration object.  The OpenKitConfiguration handle is owned by the caller and needs to be cleared after
creating the AppMonOpenKit.

```c
// C API
const char* applicationID = "application-id";
int64_t deviceID = 42;
const char* endpointURL = "https://tenantid.beaconurl.com/mbeacon";

struct OpenKitConfigurationHandle* configurationHandle = createOpenKitConfiguration(beaconURL, applicationID, serverID);

struct OpenKitHandle* openKitHandle = createAppMonOpenKit(configurationHandle);
destroyOpenKitConfiguration(configurationHandle);

```

### Optional Configuration

In addition to the mandatory parameters described above, both the `DynatraceOpenKitBuilder` and the `AppMonOpenKitBuilder` 
provide additional methods to further  customize OpenKit.
This includes device specific information like operating system, manufacturer, or model id.  

| Method Name | Description | Default Value |
| ------------- | ------------- | ---------- |
| `withApplicationVersion`  | sets the application version  | `"2.2.0"` |
| `withOperatingSystem`  | sets the operating system name | `"OpenKit 2.2.0"` |
| `withManufacturer`  | sets the manufacturer | `"Dynatrace"` |
| `withModelID`  | sets the model id  | `"OpenKitDevice"` |
| `withBeaconCacheMaxRecordAge`  | sets the maximum age of an entry in the beacon cache in milliseconds | 45 min |
| `withBeaconCacheLowerMemoryBoundary`  | sets the lower memory boundary of the beacon cache in bytes  | 80 MB |
| `withBeaconCacheUpperMemoryBoundary`  |  sets the upper memory boundary of the beacon cache in bytes | 100 MB |
| `withDataCollectionLevel` | sets the data collection level (enum DataCollectionLevel) | USER_BEHAVIOR |
| `withCrashReportingLevel` | sets the crash reporting level (enum CrashReportingLevel) | OPT_IN_CRASHES |
| `withTrustManager` | sets a custom `ISSLTrustManager` instance, replacing the builtin default instance.<br>Details are described in section [SSL/TLS Security in OpenKit](#ssltls-security-in-openkit). | `SSLStrictTrustManager` |
| `enableVerbose`  | *Deprecated*, use `withLogLevel` instead.<br>Enables extended log output for OpenKit if the default logger is used.<br>Is equivalent to `withLogLevel(LogLevel.DEBUG)`. | `false` |
| `withLogLevel` | sets the log level if the default logger is used | `LogLevel.WARN` |
| `withLogger` | sets a custom logger, replacing the builtin default logger.<br>Details are described in section [Logging](#logging). | `DefaultLogger` |
| `withHttpRequestInterceptor` | sets a custom `IHttpRequestInterceptor` instance,  replacing the builtin default one.<br>Details are described in section [Intercepting HTTP traffic to Dynatrace/AppMon](#intercepting-http-traffic-to-dynatraceappmon). | `NullHttpRequestInterceptor` |
| `withHttpResponseInterceptor` | sets a custom `IHttpResponseInterceptor` instance,  replacing the builtin default one.<br>Details are described in section [Intercepting HTTP traffic to Dynatrace/AppMon](#intercepting-http-traffic-to-dynatraceappmon). | `NullHttpResponseInterceptor` |

When using the OpenKit C API, additional configuration can applied to the configuration created with the
'createOpenKitConfiguration' function.

| Parameter Name | Description | Default Value |
| ------------- | ------------- | ---------- |
| `useApplicationVersionForConfiguration`  | sets the application version  | `"2.2.0"` is used when argument is `NULL` |
| `useOperatingSystemForConfiguration`  | sets the operating system name | `"OpenKit 2.2.0"` is used when argument is `NULL` |
| `useManufacturerForConfiguration`  | sets the manufacturer | `"Dynatrace"` is used when argument is `NULL` |
| `useModelIDForConfiguration`  | sets the model id  | `"OpenKitDevice"` is used when argument is `NULL` |
| `useBeaconCacheBehaviorForConfiguration` | sets caching behavior for the beacon cache              | 45 min retention, 80 MB lower memory boundary, 100 MB upper memory boundary |
| `useLoggerForConfiguration` | sets a custom logger | A default logger, logging to stdout, is used as fallback |
| `useTrustModeForConfiguration`           | sets a custom `ISSLTrustManager` replacing previous one | `STRICT_TRUST` |
| `useDataCollectionLevelForConfiguration` | sets the data collection level (enum DataCollectionLevel) | USER_BEHAVIOR |
| `useCrashReportingLevelForConfiguration` | sets the crash reporting level (enum CrashReportingLevel) | OPT_IN_CRASHES |
| `useHttpRequestInterceptorForConfiguration` | sets a custom HTTP request interceptor function,  replacing the builtin default one.<br>Details are described in section [Intercepting HTTP traffic to Dynatrace/AppMon](#intercepting-http-traffic-to-dynatraceappmon). | `NullHttpRequestInterceptor` |
| `useHttpResponseInterceptorForConfiguration` | sets a custom HTTP response interceptor function,  replacing the builtin default one.<br>Details are described in section [Intercepting HTTP traffic to Dynatrace/AppMon](#intercepting-http-traffic-to-dynatraceappmon). | `NullHttpResponseInterceptor` |

When passing a non-NULL `logger`, custom logging can be enabled. Further information is described in [Logging](#logging).
When passing a non-NULL `trustManagerHandle`, custom SSL/TLS certificate verification can be enabled.
Further details are described in [SSL/TLS Security in OpenKit](#ssltls-security-in-openkit).


:grey_exclamation: Please refer to the the Doxygen API documentation for more information regarding possible configuration values.

## SSL/TLS Security in OpenKit

All OpenKit communication to the backend happens via HTTPS (TLS/SSL based on libcurl included in 3rdparty folder).
By default OpenKit expects valid server certificates.
However it is possible, if really needed, to bypass TLS/SSL certificate validation. This can be achieved by
passing an implementation of `SSLTrustManager` by calling the `withTrustManager` on the builder (C++ API).

For OpenKit C API it is possible to bypass the TLS/SSL certificate validation by passing a custom `trustManagerHandle`
to the `createDynatraceOpenKit` or `createAppMonOpenKit` functions.

:warning: We do **NOT** recommend bypassing TLS/SSL server certificate validation, since this allows
man-in-the-middle attacks.

## Intercepting HTTP traffic to Dynatrace/AppMon

When routing traffic through own network infrastructure it might be necessary  to intercept HTTP traffic
to Dynatrace/AppMon and add or overwrite HTTP headers. This can be achieved by implementing the 
`IHttpRequestInterceptor` interface and passing an instance to the builder by calling 
the `withHttpRequestInterceptor` method. OpenKit invokes the `IHttpRequestInterceptor::intercept(IHttpRequest&)` 
method for each request sent to Dynatrace/AppMon.  
It might be required to intercept the HTTP response and read custom response headers. This
can be achieved by implementing the `IHttpResponseInterceptor` interface and passing an instance to the builder
by calling `withHttpResponseInterceptor`. OpenKit calls the `IHttpResponseInterceptor::intercept(const IHttpResponse&)`
for each HTTP response received from the backend.

When using the OpenKit C API request and response interception works by invoking user provided functions.
To set a custom HTTP request interceptor use the function `useHttpRequestInterceptorForConfiguration`.
If a custom HTTP response interceptor is needed, then the function `useHttpResponseInterceptorForConfiguration`
can be used to set one.

## Logging

By default, OpenKit uses a logger implementation that logs to stdout. If the default logger is used, verbose 
logging can be enabled by calling `enableVerbose` in the builder. By enabling verbose mode, info and debug
messages are logged. This only applies to the C++ API.

A custom logger can be set by calling `withLogger` in the builder. When a custom logger is used, a call to 
`withLogLevel` has no effect. In that case, debug and info logs are logged depending on the values returned 
in `isDebugEnabled` and `isInfoEnabled`.

When using the OpenKit C API a custom logger can be set by invoking `createLogger` function to create one.
The returned value can then be passed to the `createDynatraceOpenKit` or `createAppMonOpenKit` functions.
After OpenKit has been shut down, the custom logger shall be destroyed by invoking the `destroyLogger` function.

## Initializing OpenKit

When obtaining an OpenKit instance from the OpenKit builder the instance starts an automatic 
initialization phase. By default, initialization is performed asynchronously. 

There might be situations when a developer wants to ensure that initialization is completed before proceeding with 
the program logic. For example, short-lived applications where a valid init and shutdown cannot be guaranteed. In
such a case `waitForInitCompletion` can be used.

```c++
// C++ API
// wait until the OpenKit instance is fully initialized
bool success = openKit->waitForInitCompletion();
```

:grey_exclamation: Please refer to the Doxygen API documentation for additional information.

The method `waitForInitCompletion` blocks the calling thread until OpenKit is initialized. In case
of misconfiguration this might block the calling thread indefinitely. The return value
indicates whether the OpenKit instance has been initialized or `shutdown` has been called meanwhile.    
An overloaded method exists to wait a given amount of time for OpenKit to initialize as shown in the
following example.
```c++
// C++ API
// wait up to 10 seconds for OpenKit to complete initialization
int64_t timeoutInMilliseconds = 10 * 1000;
bool success = openKit->waitForInitCompletion(timeoutInMilliseconds);
```

The method returns `false` in case the timeout expired or `shutdown` has been invoked in the meantime
and `true` to indicate successful initialization.  

To verify if OpenKit has been initialized, use the `isInitialized` method as shown in the example below.
```c++
bool isInitialized = openKit.isInitialized();
if (isInitialized)
{
    std::cout << "OpenKit is initialized" << std::endl;
}
else
{
    std::cout << "OpenKit is not yet initialized" << std::endl;
}
```

Both ways to wait for OpenKit's initialization are also available for OpenKit C.
```c
// C API
// wait until the OpenKit instance is fully initialized
bool success = waitForInitCompletion(openKit);
```

```c
// C API
// wait up to 10 seconds for OpenKit to complete initialization
int64_t timeoutInMilliseconds = 10 * 1000;
bool success = waitForInitCompletionWithTimeout(openKit, timeoutInMilliseconds);
```

## Creating a Session

After obtaining an `IOpenKit` instance from a builder,
an `ISession` can be created by invoking the `createSession` method.  
This method takes an IP address as string argument, which might be a valid IPv4 or IPv6 address.
If the argument is not a valid IP address (including `nullptr`) a reasonable default value is used.


The example shows how to create sessions.
```c++
// C++ API
// create a session and pass an IP address
const char* clientIPAddress = "12.34.56.78";
std::shared_ptr<openkit::ISession> sessionWithIP = openKit->createSession(clientIPAddress);

// create a session and let the IP be assigned on the server side
std::shared_ptr<openkit::ISession> sessionWithNoIP = openKit->createSession();
```

The following example shows how to create sessions in C.
```c
// C API
// create a session and pass an IP address
const char* clientIPAddress = "12.34.56.78";
struct SessionHandle* sessionWithIP = createSession(openKit, clientIPAddress);

// create a session and let the IP be assigned on the server side
struct SessionHandle* sessionWithNoIP = createSessionWithAutoIpDetermination(openKit);
```

## Identify User

Users can be identified by calling `identifyUser` on an `ISession` instance. This enables you to search and 
filter specific user sessions and analyze individual user behavior over time in the backend.

```c++
// C++ API
session->identifyUser("jane.doe@example.com");
```

With OpenKit's C API the same can be achieved by calling the `identifyUser` user function.

```c
// C API
identifyUser(session, "jane.doe@example.com");
```

## Finishing a Session

When an `ISession` is no longer needed, it should be ended by invoking the `end` method.  
Although all open sessions are automatically ended when OpenKit is shut down (see "Terminating the OpenKit instance")
it's highly recommended to end sessions which are no longer in use manually.

Due to C++ nature, it's also advisable to reset the shared pointer by assigning nullptr  to avoid
memory leaks.

```c++
// C++ API
session->end();
session = nullptr; // reset the shared pointer and decrease the reference count
```

When using the C API a session must be ended by calling `endSession`. This function
frees all allocated resources and therefore the `SessionHandle` is no longer valid
afterwards.

```c
// C API
endSession(session);
// session now points to an invalid memory location,
// therefore it's beneficial to assign NULL
session = NULL;
```


## Reporting a Crash

Unexpected application crashes can be reported via an `ISession` by invoking the `reportCrash` method.  
The example below shows how an exception might be reported.
```c++
// C++ API
    static int div(int numerator, int denominator)
    {
        if (denominator == 0)
        {
            throw std::runtime_error("division by zero")_;
        }
        return numerator / denominator;
    }

    static void divWithCrash()
    {
        int numerator = 5;
        int denominator = 0;
        try
        {
            std::cout << "Got: " << div(numerator, denominator) << std::endl;
        }
        catch (std::runtime_error& e)
        {
            const char* errorName = "std::runtime_error";
            String reason = e.what();
            const char* stacktrace = nullptr; // or use a real one
            // and now report the application crash via the session
            session->reportCrash(errorName, reason, stacktrace);
        }
    }
```

OpenKit's C API also allows reporting crashes as demonstrated in the snippet below.
```c
// C API
const char* errorName = "some error name";
const char* reason = "some reason";
const char* stacktrace = ""; // or use a real stack trace

reportCrash(session, errorName, reason, stacktrace);
```


## Starting a RootAction

As mentioned in the [README](#../README.md) root actions and actions are hierarchical named events, where
an `IRootAction` represents the first hierarchy level. An `IRootAction` can have child actions (`IAction`) and
is created from an `ISession` as shown in the example below.
```c++
// C++ API
const char* rootActionName = "rootActionName";
std::shared_ptr<IRootAction> rootAction = session->enterAction(rootActionName);
```

The following snippet demonstrates how the same can be achieved using OpenKit's C API by
calling the `enterRootAction` function.
```c
// C API
const char* rootActionName = "rootActionName";
struct RootActionHandle* rootAction = enterRootAction(session, rootActionName);
```


## Entering a Child Action

To start a child `IAction` from a previously started `IRootAction` use the `enterAction` method from
`IRootAction`, as demonstrated below.

```c++
// C++ API
const char* childActionName = "childActionName";
std::shared_ptr<IAction> childAction = rootAction->enterAction(childActionName);
```

The following snippet demonstrates how the same can be achieved using OpenKit's C API by
calling the `enterRootAction` function.
```c
// C API
const char* childActionName = "rootActionName";
struct RootActionHandle* childAction = enterAction(session, childActionName);
```

## Leaving Actions

To leave an `IAction` simply use the `leave` method. The method returns the parent action (`IRootAction`) or nothing
if it is an `IRootAction`.

Due to C++ nature it's also a good idea to reset the shared pointer by assigning `nullptr`.
This decreases the reference count and might clean up no longer needed resources.

```c++
// C++ API
std::shared_ptr<IRootAction> parentAction = action->leave(); // returns the appropriate IRootAction
action = nullptr; // decrease reference count and let shared ptr clean up

parentAction->leave();
parentAction = nullptr; // decrease reference count and let shared ptr clean up
```

Root Actions and Actions must be left or [canceled](#canceling-actions)
when using OpenKit's C API. Calling the `leaveRootAction` or `leaveAction` functions 
frees internally allocated resources. If those functions are not invoked memory leaks will occur.  
After a call to `leaveRootAction` or `leaveAction` the pointer is no longer valid and no further
API calls must be made using this pointer.

The following snippet demonstrates how this can be achieved.

```c
// C API

// first leave the child action
leaveAction(childAction);
// Since the childAction now points to an invalid memory location
// it is a good idea to assign NULL to it.
childAction = NULL;

// now leave the root action
leaveRootAction(parentAction);
// Since the parentAction now points to an invalid memory location
// it is a good idea to assign NULL to it.
parentAction = NULL;
```

## Canceling Actions

Canceling an `IAction` or `IRootAction` is similar to [leaving](#leaving-actions) an `IAction` or `IRootAction`,
except that the `IAction` or `IRootAction` will be discarded and not reported to Dynatrace.
Open child objects, like child actions and web request tracers, will be
discarded as well.
To cancel an `IAction` or `IRootAction` simply use the method `cancelAction` as shown in the example below.

```c++
std::shared_ptr<IRootAction> parentAction = action->cancelAction(); // returns the appropriate IRootAction
action = nullptr; // decrease reference count and let shared ptr clean up

parentAction->cancelAction();
parentAction = nullptr; // decrease reference count and let shared ptr clean up
```

The same can can be achieved using OpenKit's C API, by calling the functions `cancelRootAction` or `cancelAction`.
Calling these methods will free internally allocated resources.  
After a call to `cancelRootAction` or `cancelAction` the pointer is no longer valid and no further
API calls must be made using this pointer.  

The snippet below shows how to correctly cancel an action and root action.

```c
// C API

// first cancel the child action
cancelAction(childAction);
// Since the childAction now points to an invalid memory location
// it is a good idea to assign NULL to it.
childAction = NULL;

// now cancel the root action
cancelRootAction(parentAction);
// Since the parentAction now points to an invalid memory location
// it is a good idea to assign NULL to it.
parentAction = NULL;
```

## Obtaining an Action duration

To get the `IAction` or `IRootAction` duration use the method `getDuration`.
The method returns the difference  between the end time and start time, 
if the `IAction` or `IRootAction` is left or canceled.  
If the `IAction` or `IRootAction` is still ongoing, the duration is the
difference between the current time and start time.

```c++
std::chrono::milliseconds duration = action->getDuration(); // gives current time - action start time
action->leaveAction();
duration = action->getDuration(); // gives action end time - action start time
```

The action duration can also be obtained using the C API, as demonstrated in the snippet below.  
Contrary to the C++ API, it is not possible to get the duration of a left or canceled
action or root action, since canceling and leaving actions frees the pointers.

```c
// C API

// get duration of child action in milliseconds
int64_t childActionDurationInMilliseconds = getDurationOfAction(childAction);

// get duration of root action in milliseconds
int64_t rootActionDurationInMilliseconds = getDurationOfRootAction(parentAction);
```

## Report Named Event

To report a named event use the `reportEvent` method on `IAction`.
```c++
// C++ API
const char* eventName = "eventName";
action->reportEvent(eventName);

// also report on the RootAction
rootAction->reportEvent(eventName);
```

The same can be achieved using the OpenKit C API as demonstrated below.

```c
// C API
const char* eventName = "eventName";
reportEventOnAction(action, eventName);

// also report on the RootAction
reportEventOnRootAction(rootAction, eventName);
```

## Report Key-Value Pairs

Key-value pairs can also be reported via an `IRootAction` or an `IAction` as shown in the example below.
Overloaded methods exist for the following value types:
* `int32_t`
* `int64_t`
* `double`
* `const char*`

```c++
// C++ API

// first report a 32-bit int value
const char* keyInt32Type = "int32Type";
int32_t valueInt32 = 42;
action->reportValue(keyInt32Type, valueInt32);
rootAction->reportValue(keyInt32Type, valueInt32);

// let's also report a 64-bit int value 
const char* keyInt64Type = "int64Type";
int64_t valueInt64 = 21;
action->reportValue(keyInt64Type, valueInt64);
rootAction->reportValue(keyInt64Type, valueInt64);

// then let's report a double value
const char* keyDoubleType = "doubleType";
double valueDouble = 3.141592653589793;
action->reportValue(keyDoubleType, valueDouble);
rootAction->reportValue(keyDoubleType, valueDouble);

// and also a string value
const char* keyStringType = "stringType";
const char* valueString = "The quick brown fox jumps over the lazy dog";
action->reportValue(keyStringType, valueString);
rootAction->reportValue(keyStringType, valueString);
```

The same can be achieved using the OpenKit C API as demonstrated below.

```c
// C API

// first report an int value
const char* keyInt32Type = "intType";
int32_t valueInt32 = 42;
reportIntValueOnAction(action, keyInt32Type, valueInt32);
reportIntValueOnRootAction(rootAction, keyInt32Type, valueInt32);

// let's also report a 64-bit int value 
const char* keyInt64Type = "int64Type";
int64_t valueInt64 = 21;
reportInt64ValueOnAction(action, keyInt64Type, valueInt64)
reportInt64ValueOnRootAction(rootAction, keyInt64Type, valueInt64);

// then let's report a double value
const char* keyDoubleType = "doubleType";
double valueDouble = 3.141592653589793;
reportDoubleValueOnAction(action, keyDoubleType, valueDouble);
reportDoubleValueOnRootAction(rootAction, keyDoubleType, valueDouble);

// and also a string value
const char* keyStringType = "stringType";
const char* valueString = "The quick brown fox jumps over the lazy dog";
reportStringValueOnAction(action, keyStringType, valueString);
reportStringValueOnRootAction(rootAction, keyStringType, valueString);
```

## Report an Error

`IRootAction` and `IAction` also have the possibility to report an error with a given 
name and error code.  
The code fragment below shows how.
```c++
// C++ API
const char* errorName = "Unknown Error";
int32_t errorCode = 42;

action->reportError(errorName, errorCode);
rootAction->reportError(errorName, errorCode);
```

The same can be achieved using the OpenKit C API as demonstrated below.

```c
// C API
const char* errorName = "Unknown Error";
int32_t errorCode = 42;

reportErrorCodeOnAction(action, errorName, errorCode);
reportErrorCodeOnRootAction(rootAction, errorName, errorCode);
```

Errors can also be reported with the method
`reportError(const char* errorName, const char* causeName, const char* causeDescription, const char* causeStackTrace)`
which exists for `IRootAction` and `IAction`.  
The arguments are
* `errorName` is the name of the reported error
* `causeName` is an optional short name of the cause, typically the name of the exception. E.g. `"std::runtime_error"`
* `causeDescription` is an optional short description of the cause, typically the value returned by the `std::exception::what()` method
* `causeStackTrace` is an optional stack trace of the cause

The fragment below shows how to report such an error.
```c++
// C++ API
void RestrictedClass::restrictedMethod()
{
    if (!isUserAuthorized())
    {
        const char* errorName = "Authorization error";
        const char* causeName = "User not authorized";
        const char* causeDescription = "The current user is not authorized to call restrictedMethod.";
        const char* causeStackTrace = nullptr; // no stack trace is reported

        // report the error on IAction and IRootAction
        action->reportError(errorName, causeName, causeDescription, causeStackTrace);
        rootAction->reportError(errorName, causeName, causeDescription, causeStackTrace);

        return;
    }

    // ... further processing
}
```

The same can be achieved using the OpenKit C API as demonstrated below.

```c
// C API
void restrictedFunction()
{
    if (!isUserAuthorized())
    {
        const char* errorName = "Authorization error";
        const char* causeName = "User not authorized";
        const char* causeDescription = "The current user is not authorized to call restrictedFunction.";
        const char* causeStackTrace = NULL; // no stack trace is reported

        // report the error on action and rootAction
        reportErrorCauseOnAction(action, errorName, causeName, causeDescription, causeStackTrace);
        reportErrorCauseOnRootAction(rootAction, errorName, causeName, causeDescription, causeStackTrace);

        return;
    }

    // ... further processing
}
```

## Tracing Web Requests

One of the most powerful OpenKit features is web request tracing. When the application starts a web
request (e.g. HTTP GET) a special tag can be attached to the header. This special header allows
Dynatrace SaaS/Dynatrace Managed/AppMon to correlate actions with a server side PurePath. 

An example is shown below.

```c++
// C++ API
const char* url = "http://www.my-backend.com/api/v3/users";


// create the WebRequestTracer
std::shared_ptr<IWebRequestTracer> webRequestTracer = action->traceWebRequest(url);

// Note: tracing web requests can also be done from rootActions
// std::shared_ptr<IWebRequestTracer> webRequestTracer = rootAction->traceWebRequest(url);

// this is the HTTP header name & value which needs to be added to the HTTP request.
const char* headerName = openkit::WEBREQUEST_TAG_HEADER;
const char* headerValue = webRequestTracer->getTag();

webRequestTracer->start();

// perform the request here & do not forget to add the HTTP header

webRequestTracer->setBytesSent(12345);     // 12345 bytes sent
webRequestTracer->setBytesReceived(67890); // 67890 bytes received
webRequestTracer->stop(200);               // 200 was the response code

// also reset the shared pointer after stop() method has been called
// to cleanup pointer
webRequestTracer = nullptr;
```

The same can be achieved using the OpenKit C API as demonstrated below.

```c
// C API
const char* url = "http://www.my-backend.com/api/v3/users";


// create the WebRequestTracer
struct WebRequestTracerHandle* webRequestTracer = traceWebRequestOnAction(action, url);

// Note: tracing web requests can also be done from rootActions
// struct WebRequestTracerHandle* webRequestTracer = traceWebRequestOnRootAction(rootAction, url);

// this is the HTTP header name & value which needs to be added to the HTTP request.
const char* headerName = WEBREQUEST_TAG_HEADER;
const char* headerValue = getTag(webRequestTracer);

startWebRequest(webRequestTracer);

// perform the request here & do not forget to add the HTTP header

setBytesSent(webRequestTracer, 12345);                  // 12345 bytes sent
setBytesReceived(webRequestTracer, 67890);              // 67890 bytes received
stopWebRequestWithResponseCode(webRequestTracer, 200);  // 200 was the response code

// since webRequestTracer now points to an invalid memory
// location, it's recommended to set the pointer to NULL.
webRequestTracer = NULL;
```


## Terminating the OpenKit Instance

When an OpenKit instance is no longer needed (e.g. the application using OpenKit is shut down), the previously
obtained instance can be cleared by invoking the `shutdown` method.  
Calling the `shutdown` method blocks the calling thread while the OpenKit flushes data which has not been
transmitted yet to the backend (Dynatrace SaaS/Dynatrace Managed/AppMon).  
When using OpenKit's C API the same can be achieved by calling the `shutdownOpenKit` function.  
Details are explained in [internals.md](internals.md)

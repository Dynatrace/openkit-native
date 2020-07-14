# General Concepts

In this part the concepts used throughout OpenKit are explained. A short sample how to use OpenKit is
also provided. For detailed code samples have a look into [example.md][example].

## String Handling in OpenKit

All strings to OpenKit's C and C++ API are passed as NULL terminated `const char*`.  
Internally OpenKit makes a copy of the passed parameters. Therefore developers using
OpenKit C/C++ do not need to care about the lifetime of the passed arguments.

The passed `const char*` can be encoded in UTF-8 if characters outside the ASCII range are needed.  
Using UTF-8 encoding has the advantage that all ASCII strings can be used without encoding them.
If characters outside the ASCII range are used (e.g. German Umlaut characters), the
developer using OpenKit's API is responsible for appropriate encoding.  
When OpenKit encounters invalid characters in the passed `const char*` they are
replaced using the Unicode Replacement Character (`U+FFFD`) in the internal copy.

## DynatraceOpenKitBuilder / AppMonOpenKitBuilder
A `DynatraceOpenKitBuilder`/`AppMonOpenKitBuilder` instance is responsible for setting 
application relevant information, e.g. the application's version and device specific information, and to create
an `IOpenKit` instance.

## OpenKit
 
An `IOpenKit` instance is responsible for creating user sessions (see `ISession`).
  
Although it would be possible to have multiple `IOpenKit` instances connected to the same endpoint
(Dynatrace/AppMon) within one process, there should be one unique instance. `IOpenKit` is designed to be
thread safe and therefore the instance can be shared among threads.  

On application shutdown, `shutdown()` needs to be called on the `IOpenKit` instance.

## Session

An `ISession` represents kind of a user session, similar to a browser session in a web application.
However the application developer is free to choose how to treat an `ISession`.  
The `ISession` is used to create `IRootAction` instances, report application crashes and can be used for tracing web requests.

When an `ISession` is no longer required, it's highly recommended to end it, using the `ISession::end()` method. 

## RootAction and Action

The `IRootAction` and `IAction` are named hierarchical nodes for timing and attaching further details.
An `IRootAction` is created from the `ISession` and it can create `IAction` instances.
Both, `IRootAction` and `IAction`, provide the possibility to attach key-value pairs, named events and errors, and can be used 
for tracing web requests.

When an `IRootAction` or `IAction` is no longer required, it's highly recommended to close it, using the `IAction::leaveAction()` or
`IRootAction::leaveAction()` method.

## WebRequestTracer

When the application developer wants to trace a web request, which is served by a service 
instrumented by Dynatrace, an `IWebRequestTracer` should be used, which can be
requested from an `ISession` or an `IAction`.

## Named Events

A named `Event` is attached to an `IAction` and contains a name.

## Key-Value Pairs

For an `IAction` key-value pairs can also be reported. The key is always a UTF-8 encoded String (`const char*`)
and the value may be an Integer (`int32_t`), a floating point (`double`) or a UTF-8 encoded String (`const char*`).

## Errors & Crashes

Errors are a way to report an erroneous condition on an `IAction`.  
Crashes are used to report (unhandled) exceptions on an `ISession`.

## Identify Users

OpenKit enables you to tag sessions with unique user tags. The user tag is an UTF-8 encoded String 
(`const char*`) that allows to uniquely identify a single user.

## GDPR Compliance

When creating an `OpenKit` instance, it is also possible to set the GDPR compliant mode
where you can specify which data is collected.
For detailed description and samples refer to [example.md][example].
Getting user consent must be handled within the application itself.

# Example

This small example provides a rough overview how OpenKit can be used.  
Detailed explanation is available in [example.md][example].

```c++
const char* applicationName = "My OpenKit application";
const char* applicationID = "application-id";
uint64_t deviceID = 42;
const char* endpointURL = "https://tenantid.beaconurl.com/mbeacon";

std::shared_ptr<openkit::IOpenKit> openKit = 
    DynatraceOpenKitBuilder(endpointURL, applicationID, deviceID)
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

[example]: ./example.md
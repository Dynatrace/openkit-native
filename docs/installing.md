# Installing and updating OpenKit Native

Since OpenKit Native is only available as source code, and not as pre-built binary,
the steps for initial installation and updating are the same.

## Runtime Prerequisites
### Supported operating systems
* Microsoft Windows
* GNU/Linux

### Unsupported operating systems
Other operating systems, including MacOS, iOS and Android, might
work as well, as the required build tool chains are available there as also.

### Additional runtime libraries
For Microsoft Windows, if OpenKit Native is linked against shared C/C++ runtime libraries,
Visual C++ Redestributable needs to be distributed. For details check
the official Microsoft documentation page [Deploying Native Desktop Applications (Visual C++)][ms-depolyment].

On Linux/UNIX based operating systems OpenKit does not support statically linked
system libraries. Therefore the necessary C/C++ runtime libraries, as well as OpenSSL must be
available.  
The program `ldd` can be used to determine which libraries are needed.

## Obtaining and updating OpenKit Native
The OpenKit C/C++ source code can be downloaded as `zip` or `tar.gz` file from the [GitHub Releases][gh-releases] page.
Download the latest supported version to you computer and build OpenKit as described in [Building OpenKit C/C++][building].

An alternative approach is to clone the repository and checkout the tag you want to build.
Be aware that you might need to update your cloned repository if you want to update to a newer tag.

## Necessary defines when using OpenKit

When building an application which is using OpenKit, it's necessary
to set the following preprocessor definitions if OpenKit was previously built
as static library.
* CURL_STATICLIB
* OPENKIT_STATIC_DEFINE

If OpenKit is used as shared library (DLL/SO), those defines must not be set,
otherwise the required import directives will not be used.

## Release notifications
GitHub offers the possibility to receive notifications for new releases. Detailed instructions are available
on the [Watching and unwatching releases for a repository][gh-release-notification] page. 

[building]: ./building-openkit.md
[gh-releases]: https://github.com/Dynatrace/openkit-native/releases
[gh-release-notification]: https://help.github.com/en/github/receiving-notifications-about-activity-on-github/watching-and-unwatching-releases-for-a-repository
[ms-depolyment]: https://docs.microsoft.com/en-us/cpp/windows/deploying-native-desktop-applications-visual-cpp
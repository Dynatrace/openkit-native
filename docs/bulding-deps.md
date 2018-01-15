openkit-c requires the following external libraries:

-curl
-zlib

On Linux the artifacts can be obtained with the system package manager.
On Windows some additional building steps are required.

Windows

| Building curl from source 

1. Obtaining sources
curl can be obtained from the official git repository (git@github.com:curl/curl.git)

git clone git@github.com:curl/curl.git
git checkout curl-7_57_0

2. Building
cmake is required to build curl, if not already present install latest cmake release 
from https://cmake.org/download/.

-call cmake with the following options
cmake -G"Visual Studio 15 2017" -DCURL_DISABLE_FILE=1 -DCURL_DISABLE_FTP=1 -DCURL_DISABLE_GOPHER=1 -DCURL_DISABLE_IMAP=1 -DCURL_DISABLE_LDAP=1 -DCURL_DISABLE_LDAPS=1 -DCURL_DISABLE_POP3=1 -DCURL_DISABLE_RTSP=1 -DCURL_DISABLE_SMTP=1 -DCURL_DISABLE_TELNET=1 -DCURL_DISABLE_TFTP=0 -DCMAKE_USE_WINSSL=1 -DCURL_WINDOWS_SSPI=1 -DCMAKE_INSTALL_PREFIX=..\dist ..

-Open solution file in build folder

-Select RelWithDebInfo configuration in Visual Studio

-Build solution

3.Deployment
-Run target INSTALL

-Fetch result from dist folder, copy contents of dist folder directly to curl specific third party folder of openkit-c (3rdparty\curl)

| Using prebuild artifacts for zlib

1. Download archive http://gnuwin32.sourceforge.net/downlinks/zlib-lib-zip.php and extract contents into 3rdparty/zlib-1.2.3/

2. Download archive http://www.winimage.com/zLibDll/zlib123dll.zip and extract contents into 3rdparty/zlib-1.2.3/

Linux

| Install curl dependencies via package manager
    sudo apt-get install libcurl4-dev-gnutls
    
| Install zlib dependencies via package manager
    sudo apt-get install zlib1g-dev

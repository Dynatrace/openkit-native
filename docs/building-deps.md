#Building OpenKit dependencies

openkit-c requires the following external libraries:

curl
zlib
googletest

On Linux the artifacts can be obtained with the system package manager.
On Windows additional building steps are required.

##Windows
----------

###Building curl from sources
--------------------------------

curl can be obtained from the project page at https://curl.haxx.se/download.html or from the official 
git repository (git@github.com:curl/curl.git)

```
git clone git@github.com:curl/curl.git
git checkout curl-7_57_0
```

CMake is required to build curl, if not already present install latest CMake release from https://cmake.org/download/.

-call cmake.exe with the following options

```
cmake.exe -G"Visual Studio 15 2017" -DCURL_DISABLE_FILE=1 -DCURL_DISABLE_FTP=1 -DCURL_DISABLE_GOPHER=1 -DCURL_DISABLE_IMAP=1 -DCURL_DISABLE_LDAP=1 -DCURL_DISABLE_LDAPS=1 -DCURL_DISABLE_POP3=1 -DCURL_DISABLE_RTSP=1 -DCURL_DISABLE_SMTP=1 -DCURL_DISABLE_TELNET=1 -DCURL_DISABLE_TFTP=0 -DCMAKE_USE_WINSSL=1 -DCURL_WINDOWS_SSPI=1 -DCMAKE_INSTALL_PREFIX=..\dist ..
```

-Open solution file in build folder

-Select RelWithDebInfo configuration in Visual Studio

-Build solution

-Run target INSTALL

-Fetch result from dist folder, copy contents of dist folder directly to curl specific third party folder of openkit-c (3rdparty\curl-7.57.0)

###Building zlib from sources
-------------------

zlib can be obtained from the official project page of zlib:
Download http://zlib.net/zlib-1.2.11.tar.gz and unpack contents into 
workspace.

CMake is required to build zlib, if not already present install latest CMake release 
from https://cmake.org/download/.

-create and navigate to build directory

```
mkdir build
cd build
```

-call cmake.exe with the following options

```
cmake.exe  -G"Visual Studio 15 2017" -DCMAKE_INSTALL_PREFIX=..\dist ..
```

-Open solution file zlib.sln in build folder

-Select RelWithDebInfo configuration in Visual Studio

-Build solution

-Run target INSTALL

-Fetch result from dist folder, copy contents of dist folder directly to curl specific third party folder of openkit-c (3rdparty\zlib-1.2.11)

###Building Google Test from sources
---------------------

Google Test can be obtained from the project page at https://github.com/google/googletest/releases 
or  the official git repo at git@github.com:google/googletest.git.
```
git clone git@github.com:google/googletest.git
git checkout release-1.8.0
```

CMake is required to build Google Test, if not already present install latest CMake release 
from https://cmake.org/download/.

-create and navigate to build directory

```
mkdir build
cd build
```

-call cmake.exe with the following options

```
cmake.exe -G"Visual Studio 15 2017" -Dgtest_force_shared_crt=1 -DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING -DCMAKE_INSTALL_PREFIX=../dist CMAKE_DEBUG_POSTFIX=""  ..
```

-Open solution file googletest-distribution.sln in build folder

-Select Debug configuration in Visual Studio

-Build solution

-Run target INSTALL

-Fetch result from dist folder, copy contents of dist folder directly to curl specific third party folder of openkit-c (3rdparty\googletest-1.8.0\win32-debug)

-Clean dist folder

-Select Release configuration in Visual Studio

-Build solution

-Run target INSTALL

-Fetch result from dist folder, copy contents of dist folder directly to curl specific third party folder of openkit-c (3rdparty\googletest-1.8.0\win32-release)

##Linux
----------

###Building Installing curl and zlib via package manager apt</h2>

 Install curl dependencies via package manager
    sudo apt-get install libcurl4-dev-gnutls
    
 Install zlib dependencies via package manager
    sudo apt-get install zlib1g-dev
	
###Building Google Test from sources

googletest can be obtained at the official git repo

```
git clone git@github.com:google/googletest.git
git checkout release-1.8.0
```

CMake is required to build googletest, if not already present install latest CMake release 
from https://cmake.org/download/.

-create and navigate to build directory

```
mkdir build
cd build
```

-call cmake.exe with the following options

```
cmake.exe -G"Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../dist-linux ..
```

-Build project using make

```
make
make install
```

-Fetch result from dist-linux folder, copy contents of dist-linux folder directly to curl specific third party folder of openkit-c (3rdparty\googletest-1.8.0\linux)
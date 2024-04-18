The repository include the next projects:
- TransmiterModule (placed on the TransmiterModule folder)
- RcControllerModule (placed on the RcControllerModule folder)

The repository structure
└── Repository root/
    ├── RcModule (the projekt to the RC side)
    ├── TransmiterModule (the projekt to the Transmiter side)
    ├── HAL (General. The LL STM drivers)
    ├── Lib (General. User code used the TransmiterModule and the RcControllerModule projects)
    ├── Middlewares (General)
    └── tools (General)

The projects uses CMakeLists.txt as a project file.

The next packet must be installed:
- Cmake https://cmake.org/download/
- MinGW https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/
- GNU Arm  https://developer.arm.com/downloads/-/gnu-rm

To build the desired project, entr to the prjekt folder (TransmiterModule or RcControllerModule) and call the "Prj name"/build.bat in the next format:

build "%1"

Where:
%1 - RELEASE or DEBUG

Usage example:
build.bat RELEASE

After building the "Prj name".hex file will be created on the /build folder
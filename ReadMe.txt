The repository include the next projects:
- LSM303DLHC driver and usage example

The repository structure
└── Repository root/
    ├── App (Business logic code)
    ├── BSP (Board dependencies: pin definitions, peripheral definitions and custom drivers over the HAL)
    ├── HAL (General. The LL STM drivers)
    ├── Lib (General. User code used the TransmiterModule and the RcControllerModule projects)
    ├── MCU (MCU specific sources, like CMSIS, peripheral description and so on)
    ├── Middlewares (General)
    └── tools (General)

The projects uses CMakeLists.txt as a project file.

The next packet must be installed:
- Cmake https://cmake.org/download/
- MinGW https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/
- GNU Arm  https://developer.arm.com/downloads/-/gnu-rm

To build the project, call build.bat in the next format:

build Lsm3030dlhc "%1"

Where:
%1 - RELEASE or DEBUG

Usage example:
build.bat Lsm3030dlhc RELEASE

After building the "Prj name".hex file will be created on the /build folder
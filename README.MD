# IrrlichtVectorArt
## _An Anti-Grain Geometry (AGG) project that works with Irrlicht_

Based on the following AGG project:

- [AGG's examples/svg_viewer/svg_test.cpp file](https://github.com/ghaerr/agg-2.6/blob/master/agg-src/examples/svg_viewer/svg_test.cpp)

## Features

- Render SVG files in Irrlicht.

### To be implemented

- Extend Irrlicht's file system to automatically support SVG rather than parsing file extension manually.

## Installation

Build Irrlicht however you'd like.

Build AGG using the following steps:

1. Download AGG from [their GitHub repo](https://github.com/ghaerr/agg-2.6)
2. Download libexpat from [their GitHub repo](https://github.com/libexpat/libexpat)
3. Install [MSYS2](https://www.msys2.org/) to c:/msys64
4. Edit c:/msys64/msys2_shell.cmd and remove rem from the line with `rem MSYS2\_PATH\_TYPE=inherit`
5. Open a x64 Native Tools Command Prompt for VS 2019
6. Run `c:/msys64/msys2_shell.cmd`
7. In the new MSYS2 shell, perform:
```
cd agg/agg-src/
make
```
8. (not always necessary) Did you receive an error that the Makefile does not exist? Duplicate file Makefile.in.MINGW32_NT-5.0 and rename to Makefile.in.MSYS_NT-10.0-19042, or whatever the error says it should be.
9. Get sources for x264 and ffmpeg, then fix build scripts:
```
cd expat/win32/
md build
cd build
cmake -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=RelWithDebInfo ../..
```
10. Open the new expat/win32/build/expat.sln and build the project for Release and x64
11. To compile out of the box, make sure your project directory structure looks like this:

**SomeFolder/IrrlichtVectorArt/IrrlichtVectorArt/** (this repository)

**SomeFolder/Hello/libs/agg/** (this is where AGG should be installed)

**SomeFolder/Hello/libs/libexpat/** (this is where libexpat should be installed)

**SomeFolder/irrlicht-code/** (place Irrlicht was downloaded -- this project uses the statically linked version, so you'll need to build Irrlicht statically)


## License

MIT

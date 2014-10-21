#! /bin/bash -e

# Check that valid parameters have been specified.
if [ $# -ne 2 ] || ([ "$1" != "11" ] && [ "$1" != "12" ]) || ([ "$2" != "Debug" ] && [ "$2" != "Release" ])
then
  echo "Usage: build-win.sh {11|12} {Debug|Release}"
  exit
fi

cd libraries
#./build-boost_1_56_0-win.sh "msvc-$1.0"
#./build-opencv-2.4.9-win.sh "Visual Studio $1 Win64"
./build-SDL2-2.0.3-win.sh "Visual Studio $1 Win64"
cd ..

echo "[spaint] Building spaint"

if [ ! -d build ]
then
  mkdir build
  cd build

  echo "[spaint] ...Configuring using CMake..."
  cmake -G "Visual Studio $1 Win64" ..

  cd ..
fi

cd build

echo "[spaint] ...Running build..."
cmd //c "msbuild /p:Configuration=$2 spaint.sln"

echo "[spaint] ...Installing..."
cmd //c "msbuild /p:Configuration=$2 INSTALL.vcxproj"

echo "[spaint] ...Finished building spaint."
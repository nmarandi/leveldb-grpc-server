**simple GRPC server for [LevelDB](https://github.com/google/leveldb)**

# Features not supported yet from LevelDB
- [ ]  snapshot
- [ ]  custom comparator
- [ ]  cache
- [ ]  logger
- [ ]  filter policy

# Work to do
- [ ]  automatic test
- [ ]  automatic build   
- [ ]  support input args 
- [ ]  support config file

# How to build

this project for build needs cmake and grpc to be installed and cmake find_package be able to find and load grpc setting.
for this purpose I use vcpkg to install grpc.

```
mkdir C:\Tools
git clone https://github.com/microsoft/vcpkg.git C:\Tools\vcpkg
cd C:\Tools\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe install grpc:x86-windows
.\vcpkg.exe install grpc:x64-windows
```

```
git clone https://github.com/nmarandi/leveldb-grpc-server.git
cd leveldb-grpc-server
git submodule update --init --recursive
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=c:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 16 2019" -A "x64" -DCMAKE_CONFIGURATION_TYPES="Release"
cmake --build . --config Release
```


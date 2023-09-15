# CMakeList.txt : CMake project for NetMapper, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_PREFIX_PATH "D:/vcpkg/installed/x64-windows/share")

# Add source to this project's executable.
add_executable (NetMapper "NetMapper.cpp" "NetMapper.h" "TCPScan.cpp" "IpIterator.cpp" "IpIterator.h" "TCPScan.h" "TraceMap/TraceMap.cpp" "TraceMap/TraceMap.h" "TraceMap/NetMap.cpp" "TraceMap/Node.cpp" "TraceMap/NetMap.h" "TraceMap/Node.h" "TraceMap/PacketFactory.h"  "TraceMap/PacketFactory.cpp"  "TraceMap/ThreadManager.h" "TraceMap/ThreadManager.cpp"  )

find_package(unofficial-pcapplusplus CONFIG REQUIRED)
target_link_libraries(NetMapper PRIVATE unofficial::pcapplusplus::pcappp unofficial::pcapplusplus::commonpp unofficial::pcapplusplus::packetpp)


# TODO: Add tests and install targets if needed.
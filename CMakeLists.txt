cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS "-Ddebug_cerr -lprotobuf")

set(CMAKE_AUTOMOC ON)

include_directories(/usr/local/boost_1_67_0/)




find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)




add_executable(Client main.cpp Client.cpp Request_Response.pb.cc Request_Response.pb.h)
add_executable(tests tests.cpp Client.cpp Client.hpp Request_Response.pb.cc)




target_link_libraries(Client  Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network)
target_link_libraries(tests Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network)




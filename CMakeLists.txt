cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")
set (CMAKE_CXX_FLAGS "-Ddebug_cerr -lprotobuf")

set(CMAKE_AUTOMOC ON)




find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)




add_executable(Client main.cpp Client.cpp Request_Response.pb.cc Request_Response.pb.h)



target_link_libraries(Client  Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network)



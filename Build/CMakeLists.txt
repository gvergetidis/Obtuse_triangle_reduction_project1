cmake_minimum_required(VERSION 3.1...3.23)

project(ex)

set(CMAKE_BUILD_TYPE "Release")             #set this variable for not taking warnings about CGAL performance issues 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)  #set the directory where the executable will be placed

#find CGAL
find_package(CGAL QUIET COMPONENTS Qt5)
if(NOT CGAL_FOUND)
    message(STATUS "ERROR: CGAL NOT FOUND")
    return()
endif()

#find Boost
find_package(Boost REQUIRED COMPONENTS json)
if(NOT Boost_FOUND)
    message(STATUS "ERROR BOOST NOT FOUND")
    return()
endif()

#find Qt5 for graphics
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

add_definitions(-DCGAL_USE_BASIC_VIEWER)    #enable basic viewer 

add_executable(ex ../jsontest2.cpp ../CCDT_implementation.cpp)

target_link_libraries(ex CGAL::CGAL_Qt5 Qt5::Widgets Qt5::Gui Qt5::Core CGAL::CGAL Boost::boost Boost::json)
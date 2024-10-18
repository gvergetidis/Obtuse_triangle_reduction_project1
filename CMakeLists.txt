cmake_minimum_required(VERSION 3.10)

project(ex)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find CGAL package
find_package(CGAL QUIET COMPONENTS Qt5)

#set(Boost_DEBUG ON)
find_package(Boost REQUIRED COMPONENTS system filesystem)

#find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
    message(STATUS "Boost not found")
    return()
endif()
#message(STATUS "Boost version: ${Boost_VERSION}")

# Find Qt5 packages including Qt5 OpenGL
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Enable CGAL Basic Viewer
add_definitions(-DCGAL_USE_BASIC_VIEWER)

# Enable automoc (required for Qt in CMake)
#set(CMAKE_AUTOMOC ON)


# Add executable
add_executable(ex maincentroid.cpp)


#target_link_libraries(ex PRIVATE CGAL::CGAL)

# Link libraries
#add_definitions(-DCGAL_USE_BASIC_VIEWER)
#target_link_libraries(ex PRIVATE CGAL::CGAL_Qt5)
#target_link_libraries(ex CGAL::CGAL Qt5::Widgets Qt5::OpenGL)

target_link_libraries(ex CGAL::CGAL_Qt5 Qt5::Widgets Qt5::Gui Qt5::Core CGAL::CGAL Boost::filesystem Boost::system )


cmake_minimum_required(VERSION 3.6)
project(detect)

find_package(OpenCV REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp)

add_executable(camera ${SOURCE_FILES})

#message(STATUS "OpenCV library status:")
#message(STATUS "    version: ${OpenCV_VERSION}")
#message(STATUS "    libraries: ${OpenCV_LIBS}")
#message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
 
# Add OpenCV headers location to your include paths
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("./include")
include_directories("./headers")


 
# Link your application with OpenCV libraries
target_link_libraries(camera ${OpenCV_LIBS} ${LDLIBS})



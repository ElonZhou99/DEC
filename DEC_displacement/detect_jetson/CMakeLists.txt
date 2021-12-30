CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
#指定项目名称
PROJECT(main)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")



find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")


#将main.cpp 赋值给SOURCE 变量
SET(SOURCE ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_executable(main ${SOURCE})

add_library(socket 
    include/socket.hpp
    src/socket.cpp)

add_library(txt_option 
    include/txt_option.hpp
    src/txt_option.cpp)


#指定头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories("${PROJECT_BINARY_DIR}")
include_directories(${OpenCV_INCLUDE_DIRS})

#指定链接库文件目录
link_directories(${OpenCV_LIBRARY_DIRS})


#指定main 链接库opencv
target_link_libraries(main 
    ${OpenCV_LIBS}
    txt_option
    socket)

cmake_minimum_required(VERSION 3.22)
project(ct_exame)

set(CMAKE_CXX_STANDARD 14)

include_directories(vision)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


find_package( SPINNAKER REQUIRED )
include_directories( ${SPINNAKER_INCLUDE_DIRS} )

find_package(OpenCV 3.4.2 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ct_exame ../vision/main.cpp
        ../vision/SpinnakerFrameSaver.cpp
        ../vision/SpinnakerFrameSaver.h vision/main.cpp)

target_include_directories(ct_exame PUBLIC ${OpenCV_INCLUDE_DIRS} ${SPINNAKER_INCLUDE_DIRS})
target_link_libraries(ct_exame PUBLIC ${OpenCV_LIBRARIES} ${SPINNAKER_LIBRARIES})
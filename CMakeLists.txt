CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Headtrack CXX)
SET(CMAKE_BUILD_TYPE Release)
# you might have to set this
#SET(OpenCV_DIR /path/to/your/opencv/installation)
FIND_PACKAGE(OpenCV REQUIRED) 
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/include)
ADD_EXECUTABLE(headtracker src/glmain.cpp src/headtracker.cpp)
TARGET_LINK_LIBRARIES(headtracker ${OpenCV_LIBS})
add_custom_command(TARGET headtracker POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:headtracker>/data)
ADD_DEFINITIONS(
        -std=c++11 # Or -std=c++0x
            # Other flags
            )

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Headtrack CXX)
SET(CMAKE_BUILD_TYPE Release)
# you might have to set this
#SET(OpenCV_DIR /path/to/your/opencv/installation)
FIND_PACKAGE(OpenCV REQUIRED) 
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/include)
ADD_EXECUTABLE(headtracker src/headtracker.h src/headtracker.cpp)
TARGET_LINK_LIBRARIES(headtracker ${OpenCV_LIBS})

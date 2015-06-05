CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Headtrack CXX)
SET(CMAKE_BUILD_TYPE Release)
# you might have to set this
#SET(OpenCV_DIR /path/to/your/opencv/installation)
FIND_PACKAGE(OpenCV REQUIRED) 
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/include)
ADD_EXECUTABLE(headtracker src/main.cpp src/headtracker.cpp)
TARGET_LINK_LIBRARIES(headtracker ${OpenCV_LIBS})
add_custom_command(TARGET headtracker POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:headtracker>/data)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ADD_EXECUTABLE(renderer src/renderer.cpp src/util.cpp)
FIND_PACKAGE(OpenGL REQUIRED)
TARGET_LINK_LIBRARIES(renderer ${OPENGL_LIBRARIES})
ADD_SUBDIRECTORy(lib/glfw)
INCLUDE_DIRECTORIES(lib/glfw/include)
TARGET_LINK_LIBRARIES(renderer glfw ${GLFW_LIBRARIES})
add_custom_command(TARGET renderer POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:renderer>/data)

cmake_minimum_required(VERSION 3.5)
project(my-effect)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(my-effect image-effect.cpp )

target_link_libraries(my-effect -lm)
target_link_libraries(my-effect -lpthread)
target_link_libraries(my-effect ${OpenCV_LIBS})


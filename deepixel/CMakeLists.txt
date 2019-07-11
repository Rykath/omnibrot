include_directories(.)

#set(CMAKE_CXX_FLAGS "-O3")

add_library(deepixel STATIC ./deepixel.cpp )

add_executable(deepixel-test-basics ./testing/basics.cpp)
target_link_libraries(deepixel-test-basics deepixel)

add_executable(deepixel-mandelbrot ./testing/mandel.cpp)
target_link_libraries(deepixel-mandelbrot deepixel)

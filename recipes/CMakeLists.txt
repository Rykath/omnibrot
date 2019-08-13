add_executable(mandelbrot ./mandelbrot.cpp)
target_link_libraries(mandelbrot deepixel)
target_link_libraries(mandelbrot asdf)

add_executable(test ./test.cpp)
target_link_libraries(test deepixel)
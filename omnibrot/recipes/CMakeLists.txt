add_executable(mandelbrot ./mandelbrot.cpp)
target_link_libraries(mandelbrot deepixel)
target_link_libraries(mandelbrot asdf)

add_executable(test ./test.cpp)
target_link_libraries(test deepixel)

add_executable(mandel-FPHPN ./mandel_fphpn.cpp)
target_link_libraries(mandel-FPHPN deepixel asdf)

add_executable(mandel-Rel2Cen ./mandel_rel2cen.cpp)
target_link_libraries(mandel-Rel2Cen deepixel asdf)
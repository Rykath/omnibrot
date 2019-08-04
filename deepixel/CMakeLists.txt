add_library(deepixel STATIC ./deepixel.cpp )

add_executable(deepixel-test-basics ./testing/basics.cpp)
target_link_libraries(deepixel-test-basics deepixel)


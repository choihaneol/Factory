cmake_minimum_required(VERSION 3.5)

include_directories("./include")
file(GLOB sources "src/*.cpp" )

add_executable(ori_test ${sources})

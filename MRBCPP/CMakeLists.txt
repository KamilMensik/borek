file(GLOB_RECURSE MRBCPP_SRC_FILES src/*.cpp)

project(MRBCPP)
add_library(MRBCPP STATIC ${MRBCPP_SRC_FILES})
target_link_libraries(MRBCPP ${CMAKE_SOURCE_DIR}/Borek/vendor/mruby/build/host/lib/libmruby.a)
target_include_directories(MRBCPP PUBLIC "${CMAKE_SOURCE_DIR}/Borek/vendor/mruby/include")
target_include_directories(MRBCPP PUBLIC "${PROJECT_SOURCE_DIR}/src")

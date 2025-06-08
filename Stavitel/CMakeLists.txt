file(GLOB_RECURSE STAVITEL_SRC_FILES src/*.cpp)

add_executable(Stavitel ${STAVITEL_SRC_FILES})

add_compile_definitions(_PROJECT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(Stavitel PUBLIC BOREK)
target_include_directories(Stavitel PRIVATE "./../Borek/src"
                                            "./../Borek/vendor/spdlog/include"
                                            "./src")

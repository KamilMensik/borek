file(GLOB_RECURSE SANDBOX_SRC_FILES src/*.cpp)

add_executable(SandboxApp ${SANDBOX_SRC_FILES})
target_link_libraries(SandboxApp PUBLIC BOREK)
target_include_directories(SandboxApp PUBLIC "./../Borek/src"
                                             "./../Borek/vendor/spdlog/include")

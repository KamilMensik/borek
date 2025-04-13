file(GLOB_RECURSE ENGINE_SRC_FILES src/*.cpp)

project(BOREK)
add_library(BOREK STATIC ${ENGINE_SRC_FILES})

# Add external dependencies to build
add_subdirectory(vendor)

add_compile_definitions(GLFW_INCLUDE_NONE)

target_include_directories(BOREK PUBLIC "${PROJECT_SOURCE_DIR}/src/Borek")

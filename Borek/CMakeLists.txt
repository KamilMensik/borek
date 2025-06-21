file(GLOB_RECURSE ENGINE_SRC_FILES src/*.cpp src/*.c src/*.h)

project(BOREK)
add_library(BOREK STATIC ${ENGINE_SRC_FILES})

add_compile_definitions(_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# Add external dependencies to build
add_subdirectory(vendor)

add_compile_definitions(GLFW_INCLUDE_NONE)

target_include_directories(BOREK PUBLIC "${PROJECT_SOURCE_DIR}/src/Borek")

# Include ECS library
target_link_libraries(BOREK ECS)
target_include_directories(BOREK PRIVATE "${PROJECT_SOURCE_DIR}/../ECS/src")

file(GLOB_RECURSE ECS_SRC_FILES src/*.cpp)

project(ECS)
add_library(ECS STATIC ${ECS_SRC_FILES})

target_include_directories(ECS PUBLIC "${PROJECT_SOURCE_DIR}/src")

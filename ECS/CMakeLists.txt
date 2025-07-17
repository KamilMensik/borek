file(GLOB_RECURSE ECS_SRC_FILES src/*.cpp)

project(ECS)

if(CMAKE_HOST_SYSTEM MATCHES Linux)
   message(STATUS "Build host runs Linux")
   set(CMAKE_C_FLAGS "-g -Wall -fPIC")
   set(CMAKE_CXX_FLAGS "-g -Wall -fPIC")
endif()

add_library(ECS STATIC ${ECS_SRC_FILES})

target_include_directories(ECS PUBLIC "${PROJECT_SOURCE_DIR}/src")

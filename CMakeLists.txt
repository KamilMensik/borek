cmake_minimum_required(VERSION 3.20)
project(BOREK_ENGINE)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows")
   add_compile_definitions(BR_PLATFORM_WINDOWS)
endif()

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(CMAKE_HOST_SYSTEM MATCHES Linux)
   message(STATUS "Build host runs Linux")
   set(CMAKE_C_FLAGS "-g -Wall -fPIC -msse4.1")
   set(CMAKE_CXX_FLAGS "-g -Wall -fPIC -msse4.1")
endif()

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

add_subdirectory(ECS)
add_subdirectory(MRBCPP)
add_subdirectory(Borek)
add_subdirectory(Sandbox)
add_subdirectory(Stavitel)

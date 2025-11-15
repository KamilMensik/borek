add_subdirectory(glfw)
target_link_libraries(BOREK glfw)

add_subdirectory(glad)
target_link_libraries(BOREK GLAD)

add_subdirectory(imgui)
target_link_libraries(BOREK imgui)

add_subdirectory(stb)
target_link_libraries(BOREK stb)

add_subdirectory(yaml-cpp)
target_link_libraries(BOREK yaml-cpp)

add_subdirectory(imguismo)
target_link_libraries(BOREK imguismo)

add_subdirectory(spdlog)
target_link_libraries(BOREK spdlog)

add_subdirectory(msdf-atlas-gen)
target_link_libraries(BOREK msdf-atlas-gen)

add_subdirectory(freetype)
target_link_libraries(BOREK freetype)

add_subdirectory(miniaudio)
target_link_libraries(BOREK miniaudio)

# Add ImSearch - Cant bother to fork it.
add_library(imsearch STATIC "./imsearch/imsearch.cpp")
target_include_directories(imsearch PRIVATE "./imsearch"
                                            "./imgui")
target_link_libraries(BOREK imsearch)

target_link_libraries(BOREK ${CMAKE_SOURCE_DIR}/Borek/vendor/mruby/build/host/lib/libmruby.a)

target_include_directories(BOREK PUBLIC "./spdlog/include"
                                        "./../vendor"
                                        "./magic_enum/include"
                                        "./glfw/include"
                                        "./glad/include"
                                        "./imgui"
                                        "./glm"
                                        "./stb"
                                        "./yaml-cpp/include"
                                        "./mruby/include"
                                        "./msdf-atlas-gen"
                                        "./freetype/include",
                                        "./miniaudio")

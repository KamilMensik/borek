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

add_subdirectory(Box2D)
target_link_libraries(BOREK box2d)

target_include_directories(BOREK PUBLIC "./spdlog/include"
                                        "./../vendor"
                                        "./magic_enum/include"
                                        "./glfw/include"
                                        "./glad/include"
                                        "./imgui"
                                        "./glm"
                                        "./stb"
                                        "./Box2D/include"
                                        "./yaml-cpp/include")

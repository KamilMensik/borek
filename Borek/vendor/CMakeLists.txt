add_subdirectory(glfw)
target_link_libraries(BOREK glfw)

add_subdirectory(glad)
target_link_libraries(BOREK GLAD)

add_subdirectory(imgui)
target_link_libraries(BOREK imgui)

target_include_directories(BOREK PUBLIC "./spdlog/include"
                                        "./magic_enum/include"
                                        "./glfw/include"
                                        "./glad/include"
                                        "./imgui"
                                        "./glm")

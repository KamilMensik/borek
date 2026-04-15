add_library(im-neo-sequencer STATIC "./imgui_neo_internal.cpp"
                                    "./imgui_neo_sequencer.cpp")

target_include_directories(im-neo-sequencer PRIVATE "./../imgui")

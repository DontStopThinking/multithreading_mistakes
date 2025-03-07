cmake_minimum_required(VERSION 3.30)

project(MultithreadingMistakes)

add_executable(
    multithreading_mistakes
    "code/main.cpp"
)

target_compile_features(multithreading_mistakes PRIVATE cxx_std_23)

set_target_properties(
    multithreading_mistakes
    PROPERTIES
        UNITY_BUILD true
        CXX_SCAN_FOR_MODULES false # Need to disable scanning for modules since we're using unity build.
)

install(TARGETS multithreading_mistakes)

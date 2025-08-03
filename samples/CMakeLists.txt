cmake_minimum_required(VERSION 3.20)

include(FindVulkan)

add_executable(main main.cpp)
target_include_directories(main PRIVATE "../include")
target_link_libraries(main PRIVATE Vulkan::Vulkan)
target_compile_features(main PRIVATE cxx_std_20)


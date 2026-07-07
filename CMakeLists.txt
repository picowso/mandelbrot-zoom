cmake_minimum_required(VERSION 3.28)
project(MANDELBROT-ZOOM LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp src/core.cpp)
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)

if(WIN32)
    target_link_options(main PRIVATE -Wl,--stack=268435456)
else()
    set_target_properties(main PROPERTIES LINK_FLAGS -Wl,-z,stack-size=268435456)
endif ()

target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE SFML::Graphics)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)

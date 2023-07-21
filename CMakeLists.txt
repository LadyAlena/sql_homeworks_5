cmake_minimum_required(VERSION 3.20)

project(ControlClient)

set(SOURCE_FILES
    source/main.cpp
    source/control_data_base/control_data_base.cpp
)

set(HEADER_FILES
    source/control_data_base/control_data_base.h
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libpqxx-7.7.5 libpqxx-build)

add_executable(ControlClient ${SOURCE_FILES} ${HEADER_FILES})

target_compile_features(ControlClient PRIVATE cxx_std_17)

target_link_libraries(ControlClient pqxx)
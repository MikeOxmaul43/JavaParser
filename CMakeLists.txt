cmake_minimum_required(VERSION 3.16)

project(KNPO_Java_Parser LANGUAGES CXX)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ENABLE_COVERAGE)
    message(STATUS "Coverage enabled")

    add_compile_options(
        -O0
        -g
        --coverage
    )

    add_link_options(--coverage)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Test)

set(COMMON_SOURCES
    ${PROJECT_SOURCE_DIR}/src/parser/parser.cpp
    ${PROJECT_SOURCE_DIR}/src/errors/errors.cpp
    ${PROJECT_SOURCE_DIR}/src/io/file_writer.cpp
    ${PROJECT_SOURCE_DIR}/src/io/file_reader.cpp
    ${PROJECT_SOURCE_DIR}/src/io/dot_generator.cpp
)

add_executable(KNPO_Java_Parser
    src/main.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(KNPO_Java_Parser PRIVATE
    Qt6::Core
)

target_include_directories(KNPO_Java_Parser PRIVATE src)

enable_testing()
add_subdirectory(tests)
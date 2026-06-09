cmake_minimum_required(VERSION 3.16)

project(KNPO_Java_Parser LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test)



add_executable(KNPO_Java_Parser
    src/main.cpp
)

target_link_libraries(KNPO_Java_Parser
    Qt${QT_VERSION_MAJOR}::Core
)

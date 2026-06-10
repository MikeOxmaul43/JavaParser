add_executable(tst_parser
    tst_parser.cpp
    ${COMMON_SOURCES}
)

target_include_directories(tst_parser PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(tst_parser
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME ParserTests COMMAND tst_parser)
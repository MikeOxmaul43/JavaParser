add_executable(tst_parser
    unit/tst_parser.cpp
    ${COMMON_SOURCES}
)

target_include_directories(tst_parser PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(tst_parser PRIVATE
    Qt6::Core
    Qt6::Test
)

add_test(NAME ParserTests COMMAND tst_parser)
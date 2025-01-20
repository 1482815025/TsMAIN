include(CPack)
set(CMAKE_CXX_STANDARD 17)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
aux_source_directory (. SRC_LIST)

# # 静态编译
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
# add_compile_options(-static)

include_directories (../include )
# link_directories(../lib/static)
link_directories(../lib/dynamic)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(test main.cpp)
# target_link_libraries(test PRIVATE SendFunc db_parser TsAPI vxlapi64 fltk fltk_gl comctl32 wsock32 gdiplus)
target_link_libraries(test PRIVATE SendFunc db_parser TsAPI vxlapi64)
# target_link_libraries(test db_parser)
# so that the compiler knows those functions are implemented in the .a file
# target_compile_definitions(test PUBLIC PARSER_STATIC TS_STATIC SEND_FUNCTION_STATIC)

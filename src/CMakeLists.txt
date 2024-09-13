include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-static)
aux_source_directory (. SRC_LIST)

include_directories (../include )
link_directories(../lib/static)
# link_directories(../lib/dynamic)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(test main.cpp)
# target_link_libraries(test PRIVATE SendFunc db_parser TsAPI vxlapi64 fltk fltk_gl comctl32 wsock32 gdiplus)
target_link_libraries(test PRIVATE SendFunc db_parser TsAPI vxlapi64)
target_compile_definitions(test PUBLIC PARSER_STATIC TS_STATIC SEND_FUNCTION_STATIC)
# 解决报错缺少xxxx.dll
# set(CMAKE_CXX_FLAGS "-static")

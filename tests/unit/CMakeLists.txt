# Unit tests - 单元测试
cmake_minimum_required(VERSION 3.20)

# 创建单元测试可执行文件
add_executable(unit_tests)

# 设置测试属性
target_compile_definitions(unit_tests PRIVATE UNIT_TESTS)

# 添加测试源文件
file(GLOB UNIT_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

target_sources(unit_tests
    PRIVATE
    ${UNIT_TEST_SOURCES}
)

# 链接依赖
target_link_libraries(unit_tests
    PRIVATE
    Qt6::Core
    Qt6::Test
    common
    infrastructure
    domain
    ai
    app
    ui
    data
)

# 添加测试
add_test(
    NAME unit_tests
    COMMAND unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 输出信息
message(STATUS "Unit tests configured")

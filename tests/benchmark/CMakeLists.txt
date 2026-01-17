# Benchmark tests - 性能测试
cmake_minimum_required(VERSION 3.20)

# 创建性能测试可执行文件
add_executable(benchmark_tests)

# 设置测试属性
target_compile_definitions(benchmark_tests PRIVATE BENCHMARK_TESTS)

# 添加测试源文件
file(GLOB BENCHMARK_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

target_sources(benchmark_tests
    PRIVATE
    ${BENCHMARK_TEST_SOURCES}
)

# 链接依赖
target_link_libraries(benchmark_tests
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
    NAME benchmark_tests
    COMMAND benchmark_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 输出信息
message(STATUS "Benchmark tests configured")

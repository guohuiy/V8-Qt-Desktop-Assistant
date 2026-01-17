# Integration tests - 集成测试
cmake_minimum_required(VERSION 3.20)

# 创建集成测试可执行文件
add_executable(integration_tests)

# 设置测试属性
target_compile_definitions(integration_tests PRIVATE INTEGRATION_TESTS)

# 添加测试源文件
file(GLOB INTEGRATION_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

target_sources(integration_tests
    PRIVATE
    ${INTEGRATION_TEST_SOURCES}
)

# 链接依赖
target_link_libraries(integration_tests
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    Qt6::Widgets
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
    NAME integration_tests
    COMMAND integration_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 输出信息
message(STATUS "Integration tests configured")

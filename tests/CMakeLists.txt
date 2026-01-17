# Tests - 测试总目录
cmake_minimum_required(VERSION 3.20)

# 启用测试
enable_testing()

# 包含测试子目录
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(benchmark)

# 输出信息
message(STATUS "Tests configured")

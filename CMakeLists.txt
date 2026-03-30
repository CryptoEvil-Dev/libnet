cmake_minimum_required(VERSION 3.10)
project(libnet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Собираем библиотеку
file(GLOB_RECURSE SOURCES "models/impl/*.cpp" "tun/impl/*.cpp" "types/impl/*.cpp")
add_library(libnet STATIC ${SOURCES})

target_include_directories(libnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 2. Загружаем GoogleTest
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 3. Настройка тестов
enable_testing()


# Тест IPv4
add_executable(unit_test tests/test_ipv4.cpp)
target_link_libraries(unit_test PRIVATE libnet gtest_main)

# Тест uint128_t
add_executable(uint128t_test tests/test_uint128.cpp)
target_link_libraries(uint128t_test PRIVATE libnet gtest_main)


# Переносим бинарник в build/tests
set_target_properties(unit_test uint128t_test PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
)

# Помогаем VS Code найти заголовки gtest
target_include_directories(unit_test PRIVATE 
    ${googletest_SOURCE_DIR}/googletest/include 
    ${googletest_SOURCE_DIR}/googlemock/include
)

add_test(NAME MyIPv4Test COMMAND unit_test)
add_test(NAME UInt128Test COMMAND test_uint128)
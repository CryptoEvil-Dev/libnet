cmake_minimum_required(VERSION 3.10)
project(libnet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Сборка библиотеки
# Используем явное перечисление или GLOB по новой структуре src/
file(GLOB_RECURSE LIB_SOURCES 
    "src/core/*.cpp" 
    "src/network/*.cpp" 
    "src/dev/*.cpp"
)

add_library(net STATIC ${LIB_SOURCES})

add_custom_command(TARGET net POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/include/libnet"
    "${CMAKE_BINARY_DIR}/include/libnet"
    COMMENT "Copying headers to build directory..."
)

# Важно: указываем путь к include так, чтобы внутри файлов 
# можно было писать #include <libnet/core/uint128.hpp>
target_include_directories(net PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 2. Установка (Инсталляция)
install(TARGETS net DESTINATION lib)
install(DIRECTORY "include/libnet" DESTINATION include)

# 3. Тесты (GoogleTest)
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



# 4. Примеры (Examples)
option(LIBNET_BUILD_EXAMPLES "Build library examples" ON)

if(LIBNET_BUILD_EXAMPLES)
    # Ищем все .cpp файлы в папке examples
    file(GLOB EXAMPLE_SOURCES "examples/*.cpp")

    foreach(source_file ${EXAMPLE_SOURCES})
        # Получаем имя файла без расширения (например, "udp_echo")
        get_filename_component(example_name ${source_file} NAME_WE)
        
        # Создаем исполняемый файл
        add_executable(${example_name} ${source_file})
        
        # Линкуем с нашей библиотекой
        target_link_libraries(${example_name} PRIVATE net)
        
        # Кладём скомпилированные примеры в отдельную папку в build
        set_target_properties(${example_name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
        )
        
        message(STATUS "Example added: ${example_name}")
    endforeach()
endif()



enable_testing()

# Функция для быстрого добавления тестов
function(add_libnet_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE net gtest_main)
    set_target_properties(${name} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
endfunction()

add_libnet_test(test_ipv4 tests/test_ipv4.cpp)
add_libnet_test(test_ipv6 tests/test_ipv6.cpp)
add_libnet_test(test_ipv4net tests/test_ipv4net.cpp)
add_libnet_test(test_ipv6net tests/test_ipv6net.cpp)
add_libnet_test(test_uint128 tests/test_uint128.cpp)
add_libnet_test(test_int128 tests/test_int128.cpp)
add_libnet_test(test_bitset tests/test_bitset.cpp)

add_libnet_test(test_udpsocket tests/test_udpsocket.cpp)
add_libnet_test(test_tcp tests/test_tcp.cpp)
add_libnet_test(test_tun tests/test_tun.cpp)
add_libnet_test(test_packet tests/test_packet.cpp)
add_libnet_test(test_eventloop tests/test_eventloop.cpp)

add_test(NAME IPv4Test COMMAND test_ipv4)
add_test(NAME UInt128Test COMMAND test_uint128)
add_test(NAME Int128Test COMMAND test_int128)

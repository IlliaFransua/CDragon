cmake_minimum_required(VERSION 3.10)
project(Framework)

# C++ 17
set(CMAKE_CXX_STANDARD 17)

# OpenSSL variables for MySql
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    message(STATUS "OpenSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
endif()

add_library(Framework
    src/http/http_request.cpp
    src/http/http_response.cpp
    src/http/http_server.cpp

    src/router/router.cpp
    src/router/routes.cpp
    
    src/framework.cpp
)

target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/models)

# Libraries
target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/json-develop/include)
target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/include)

add_executable(app src/main.cpp)

target_link_libraries(app Framework 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64/libmysqlcppconn-static.a
    # ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64/libmysqlcppconn.dylib
    resolv
    ${OPENSSL_LIBRARIES}
)

# ======== Google Test ========
add_subdirectory(tests/googletest)

add_executable(test_executable
    tests/http_request.cpp
    tests/http_response.cpp
    tests/template_renderer.cpp
    # tests/session_repository.cpp
    tests/router.cpp
)

target_link_libraries(test_executable gtest gtest_main Framework)

set_target_properties(test_executable PROPERTIES LINKER_LANGUAGE CXX)

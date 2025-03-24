cmake_minimum_required(VERSION 3.10)
project(Framework)

set(CMAKE_CXX_STANDARD 17)

# ================================
# Copying templates to the build directory
# ================================
file(COPY ${CMAKE_SOURCE_DIR}/src/views DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR})

# ================================
# Adding GoogleTest (via Homebrew)
# ================================
find_package(GTest REQUIRED)

if (NOT GTest_FOUND)
    message(FATAL_ERROR "GoogleTest not found, please install it with Homebrew or provide a local version.")
else ()
    message(STATUS "GoogleTest found at: ${GTest_INCLUDE_DIRS}")
endif ()

# ================================
# Adding bcryptcpp (local version)
# ================================
find_package(bcryptcpp QUIET)

if (NOT bcryptcpp_FOUND)
    message(STATUS "bcryptcpp not found in system, using local version.")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/bcryptcpp)
else ()
    message(STATUS "bcryptcpp found in system.")
endif ()

# ================================
# OpenSSL settings
# ================================
set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)
set(OPENSSL_LIBRARIES /opt/homebrew/opt/openssl@3/lib)
set(OPENSSL_INCLUDE_DIR /opt/homebrew/opt/openssl@3/include)

find_package(OpenSSL REQUIRED)

if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    message(STATUS "OpenSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")
else ()
    message(FATAL_ERROR "OpenSSL not found")
endif ()

# ================================
# MySQL Connector settings
# ================================
find_library(MYSQL_CLIENT_LIB mysqlcppconn HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64)

if (NOT MYSQL_CLIENT_LIB)
    message(FATAL_ERROR "MySQL Connector library not found!")
else ()
    message(STATUS "MySQL Client found: ${MYSQL_CLIENT_LIB}")
endif ()

# ================================
# nlohmann/json settings (header-only)
# ================================
include_directories("/opt/homebrew/include")

find_package(nlohmann_json CONFIG REQUIRED)

# ================================
# Specifying source files for the main program
# ================================
add_library(Framework
        src/http/http_request.cpp
        src/http/http_response.cpp
        src/http/http_server.cpp

        src/router/router.cpp
        src/router/routes.cpp

        src/framework.cpp

        lib/base64/base64.cpp
)

# ================================
# Connecting header files
# ================================
target_include_directories(Framework PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models/include

        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/base64

        ${OPENSSL_INCLUDE_DIR}
        ${GTest_INCLUDE_DIRS}
)

# ================================
# Linking to bcryptcpp
# ================================
target_link_libraries(Framework bcrypt)

# ================================
# Creating the main application
# ================================
add_executable(Server src/main.cpp)

target_link_libraries(Server Framework
        ${MYSQL_CLIENT_LIB}
        resolv
        ${OPENSSL_LIBRARIES}
)

# ================================
# Linking GoogleTest
# ================================
link_directories(/opt/homebrew/opt/googletest/lib)

# ================================
# Specifying source files for tests
# ================================
add_executable(test_executable
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models/include/tables/tests/test_users_table.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models/include/tables/tests/test_roles_table.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models/include/tables/tests/test_user_roles_table.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/include/tests/test_http_request.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tests/test_http_response.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tests/test_router.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tests/test_session_repository.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tests/test_template_renderer.cpp
)

target_include_directories(test_executable PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/include
        ${OPENSSL_INCLUDE_DIR}
        ${GTest_INCLUDE_DIRS}
)

target_link_libraries(test_executable Framework
        gtest gtest_main
        ${MYSQL_CLIENT_LIB}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64/libmysqlcppconn.dylib
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64/libmysqlcppconn-static.a
        ${OPENSSL_LIBRARIES}
)

set_target_properties(test_executable PROPERTIES LINKER_LANGUAGE CXX)

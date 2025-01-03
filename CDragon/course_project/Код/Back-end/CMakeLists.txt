cmake_minimum_required(VERSION 3.10)
project(Framework)

set(CMAKE_CXX_STANDARD 17)

# ================================
# Copying templates to the build directory (but i added them manually anyway)
# ================================
file(COPY ${CMAKE_SOURCE_DIR}/src/views DESTINATION ${CMAKE_BINARY_DIR})

# ================================
# Adding GoogleTest and bcryptcpp
# ================================
add_subdirectory(lib/googletest)
add_subdirectory(lib/bcryptcpp)

# ================================
# OpenSSL settings
# ================================
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    message(STATUS "OpenSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found")
endif()

# ================================
# MySQL Connector settings
# ================================
find_library(MYSQL_CLIENT mysqlcppconn HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64)

if(NOT MYSQL_CLIENT)
    message(FATAL_ERROR "MySQL Connector library not found!")
else()
    message(STATUS "MySQL Client found: ${MYSQL_CLIENT}")
endif()



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

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googlemock/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest/googletest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/json-develop/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/base64
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/bcryptcpp/include
    ${OPENSSL_INCLUDE_DIR}  # OpenSSL includes
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
    ${MYSQL_CLIENT}
    resolv
    ${OPENSSL_LIBRARIES}
)

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
    # ${CMAKE_CURRENT_SOURCE_DIR}/include/**/test_*.cpp
)

target_link_libraries(test_executable Framework gtest gtest_main
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64/libmysqlcppconn.dylib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/mysql-connector-c++-9.1.0-macos14-arm64/lib64/libmysqlcppconn-static.a
    ${OPENSSL_LIBRARIES}
)

set_target_properties(test_executable PROPERTIES LINKER_LANGUAGE CXX)




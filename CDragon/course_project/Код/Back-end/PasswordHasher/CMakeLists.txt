cmake_minimum_required(VERSION 3.10)

project(PasswordHasher)

set(CMAKE_CXX_STANDARD 17)

include_directories(Bcrypt.cpp-master/include)
file(GLOB SOURCES
    Bcrypt.cpp-master/src/*.cpp
    main.cpp
)

add_executable(PasswordHasher ${SOURCES})

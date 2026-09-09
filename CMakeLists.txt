cmake_minimum_required(VERSION 3.20)

project(mini-git
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

find_package(OpenSSL REQUIRED)

add_executable(mini-git
    src/main.cpp
    src/Repository.cpp
    src/Hash.cpp
)

target_include_directories(mini-git
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(mini-git
    PRIVATE
        OpenSSL::Crypto
)

add_executable(hash-tests
    tests/HashTests.cpp
    src/Hash.cpp
)

target_include_directories(hash-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(hash-tests
    PRIVATE
        OpenSSL::Crypto
)

add_test(
    NAME HashTests
    COMMAND hash-tests
)
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


# ============================================================
# Main executable
# ============================================================

add_executable(mini-git
    src/main.cpp
    src/Repository.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/FileReader.cpp
    src/ObjectDatabase.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Status.cpp
)

target_include_directories(mini-git
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(mini-git
    PRIVATE
        OpenSSL::Crypto
)


# ============================================================
# Hash tests
# ============================================================

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


# ============================================================
# Object tests
# ============================================================

add_executable(object-tests
    tests/ObjectTests.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/FileReader.cpp
    src/ObjectDatabase.cpp
)

target_include_directories(object-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(object-tests
    PRIVATE
        OpenSSL::Crypto
)

add_test(
    NAME ObjectTests
    COMMAND object-tests
)


# ============================================================
# FileReader tests
# ============================================================

add_executable(file-reader-tests
    tests/FileReaderTests.cpp
    src/FileReader.cpp
)

target_include_directories(file-reader-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME FileReaderTests
    COMMAND file-reader-tests
)


# ============================================================
# Blob tests
# ============================================================

add_executable(blob-tests
    tests/BlobTests.cpp
    src/Blob.cpp
    src/FileReader.cpp
)

target_include_directories(blob-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME BlobTests
    COMMAND blob-tests
)


# ============================================================
# TreeBuilder tests
# ============================================================

add_executable(tree-builder-tests
    tests/TreeBuilderTests.cpp
    src/TreeBuilder.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/ObjectDatabase.cpp
    src/Hash.cpp
    src/Index.cpp
)

target_include_directories(tree-builder-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(tree-builder-tests
    PRIVATE
        OpenSSL::Crypto
)

add_test(
    NAME TreeBuilderTests
    COMMAND tree-builder-tests
)


# ============================================================
# Index tests
# ============================================================

add_executable(index-tests
    tests/IndexTests.cpp
    src/Index.cpp
)

target_include_directories(index-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME IndexTests
    COMMAND index-tests
)


# ============================================================
# Status tests
# ============================================================

add_executable(status-tests
    tests/StatusTests.cpp
    src/Status.cpp
    src/Index.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
)

target_include_directories(status-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(status-tests
    PRIVATE
        OpenSSL::Crypto
)

add_test(
    NAME StatusTests
    COMMAND status-tests
)


# ============================================================
# Log tests
# ============================================================

add_executable(log-tests
    tests/LogTests.cpp
    src/Commit.cpp
    src/ObjectDatabase.cpp
    src/Hash.cpp
    src/Repository.cpp
)

target_include_directories(log-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(log-tests
    PRIVATE
        OpenSSL::Crypto
)

add_test(
    NAME LogTests
    COMMAND log-tests
)


# ============================================================
# Commit tests
# ============================================================

add_executable(commit-tests
    tests/CommitTests.cpp
    src/Commit.cpp
)

target_include_directories(commit-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME CommitTests
    COMMAND commit-tests
)


# ============================================================
# Repository tests
# ============================================================

add_executable(repository-tests
    tests/RepositoryTests.cpp
    src/Repository.cpp
)

target_include_directories(repository-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME RepositoryTests
    COMMAND repository-tests
)
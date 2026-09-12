cmake_minimum_required(VERSION 3.20)

project(
    mini-git
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)

# ============================================================
# Main executable
# ============================================================

add_executable(
    mini-git
    src/main.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/Diff.cpp
    src/Merge.cpp
    src/FileReader.cpp
    src/ObjectDatabase.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Status.cpp
    src/Inspector.cpp
    src/Explainer.cpp
    src/Graph.cpp
    src/Stats.cpp
    src/Analyzer.cpp
    src/Impact.cpp
    src/Doctor.cpp
    src/StorageAnalyzer.cpp
    src/Benchmark.cpp
    src/ObjectType.cpp
    src/Reachability.cpp
    src/IntegrityChecker.cpp
    src/Ancestry.cpp
    src/GarbageCollector.cpp
)

target_include_directories(
    mini-git
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
    mini-git
    PRIVATE
    OpenSSL::Crypto
)

# ============================================================
# Testing
# ============================================================

include(CTest)

function(add_mini_git_test target)
    cmake_parse_arguments(
        TEST
        "NO_OPENSSL"
        ""
        ""
        ${ARGN}
    )

    add_executable(
        ${target}
        ${TEST_UNPARSED_ARGUMENTS}
    )

    target_include_directories(
        ${target}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
    )

    if(NOT TEST_NO_OPENSSL)
        target_link_libraries(
            ${target}
            PRIVATE
            OpenSSL::Crypto
        )
    endif()

    add_test(
        NAME ${target}
        COMMAND ${target}
    )
endfunction()

# ============================================================
# Phase 1 - Core Tests
# ============================================================

add_mini_git_test(
    hash-tests
    tests/HashTests.cpp
    src/Hash.cpp
)

add_mini_git_test(
    object-tests
    tests/ObjectTests.cpp
    src/Blob.cpp
    src/ObjectDatabase.cpp
    src/Hash.cpp
    src/FileReader.cpp
    src/Tree.cpp
    src/Commit.cpp
)

add_mini_git_test(
    file-reader-tests
    tests/FileReaderTests.cpp
    src/FileReader.cpp
    NO_OPENSSL
)

add_mini_git_test(
    blob-tests
    tests/BlobTests.cpp
    src/Blob.cpp
    src/FileReader.cpp
    NO_OPENSSL
)

add_mini_git_test(
    tree-builder-tests
    tests/TreeBuilderTests.cpp
    src/TreeBuilder.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/ObjectDatabase.cpp
    src/Index.cpp
)

add_mini_git_test(
    index-tests
    tests/IndexTests.cpp
    src/Index.cpp
    NO_OPENSSL
)

add_mini_git_test(
    status-tests
    tests/StatusTests.cpp
    src/Status.cpp
    src/Index.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
)

add_mini_git_test(
    log-tests
    tests/LogTests.cpp
    src/Commit.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
)

add_mini_git_test(
    commit-tests
    tests/CommitTests.cpp
    src/Commit.cpp
    NO_OPENSSL
)

add_mini_git_test(
    reference-tests
    tests/ReferenceTests.cpp
    src/Reference.cpp
    NO_OPENSSL
)

add_mini_git_test(
    repository-tests
    tests/RepositoryTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Hash.cpp
)

# ============================================================
# Core Repository Tests
# ============================================================

add_mini_git_test(
    branch-tests
    tests/BranchTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
)

add_mini_git_test(
    checkout-tests
    tests/CheckoutTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
)

add_mini_git_test(
    diff-tests
    tests/DiffTests.cpp
    src/Diff.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Index.cpp
    src/Hash.cpp
)

add_mini_git_test(
    merge-tests
    tests/MergeTests.cpp
    src/Merge.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Hash.cpp
)

# ============================================================
# Phase 16 - Merge State Tests
# ============================================================

add_mini_git_test(
    merge-state-tests
    tests/MergeStateTests.cpp
    src/Merge.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Hash.cpp
)

# ============================================================
# Phase 17 - Tag Tests
# ============================================================

add_mini_git_test(
    tag-tests
    tests/TagTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Hash.cpp
)

# ============================================================
# Phase 18 - Educational Intelligence Tests
# ============================================================

add_mini_git_test(
    inspector-tests
    tests/InspectorTests.cpp
    src/Inspector.cpp
    src/ObjectDatabase.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/Hash.cpp
    src/FileReader.cpp
    src/ObjectType.cpp
)

add_mini_git_test(
    explainer-tests
    tests/ExplainerTests.cpp
    src/Explainer.cpp
    NO_OPENSSL
)

add_mini_git_test(
    graph-tests
    tests/GraphTests.cpp
    src/Graph.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Hash.cpp
)

add_mini_git_test(
    stats-tests
    tests/StatsTests.cpp
    src/Stats.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/TreeBuilder.cpp
    src/Index.cpp
    src/Hash.cpp
    src/ObjectType.cpp
)

# ============================================================
# Phase 19 - Repository Intelligence Tests
# ============================================================

add_mini_git_test(
    analyzer-tests
    tests/AnalyzerTests.cpp
    src/Analyzer.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
)

add_mini_git_test(
    impact-tests
    tests/ImpactTests.cpp
    src/Impact.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
)

add_mini_git_test(
    doctor-tests
    tests/DoctorTests.cpp
    src/Doctor.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
    src/ObjectType.cpp
)

add_mini_git_test(
    storage-tests
    tests/StorageAnalyzerTests.cpp
    src/StorageAnalyzer.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
    src/ObjectType.cpp
)

add_mini_git_test(
    benchmark-tests
    tests/BenchmarkTests.cpp
    src/Benchmark.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/ObjectDatabase.cpp
    src/Commit.cpp
    src/Tree.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
    src/Index.cpp
)

add_mini_git_test(
    object-type-tests
    tests/ObjectTypeTests.cpp
    src/ObjectType.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/FileReader.cpp
    src/Hash.cpp
)

# ============================================================
# Phase 19 - Repository Integrity & Reachability Tests
# ============================================================

add_mini_git_test(
    reachability-tests
    tests/ReachabilityTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/ObjectDatabase.cpp
    src/Index.cpp
    src/FileReader.cpp
    src/Reachability.cpp
)

add_mini_git_test(
    integrity-tests
    tests/IntegrityTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/ObjectDatabase.cpp
    src/Index.cpp
    src/FileReader.cpp
    src/Reachability.cpp
    src/IntegrityChecker.cpp
    src/ObjectType.cpp
)

add_mini_git_test(
    fsck-tests
    tests/FsckTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/ObjectDatabase.cpp
    src/Index.cpp
    src/FileReader.cpp
    src/Reachability.cpp
    src/IntegrityChecker.cpp
    src/ObjectType.cpp
)

add_mini_git_test(
    ancestry-tests
    tests/AncestryTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/ObjectDatabase.cpp
    src/Index.cpp
    src/FileReader.cpp
    src/Ancestry.cpp
)

add_mini_git_test(
    garbage-collection-tests
    tests/GarbageCollectionTests.cpp
    src/Repository.cpp
    src/Reference.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/ObjectDatabase.cpp
    src/Index.cpp
    src/FileReader.cpp
    src/Reachability.cpp
    src/GarbageCollector.cpp
)
cmake_minimum_required(VERSION 3.20)

project(
    mini-git
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    include
)

target_link_libraries(
    mini-git
    PRIVATE
    OpenSSL::Crypto
)

# ============================================================
# Testing
# ============================================================

enable_testing()

add_executable(
    hash-tests
    tests/HashTests.cpp
    src/Hash.cpp
)

target_include_directories(
    hash-tests
    PRIVATE
    include
)

target_link_libraries(
    hash-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME HashTests
    COMMAND hash-tests
)

add_executable(
    object-tests
    tests/ObjectTests.cpp
    src/Blob.cpp
    src/ObjectDatabase.cpp
    src/Hash.cpp
    src/FileReader.cpp
    src/Tree.cpp
    src/Commit.cpp
)

target_include_directories(
    object-tests
    PRIVATE
    include
)

target_link_libraries(
    object-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME ObjectTests
    COMMAND object-tests
)

add_executable(
    file-reader-tests
    tests/FileReaderTests.cpp
    src/FileReader.cpp
)

target_include_directories(
    file-reader-tests
    PRIVATE
    include
)

add_test(
    NAME FileReaderTests
    COMMAND file-reader-tests
)

add_executable(
    blob-tests
    tests/BlobTests.cpp
    src/Blob.cpp
    src/FileReader.cpp
)

target_include_directories(
    blob-tests
    PRIVATE
    include
)

add_test(
    NAME BlobTests
    COMMAND blob-tests
)

add_executable(
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

target_include_directories(
    tree-builder-tests
    PRIVATE
    include
)

target_link_libraries(
    tree-builder-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME TreeBuilderTests
    COMMAND tree-builder-tests
)

add_executable(
    index-tests
    tests/IndexTests.cpp
    src/Index.cpp
)

target_include_directories(
    index-tests
    PRIVATE
    include
)

add_test(
    NAME IndexTests
    COMMAND index-tests
)

add_executable(
    status-tests
    tests/StatusTests.cpp
    src/Status.cpp
    src/Index.cpp
    src/Blob.cpp
    src/FileReader.cpp
    src/Hash.cpp
)

target_include_directories(
    status-tests
    PRIVATE
    include
)

target_link_libraries(
    status-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME StatusTests
    COMMAND status-tests
)

add_executable(
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

target_include_directories(
    log-tests
    PRIVATE
    include
)

target_link_libraries(
    log-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME LogTests
    COMMAND log-tests
)

add_executable(
    commit-tests
    tests/CommitTests.cpp
    src/Commit.cpp
)

target_include_directories(
    commit-tests
    PRIVATE
    include
)

add_test(
    NAME CommitTests
    COMMAND commit-tests
)

add_executable(
    reference-tests
    tests/ReferenceTests.cpp
    src/Reference.cpp
)

target_include_directories(
    reference-tests
    PRIVATE
    include
)

add_test(
    NAME ReferenceTests
    COMMAND reference-tests
)

add_executable(
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

target_include_directories(
    repository-tests
    PRIVATE
    include
)

target_link_libraries(
    repository-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME RepositoryTests
    COMMAND repository-tests
)

add_executable(
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

target_include_directories(
    branch-tests
    PRIVATE
    include
)

target_link_libraries(
    branch-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME BranchTests
    COMMAND branch-tests
)

add_executable(
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

target_include_directories(
    checkout-tests
    PRIVATE
    include
)

target_link_libraries(
    checkout-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME CheckoutTests
    COMMAND checkout-tests
)

add_executable(
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

target_include_directories(
    diff-tests
    PRIVATE
    include
)

target_link_libraries(
    diff-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME DiffTests
    COMMAND diff-tests
)

add_executable(
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

target_include_directories(
    merge-tests
    PRIVATE
    include
)

target_link_libraries(
    merge-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME MergeTests
    COMMAND merge-tests
)

# ============================================================
# Phase 16 - Merge State Tests
# ============================================================

add_executable(
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

target_include_directories(
    merge-state-tests
    PRIVATE
    include
)

target_link_libraries(
    merge-state-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME MergeStateTests
    COMMAND merge-state-tests
)

# ============================================================
# Phase 17 - Tag Tests
# ============================================================

add_executable(
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

target_include_directories(
    tag-tests
    PRIVATE
    include
)

target_link_libraries(
    tag-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME TagTests
    COMMAND tag-tests
)

# ============================================================
# Phase 18 - Educational Intelligence Tests
# ============================================================

add_executable(
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

target_include_directories(
    inspector-tests
    PRIVATE
    include
)

target_link_libraries(
    inspector-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME InspectorTests
    COMMAND inspector-tests
)

add_executable(
    explainer-tests
    tests/ExplainerTests.cpp
    src/Explainer.cpp
)

target_include_directories(
    explainer-tests
    PRIVATE
    include
)

add_test(
    NAME ExplainerTests
    COMMAND explainer-tests
)

add_executable(
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

target_include_directories(
    graph-tests
    PRIVATE
    include
)

target_link_libraries(
    graph-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME GraphTests
    COMMAND graph-tests
)

add_executable(
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

target_include_directories(
    stats-tests
    PRIVATE
    include
)

target_link_libraries(
    stats-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME StatsTests
    COMMAND stats-tests
)

# ============================================================
# Phase 19 - Unique Repository Intelligence Tests
# ============================================================

add_executable(
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

target_include_directories(
    analyzer-tests
    PRIVATE
    include
)

target_link_libraries(
    analyzer-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME AnalyzerTests
    COMMAND analyzer-tests
)

add_executable(
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

target_include_directories(
    impact-tests
    PRIVATE
    include
)

target_link_libraries(
    impact-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME ImpactTests
    COMMAND impact-tests
)

add_executable(
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
)

target_include_directories(
    doctor-tests
    PRIVATE
    include
)

target_link_libraries(
    doctor-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME DoctorTests
    COMMAND doctor-tests
)

add_executable(
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
)

target_include_directories(
    storage-tests
    PRIVATE
    include
)

target_link_libraries(
    storage-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME StorageAnalyzerTests
    COMMAND storage-tests
)

add_executable(
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

target_include_directories(
    benchmark-tests
    PRIVATE
    include
)

target_link_libraries(
    benchmark-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME BenchmarkTests
    COMMAND benchmark-tests
)

add_executable(
    object-type-tests
    tests/ObjectTypeTests.cpp
    src/ObjectType.cpp
    src/Blob.cpp
    src/Tree.cpp
    src/Commit.cpp
    src/FileReader.cpp
    src/Hash.cpp
)

target_include_directories(
    object-type-tests
    PRIVATE
    include
)

target_link_libraries(
    object-type-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME ObjectTypeTests
    COMMAND object-type-tests
)

# ============================================================
# Phase 19 - Repository Integrity & Reachability Tests
# ============================================================

add_executable(
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

target_include_directories(
    reachability-tests
    PRIVATE
    include
)

target_link_libraries(
    reachability-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME ReachabilityTests
    COMMAND reachability-tests
)


add_executable(
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

target_include_directories(
    integrity-tests
    PRIVATE
    include
)

target_link_libraries(
    integrity-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME IntegrityTests
    COMMAND integrity-tests
)


add_executable(
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

target_include_directories(
    fsck-tests
    PRIVATE
    include
)

target_link_libraries(
    fsck-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME FsckTests
    COMMAND fsck-tests
)


add_executable(
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

target_include_directories(
    ancestry-tests
    PRIVATE
    include
)

target_link_libraries(
    ancestry-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME AncestryTests
    COMMAND ancestry-tests
)


add_executable(
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

target_include_directories(
    garbage-collection-tests
    PRIVATE
    include
)

target_link_libraries(
    garbage-collection-tests
    PRIVATE
    OpenSSL::Crypto
)

add_test(
    NAME GarbageCollectionTests
    COMMAND garbage-collection-tests
)
#include "Blob.hpp"
#include "Commit.hpp"
#include "Diff.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {

void write_file(
    const std::filesystem::path& path,
    const std::string& content
) {
    std::filesystem::create_directories(
        path.parent_path()
    );

    std::ofstream file(
        path,
        std::ios::binary
    );

    assert(file);

    file << content;
}

std::string create_commit(
    Repository& repository,
    const std::string& filename,
    const std::string& content,
    const std::string& parent
) {
    ObjectDatabase database(
        repository.git_directory()
    );

    Blob blob(content);

    const std::string blob_id =
        database.store(blob);

    Tree tree;

    tree.add_entry(
        TreeEntry{
            filename,
            blob_id,
            false
        }
    );

    const std::string tree_id =
        database.store(tree);

    Commit commit(
        tree_id,
        parent,
        "test",
        "test commit"
    );

    return database.store(commit);
}

} // namespace

int main()
{
    const auto test_directory =
        std::filesystem::temp_directory_path()
        / "mini-git-diff-tests";

    std::filesystem::remove_all(
        test_directory
    );

    std::filesystem::create_directories(
        test_directory
    );

    Repository repository(
        test_directory
    );

    repository.initialize();

    /*
     * --------------------------------------------------------
     * Create initial working tree
     * --------------------------------------------------------
     */

    write_file(
        test_directory / "hello.txt",
        "hello\nworld\n"
    );

    /*
     * --------------------------------------------------------
     * Create first commit
     * --------------------------------------------------------
     */

    const std::string first_commit =
        create_commit(
            repository,
            "hello.txt",
            "hello\nworld\n",
            ""
        );

    repository.update_branch(
        "main",
        first_commit
    );

    /*
     * --------------------------------------------------------
     * Working Tree vs Index
     * --------------------------------------------------------
     */

    Diff diff(repository);

    {
        std::string output =
            diff.working_tree_vs_index();

        assert(
            output.empty()
        );
    }

    /*
     * Modify working tree.
     */

    write_file(
        test_directory / "hello.txt",
        "hello\nMini Git\n"
    );

    {
        std::string output =
            diff.working_tree_vs_index();

        /*
         * Index is currently empty,
         * so the working tree file appears
         * as an addition relative to the index.
         */

        assert(
            output.find(
                "+hello"
            ) != std::string::npos
        );

        assert(
            output.find(
                "+Mini Git"
            ) != std::string::npos
        );
    }

    /*
     * --------------------------------------------------------
     * Add file to index
     * --------------------------------------------------------
     */

    Blob staged_blob(
        "hello\nMini Git\n"
    );

    ObjectDatabase database(
        repository.git_directory()
    );

    const std::string staged_blob_id =
        database.store(staged_blob);

    Index index(
        repository.git_directory()
    );

    index.load();

    index.add(
        IndexEntry{
            "hello.txt",
            staged_blob_id
        }
    );

    index.save();

    /*
     * Working tree and index now match.
     */

    {
        std::string output =
            diff.working_tree_vs_index();

        assert(
            output.empty()
        );
    }

    /*
     * --------------------------------------------------------
     * Index vs HEAD
     * --------------------------------------------------------
     */

    {
        std::string output =
            diff.index_vs_head();

        assert(
            output.find(
                "-world"
            ) != std::string::npos
        );

        assert(
            output.find(
                "+Mini Git"
            ) != std::string::npos
        );
    }

    /*
     * --------------------------------------------------------
     * Commit vs working tree
     * --------------------------------------------------------
     */

    {
        std::string output =
            diff.commit_vs_working_tree(
                first_commit
            );

        assert(
            output.find(
                "-world"
            ) != std::string::npos
        );

        assert(
            output.find(
                "+Mini Git"
            ) != std::string::npos
        );
    }

    /*
     * --------------------------------------------------------
     * Create second commit
     * --------------------------------------------------------
     */

    const std::string second_commit =
        create_commit(
            repository,
            "hello.txt",
            "hello\nMini Git\n",
            first_commit
        );

    /*
     * --------------------------------------------------------
     * Commit vs Commit
     * --------------------------------------------------------
     */

    {
        std::string output =
            diff.commit_vs_commit(
                first_commit,
                second_commit
            );

        assert(
            output.find(
                "-world"
            ) != std::string::npos
        );

        assert(
            output.find(
                "+Mini Git"
            ) != std::string::npos
        );
    }

    /*
     * Same commit should produce no diff.
     */

    {
        std::string output =
            diff.commit_vs_commit(
                first_commit,
                first_commit
            );

        assert(
            output.empty()
        );
    }

    std::filesystem::remove_all(
        test_directory
    );

    std::cout
        << "Diff tests passed\n";

    return 0;
}
#include "Explainer.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <stdexcept>

namespace {

std::string normalize(
    std::string value
) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character) {
            return static_cast<char>(
                std::tolower(character)
            );
        }
    );

    return value;
}

}

std::string Explainer::explain(
    const std::string& command
) {

    const std::string name =
        normalize(command);

    if (name == "init") {
        return
            "mini-git init\n\n"
            "Creates the repository metadata directory.\n\n"
            "Pipeline:\n"
            "  Working Directory\n"
            "       |\n"
            "       v\n"
            "  .mini-git/\n"
            "       |\n"
            "       +-- objects/\n"
            "       +-- refs/heads/\n"
            "       +-- HEAD\n\n"
            "HEAD initially points to refs/heads/main.";
    }

    if (name == "add") {
        return
            "mini-git add <file>\n\n"
            "Stages a file by converting its contents into "
            "an immutable Blob object.\n\n"
            "Pipeline:\n"
            "  Working Tree\n"
            "       |\n"
            "       v\n"
            "  FileReader\n"
            "       |\n"
            "       v\n"
            "     Blob\n"
            "       |\n"
            "       v\n"
            "  serialize()\n"
            "       |\n"
            "       v\n"
            "    SHA-256\n"
            "       |\n"
            "       v\n"
            "  Object Database\n"
            "       |\n"
            "       v\n"
            "     Index\n\n"
            "The Index stores the path and Blob object ID.";
    }

    if (name == "status") {
        return
            "mini-git status\n\n"
            "Compares the Index with the Working Tree.\n\n"
            "  Index\n"
            "    |\n"
            "    | compare\n"
            "    v\n"
            "Working Tree\n\n"
            "The current implementation can identify:\n"
            "  - modified files\n"
            "  - deleted files\n"
            "  - untracked files\n"
            "  - merge conflicts";
    }

    if (name == "commit") {
        return
            "mini-git commit -m <message>\n\n"
            "Creates a new immutable snapshot.\n\n"
            "  Index\n"
            "    |\n"
            "    v\n"
            "TreeBuilder\n"
            "    |\n"
            "    v\n"
            "   Tree\n"
            "    |\n"
            "    v\n"
            "  Commit\n"
            "    |\n"
            "    v\n"
            "Object Database\n"
            "    |\n"
            "    v\n"
            "Branch Reference\n"
            "    |\n"
            "    v\n"
            "   HEAD";
    }

    if (name == "log") {
        return
            "mini-git log\n\n"
            "Starts at the current HEAD commit and follows "
            "commit parent references.\n\n"
            "  HEAD\n"
            "    |\n"
            "    v\n"
            " Commit\n"
            "    |\n"
            "    v\n"
            " Parent\n"
            "    |\n"
            "    v\n"
            " Parent\n\n"
            "Merge commits can contain multiple parents.";
    }

    if (name == "branch") {
        return
            "mini-git branch\n\n"
            "A branch is a named reference pointing to a commit.\n\n"
            "  branch name\n"
            "       |\n"
            "       v\n"
            "   Commit ID\n\n"
            "Creating a branch does not copy the commit history.";
    }

    if (name == "checkout") {
        return
            "mini-git checkout <branch>\n\n"
            "Moves HEAD to another branch and restores the "
            "branch's committed tree.\n\n"
            "  HEAD\n"
            "    |\n"
            "    v\n"
            " Branch Reference\n"
            "    |\n"
            "    v\n"
            " Commit\n"
            "    |\n"
            "    v\n"
            " Tree\n"
            "    |\n"
            "    v\n"
            "Working Tree";
    }

    if (name == "diff") {
        return
            "mini-git diff\n\n"
            "Converts repository states into normalized snapshots "
            "and compares them.\n\n"
            "  Repository State\n"
            "        |\n"
            "        v\n"
            "     Snapshot\n"
            "        |\n"
            "        v\n"
            "      Compare\n"
            "        |\n"
            "        v\n"
            "       LCS\n"
            "        |\n"
            "        v\n"
            "  Unified Diff";
    }

    if (name == "merge") {
        return
            "mini-git merge <branch>\n\n"
            "Combines two histories using ancestry and a "
            "three-way comparison.\n\n"
            "  Current Commit      Target Commit\n"
            "        |                  |\n"
            "        +--------+---------+\n"
            "                 |\n"
            "                 v\n"
            "             Merge Base\n"
            "                 |\n"
            "                 v\n"
            "          Three-Way Merge\n"
            "             /       \\\n"
            "            /         \\\n"
            "         Clean      Conflict\n"
            "           |\n"
            "           v\n"
            "      Merge Commit";
    }

    if (name == "tag") {
        return
            "mini-git tag <name>\n\n"
            "Creates a named reference to a commit.\n\n"
            "  Tag\n"
            "   |\n"
            "   v\n"
            " Commit\n\n"
            "The tag does not copy the commit or its objects.";
    }

    throw std::invalid_argument(
        "Unknown command for explain: " + command
    );
}
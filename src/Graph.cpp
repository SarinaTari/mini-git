#include "Graph.hpp"

#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

Graph::Graph(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Graph::render() const {

    ObjectDatabase database(
        repository_.git_directory()
    );

    std::map<
        std::string,
        std::set<std::string>
    > labels;

    const std::string head =
        repository_.head_commit();

    if (!head.empty()) {
        labels[head].insert("HEAD");
    }

    for (const auto& branch :
         repository_.branches()) {

        Reference reference(
            repository_.git_directory(),
            "refs/heads/" + branch
        );

        if (reference.exists()) {
            labels[reference.read()]
                .insert(branch);
        }
    }

    for (const auto& tag :
         repository_.tags()) {

        Reference reference(
            repository_.git_directory(),
            "refs/tags/" + tag
        );

        if (reference.exists()) {
            labels[reference.read()]
                .insert("tag:" + tag);
        }
    }

    if (head.empty()) {
        return "No commits yet.\n";
    }

    std::ostringstream output;

    std::set<std::string> visited;
    std::vector<std::string> pending;
    pending.push_back(head);

    while (!pending.empty()) {

        const std::string current =
            pending.front();

        pending.erase(
            pending.begin()
        );

        if (!visited.insert(current).second) {
            continue;
        }

        const Commit commit =
            Commit::deserialize(
                database.read(current)
            );

        output
            << "* "
            << current.substr(
                0,
                std::min<std::size_t>(
                    8,
                    current.size()
                )
            );

        if (!labels[current].empty()) {

            output << " (";

            bool first = true;

            for (const auto& label :
                 labels[current]) {

                if (!first) {
                    output << ", ";
                }

                output << label;
                first = false;
            }

            output << ")";
        }

        output
            << " "
            << commit.message()
            << '\n';

        for (const auto& parent :
             commit.parent_ids()) {

            output
                << "|\\\n";

            pending.push_back(parent);
        }
    }

    return output.str();
}
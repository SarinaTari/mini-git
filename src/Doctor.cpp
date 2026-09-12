#include "Doctor.hpp"

#include "Commit.hpp"
#include "ObjectDatabase.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <filesystem>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct Report {

    std::size_t passed = 0;

    std::size_t warnings = 0;

    std::size_t errors = 0;

};

void mark_pass(
    Report& report
) {
    ++report.passed;
}

void mark_warning(
    Report& report
) {
    ++report.warnings;
}

void mark_error(
    Report& report
) {
    ++report.errors;
}

void collect_reachable(
    ObjectDatabase& database,
    const std::string& object_id,
    std::set<std::string>& reachable
) {
    if (object_id.empty()) {
        return;
    }

    if (!reachable.insert(object_id).second) {
        return;
    }

    if (!database.exists(object_id)) {
        return;
    }

    const std::string data =
        database.read(object_id);

    if (data.rfind("blob ", 0) == 0) {
        return;
    }

    if (data.rfind("tree ", 0) != 0) {
        return;
    }

    try {

        const Commit commit =
            Commit::deserialize(data);

        collect_reachable(
            database,
            commit.tree_id(),
            reachable
        );

        for (const auto& parent :
             commit.parent_ids()) {

            collect_reachable(
                database,
                parent,
                reachable
            );
        }

        return;
    }
    catch (...) {
    }

    try {

        const Tree tree =
            Tree::deserialize(data);

        for (const auto& entry :
             tree.entries()) {

            collect_reachable(
                database,
                entry.object_id,
                reachable
            );
        }
    }
    catch (...) {
    }
}

}

Doctor::Doctor(
    const Repository& repository
)
    : repository_(repository) {
}

std::string Doctor::render() const {

    ObjectDatabase database(
        repository_.git_directory()
    );

    Report report;

    std::ostringstream output;

    output
        << "Mini Git Repository Doctor\n"
        << "==========================\n\n";

    output << "Repository structure:\n";

    const auto objects =
        repository_.git_directory()
        / "objects";

    const auto heads =
        repository_.git_directory()
        / "refs"
        / "heads";

    const auto tags =
        repository_.git_directory()
        / "refs"
        / "tags";

    const auto head_file =
        repository_.git_directory()
        / "HEAD";

    if (std::filesystem::is_directory(
            objects
        )) {

        output
            << "  [OK] objects directory\n";

        mark_pass(report);
    }
    else {

        output
            << "  [ERROR] objects directory missing\n";

        mark_error(report);
    }

    if (std::filesystem::is_directory(
            heads
        )) {

        output
            << "  [OK] branch references\n";

        mark_pass(report);
    }
    else {

        output
            << "  [ERROR] branch references missing\n";

        mark_error(report);
    }

    if (std::filesystem::is_directory(
            tags
        )) {

        output
            << "  [OK] tag references\n";

        mark_pass(report);
    }
    else {

        output
            << "  [WARNING] tag references missing\n";

        mark_warning(report);
    }

    if (
        std::filesystem::is_regular_file(
            head_file
        )
    ) {

        output
            << "  [OK] HEAD\n";

        mark_pass(report);
    }
    else {

        output
            << "  [ERROR] HEAD missing\n";

        mark_error(report);
    }

    output << "\nReferences:\n";

    std::set<std::string> referenced_commits;

    for (const auto& branch :
         repository_.branches()) {

        Reference reference(
            repository_.git_directory(),
            "refs/heads/" + branch
        );

        if (!reference.exists()) {
            continue;
        }

        const std::string id =
            reference.read();

        if (database.exists(id)) {

            output
                << "  [OK] branch "
                << branch
                << '\n';

            referenced_commits.insert(id);

            mark_pass(report);
        }
        else {

            output
                << "  [ERROR] branch "
                << branch
                << " points to missing object\n";

            mark_error(report);
        }
    }

    for (const auto& tag :
         repository_.tags()) {

        Reference reference(
            repository_.git_directory(),
            "refs/tags/" + tag
        );

        if (!reference.exists()) {
            continue;
        }

        const std::string id =
            reference.read();

        if (database.exists(id)) {

            output
                << "  [OK] tag "
                << tag
                << '\n';

            referenced_commits.insert(id);

            mark_pass(report);
        }
        else {

            output
                << "  [ERROR] tag "
                << tag
                << " points to missing object\n";

            mark_error(report);
        }
    }

    const std::string head =
        repository_.head_commit();

    if (!head.empty()) {

        if (database.exists(head)) {

            output
                << "  [OK] HEAD target\n";

            referenced_commits.insert(head);

            mark_pass(report);
        }
        else {

            output
                << "  [ERROR] HEAD target missing\n";

            mark_error(report);
        }
    }

    std::set<std::string> reachable;

    for (const auto& id :
         referenced_commits) {

        collect_reachable(
            database,
            id,
            reachable
        );
    }

    output
        << "\nReachability:\n";

    std::size_t unreachable = 0;

    for (const auto& id :
         database.object_ids()) {

        if (
            reachable.find(id) ==
            reachable.end()
        ) {
            ++unreachable;
        }
    }

    if (unreachable == 0) {

        output
            << "  [OK] no unreachable objects\n";

        mark_pass(report);
    }
    else {

        output
            << "  [WARNING] "
            << unreachable
            << " unreachable object";

        if (unreachable != 1) {
            output << 's';
        }

        output << '\n';

        mark_warning(report);
    }

    output
        << "\nSummary:\n"
        << "  Passed:   "
        << report.passed
        << '\n'
        << "  Warnings: "
        << report.warnings
        << '\n'
        << "  Errors:   "
        << report.errors
        << '\n';

    if (report.errors > 0) {

        output
            << "\nHealth: ERROR\n";
    }
    else if (report.warnings > 0) {

        output
            << "\nHealth: WARNING\n";
    }
    else {

        output
            << "\nHealth: GOOD\n";
    }

    return output.str();
}
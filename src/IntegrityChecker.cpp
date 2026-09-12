#include "IntegrityChecker.hpp"

#include "Blob.hpp"
#include "Commit.hpp"
#include "Hash.hpp"
#include "ObjectDatabase.hpp"
#include "ObjectType.hpp"
#include "Reachability.hpp"
#include "Reference.hpp"
#include "Repository.hpp"
#include "Tree.hpp"

#include <algorithm>
#include <sstream>

bool IntegrityReport::repository_consistent() const
{
    return
        corrupted_objects.empty() &&
        missing_objects.empty() &&
        invalid_references.empty();
}

IntegrityChecker::IntegrityChecker(
    const std::filesystem::path& git_directory
)
    : git_directory_(git_directory)
{
}

bool IntegrityChecker::verify_object(
    const std::string& object_id
) const
{
    if (object_id.empty()) {
        return false;
    }

    ObjectDatabase database(
        git_directory_
    );

    if (!database.exists(object_id)) {
        return false;
    }

    const std::string data =
        database.read(object_id);

    return Hash::sha256(data) == object_id;
}

void IntegrityChecker::verify_commit_relationships(
    const std::string& object_id,
    std::vector<std::string>& missing,
    std::vector<std::string>& invalid
) const
{
    ObjectDatabase database(
        git_directory_
    );

    try {
        const Commit commit =
            Commit::deserialize(
                database.read(object_id)
            );

        const std::string tree_id =
            commit.tree_id();

        if (tree_id.empty()) {
            invalid.push_back(object_id);
        }
        else if (!database.exists(tree_id)) {
            missing.push_back(tree_id);
        }
        else if (!verify_object(tree_id)) {
            invalid.push_back(tree_id);
        }
        else {
            const std::string tree_data =
                database.read(tree_id);

            if (
                detect_object_type(tree_data) !=
                ObjectType::Tree
            ) {
                invalid.push_back(tree_id);
            }
        }

        for (const auto& parent :
             commit.parent_ids()) {
            if (parent.empty()) {
                continue;
            }

            if (!database.exists(parent)) {
                missing.push_back(parent);
                continue;
            }

            if (!verify_object(parent)) {
                invalid.push_back(parent);
                continue;
            }

            const std::string parent_data =
                database.read(parent);

            if (
                detect_object_type(parent_data) !=
                ObjectType::Commit
            ) {
                invalid.push_back(parent);
            }
        }
    }
    catch (...) {
        invalid.push_back(object_id);
    }
}

void IntegrityChecker::verify_tree_relationships(
    const std::string& object_id,
    std::vector<std::string>& missing,
    std::vector<std::string>& invalid
) const
{
    ObjectDatabase database(
        git_directory_
    );

    try {
        const Tree tree =
            Tree::deserialize(
                database.read(object_id)
            );

        for (const auto& entry :
             tree.entries()) {

            if (entry.object_id.empty()) {
                invalid.push_back(object_id);
                continue;
            }

            if (!database.exists(entry.object_id)) {
                missing.push_back(
                    entry.object_id
                );
                continue;
            }

            if (!verify_object(entry.object_id)) {
                invalid.push_back(
                    entry.object_id
                );
                continue;
            }

            const std::string child_data =
                database.read(entry.object_id);

            const ObjectType expected_type =
                entry.is_tree
                    ? ObjectType::Tree
                    : ObjectType::Blob;

            if (
                detect_object_type(child_data) !=
                expected_type
            ) {
                invalid.push_back(
                    entry.object_id
                );
            }
        }
    }
    catch (...) {
        invalid.push_back(object_id);
    }
}

IntegrityReport IntegrityChecker::check() const
{
    IntegrityReport report;

    ObjectDatabase database(
        git_directory_
    );

    const auto objects =
        database.object_ids();

    report.total_objects =
        objects.size();

    for (const auto& object_id :
         objects) {

        if (!verify_object(object_id)) {
            report.corrupted_objects.push_back(
                object_id
            );
            continue;
        }

        try {
            const std::string data =
                database.read(object_id);

            const ObjectType type =
                detect_object_type(data);

            switch (type) {
                case ObjectType::Blob:
                    (void)Blob::deserialize(data);
                    break;

                case ObjectType::Tree:
                    (void)Tree::deserialize(data);

                    verify_tree_relationships(
                        object_id,
                        report.missing_objects,
                        report.invalid_references
                    );

                    break;

                case ObjectType::Commit:
                    (void)Commit::deserialize(data);

                    verify_commit_relationships(
                        object_id,
                        report.missing_objects,
                        report.invalid_references
                    );

                    break;
            }

            ++report.valid_objects;
        }
        catch (...) {
            report.corrupted_objects.push_back(
                object_id
            );
        }
    }

    Repository repository(
        git_directory_.parent_path()
    );

    const std::string head =
        repository.head_commit();

    if (!head.empty()) {
        if (!database.exists(head)) {
            report.missing_objects.push_back(head);
        }
        else if (!verify_object(head)) {
            report.invalid_references.push_back(
                head
            );
        }
        else {
            try {
                const ObjectType type =
                    detect_object_type(
                        database.read(head)
                    );

                if (type != ObjectType::Commit) {
                    report.invalid_references.push_back(
                        "HEAD"
                    );
                }
            }
            catch (...) {
                report.invalid_references.push_back(
                    "HEAD"
                );
            }
        }
    }

    for (const auto& branch :
         repository.branches()) {

        Reference reference(
            git_directory_,
            "refs/heads/" + branch
        );

        if (!reference.exists()) {
            continue;
        }

        const std::string target =
            reference.read();

        const std::string reference_name =
            "refs/heads/" + branch;

        if (target.empty()) {
            report.invalid_references.push_back(
                reference_name
            );
            continue;
        }

        if (!database.exists(target)) {
            report.missing_objects.push_back(target);
            continue;
        }

        if (!verify_object(target)) {
            report.invalid_references.push_back(
                reference_name
            );
            continue;
        }

        try {
            if (
                detect_object_type(
                    database.read(target)
                ) != ObjectType::Commit
            ) {
                report.invalid_references.push_back(
                    reference_name
                );
            }
        }
        catch (...) {
            report.invalid_references.push_back(
                reference_name
            );
        }
    }

    for (const auto& tag :
         repository.tags()) {

        Reference reference(
            git_directory_,
            "refs/tags/" + tag
        );

        if (!reference.exists()) {
            continue;
        }

        const std::string target =
            reference.read();

        const std::string reference_name =
            "refs/tags/" + tag;

        if (target.empty()) {
            report.invalid_references.push_back(
                reference_name
            );
            continue;
        }

        if (!database.exists(target)) {
            report.missing_objects.push_back(target);
            continue;
        }

        if (!verify_object(target)) {
            report.invalid_references.push_back(
                reference_name
            );
            continue;
        }

        try {
            if (
                detect_object_type(
                    database.read(target)
                ) != ObjectType::Commit
            ) {
                report.invalid_references.push_back(
                    reference_name
                );
            }
        }
        catch (...) {
            report.invalid_references.push_back(
                reference_name
            );
        }
    }

    auto deduplicate =
        [](std::vector<std::string>& values) {
            std::sort(
                values.begin(),
                values.end()
            );

            values.erase(
                std::unique(
                    values.begin(),
                    values.end()
                ),
                values.end()
            );
        };

    deduplicate(report.corrupted_objects);
    deduplicate(report.missing_objects);
    deduplicate(report.invalid_references);

    Reachability reachability(
        git_directory_
    );

    const auto unreachable =
        reachability.unreachable_objects();

    report.unreachable_objects.assign(
        unreachable.begin(),
        unreachable.end()
    );

    return report;
}

std::string IntegrityChecker::render() const
{
    const IntegrityReport report =
        check();

    std::ostringstream output;

    Repository repository(
        git_directory_.parent_path()
    );

    output
        << "Mini Git Repository Integrity Check\n\n"
        << "References:\n"
        << "  HEAD:     ";

    const std::string head =
        repository.head_commit();

    if (head.empty()) {
        output << "(no commit)\n";
    }
    else {
        output
            << head
            << '\n';
    }

    output
        << "  Branches: "
        << repository.branches().size()
        << '\n'
        << "  Tags:     "
        << repository.tags().size()
        << "\n\n"
        << "Objects:\n"
        << "  Total:       "
        << report.total_objects
        << '\n'
        << "  Valid:       "
        << report.valid_objects
        << '\n'
        << "  Corrupted:   "
        << report.corrupted_objects.size()
        << '\n'
        << "  Missing:     "
        << report.missing_objects.size()
        << '\n'
        << "  Unreachable: "
        << report.unreachable_objects.size()
        << "\n\n"
        << "Invalid references: "
        << report.invalid_references.size()
        << '\n';

    if (!report.corrupted_objects.empty()) {
        output
            << "\nCorrupted objects:\n";

        for (const auto& object_id :
             report.corrupted_objects) {
            output
                << "  "
                << object_id
                << '\n';
        }
    }

    if (!report.missing_objects.empty()) {
        output
            << "\nMissing objects:\n";

        for (const auto& object_id :
             report.missing_objects) {
            output
                << "  "
                << object_id
                << '\n';
        }
    }

    if (!report.invalid_references.empty()) {
        output
            << "\nInvalid references:\n";

        for (const auto& reference :
             report.invalid_references) {
            output
                << "  "
                << reference
                << '\n';
        }
    }

    if (!report.unreachable_objects.empty()) {
        output
            << "\nUnreachable objects:\n";

        for (const auto& object_id :
             report.unreachable_objects) {
            output
                << "  "
                << object_id
                << '\n';
        }
    }

    output << '\n';

    if (report.repository_consistent()) {
        output
            << "Repository is structurally consistent.\n";
    }
    else {
        output
            << "Repository has integrity problems.\n";
    }

    return output.str();
}
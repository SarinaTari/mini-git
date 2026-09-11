#pragma once

#include "Object.hpp"

#include <string>
#include <vector>

class Commit : public Object {
public:
    Commit(
        std::string tree_id,
        std::string parent_id,
        std::string author,
        std::string message
    );

    Commit(
        std::string tree_id,
        std::vector<std::string> parent_ids,
        std::string author,
        std::string message
    );

    std::string serialize() const override;

    static Commit deserialize(
        const std::string& data
    );

    const std::string& tree_id() const;

    const std::string& parent_id() const;

    const std::vector<std::string>& parent_ids() const;

    const std::string& author() const;

    const std::string& message() const;

private:
    std::string tree_id_;
    std::vector<std::string> parent_ids_;
    std::string author_;
    std::string message_;
};
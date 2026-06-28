//
// Created by Abhinav Jha on 20/06/26.
//

#ifndef WISP_ENVIRONMENT_H
#define WISP_ENVIRONMENT_H

#include "WispValue.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace Wisp {

enum class AssignResult {
    Ok,
    NotDefined,
    Immutable,
};

class Environment {
public:
    explicit Environment(std::shared_ptr<Environment> parent = nullptr);

    bool define(const std::string& name, std::unique_ptr<WispValue> value, bool is_mutable);
    const WispValue* get(const std::string& name) const;
    AssignResult assign(const std::string& name, std::unique_ptr<WispValue> value);
    std::shared_ptr<Environment> take_parent();

private:
    struct Entry {
        std::unique_ptr<WispValue> value;
        bool is_mutable;
    };

    std::unordered_map<std::string, Entry> m_vars;
    std::shared_ptr<Environment> m_parent;
};

}   // namespace Wisp

#endif //WISP_ENVIRONMENT_H

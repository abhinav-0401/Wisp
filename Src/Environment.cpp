//
// Created by Abhinav Jha on 20/06/26.
//

#include "Environment.h"

#include <format>

namespace Wisp {

Environment::Environment(std::shared_ptr<Environment> parent)
    : m_parent(parent) {}

bool Environment::define(const std::string& name, std::unique_ptr<WispValue> value, bool is_mutable) {
    if (m_vars.contains(name)) {
        return false;
    }
    m_vars.emplace(name, Entry{ std::move(value), is_mutable });
    return true;
}

const WispValue* Environment::get(const std::string& name) const {
    auto it = m_vars.find(name);
    if (it != m_vars.end()) {
        return it->second.value.get();
    }
    if (m_parent) {
        return m_parent->get(name);
    }
    return nullptr;
}

AssignResult Environment::assign(const std::string& name, std::unique_ptr<WispValue> value) {
    auto it = m_vars.find(name);
    if (it != m_vars.end()) {
        if (!it->second.is_mutable) {
            return AssignResult::Immutable;   // leave the binding untouched
        }
        it->second.value = std::move(value);
        return AssignResult::Ok;
    }
    if (m_parent) {
        return m_parent->assign(name, std::move(value));
    }
    return AssignResult::NotDefined;
}

}   // namespace Wisp

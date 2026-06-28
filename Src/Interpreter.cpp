//
// Created by Abhinav Jha on 07/06/26.
//

#include "Interpreter.h"
#include "NodeVisitor.h"
#include "Token.h"

#include <format>
#include <iostream>

namespace Wisp {

/*   HERE BE STATEMENTS   */

Interpreter::Interpreter() {
    m_current_env = std::make_unique<Environment>();
}

void Interpreter::visit_expr_stmt(const ExprStmt* stmt) {
    stmt->expr().accept(*this);
}

void Interpreter::visit_print_stmt(const PrintStmt* stmt) {
    stmt->expr().accept(*this);
    if (check_err_val()) {
        return;
    }
    std::cout << m_view_value->to_string() << std::endl;
}

void Interpreter::visit_var_decl_stmt(const VarDeclStmt* stmt) {
    stmt->init().accept(*this);
    if (check_err_val()) {
        return;
    }

    // we now have two cases, the value could be owned, or it could be non - owned (owned by the env)
    // for e.g.: (var x = 5) vs (var x = y)
    // if we clone, there are now two unique_ptr, each a copy of the other (kind neat: copy semantics auto implemented)
    // if we don't need to clone, the evaluated expr's owned WispValue can just be moved (nice, no copy for "r-values")

    if (!m_owned_value) {
        m_current_env->define(stmt->name(), std::move(m_owned_value->clone()), stmt->is_mutable());
        return;
    }

    m_current_env->define(stmt->name(), std::move(m_owned_value), stmt->is_mutable());
}


/*   HERE BE EXPRs   */

void Interpreter::visit_var_expr(const VarExpr* expr) {
    set_view_value(m_current_env->get(expr->name()));
}

void Interpreter::visit_assign_expr(const AssignExpr* expr) {
    expr->value().accept(*this);
    if (check_err_val()) {
        return;
    }

    if (!m_owned_value) {
        m_current_env->assign(expr->name(), std::move(m_owned_value->clone()));
        return;
    }

    auto result = m_current_env->assign(expr->name(), std::move(m_owned_value));
    switch (result) {
        case AssignResult::Immutable:
            set_owned_value(std::make_unique<ErrorValue>(
                std::format("Cannot assign to a constant variable '{}'.", expr->name()),
                expr->line()));
            break;
        case AssignResult::NotDefined:
            set_owned_value(std::make_unique<ErrorValue>(
                std::format("Undefined variable '{}'.", expr->name()),
                expr->line()));
            break;
        case AssignResult::Ok:
            set_view_value(m_current_env->get(expr->name()));
            break;
        default:
            set_owned_value(std::make_unique<ErrorValue>(
                std::format("Unknown error while assigning to '{}'.", expr->name()),
                expr->line()));
            break;
    }
}

void Interpreter::visit_unary_expr(const UnaryExpr* expr) {
    expr->expr().accept(*this);
    if (check_err_val()) {
        return;
    }

    switch (expr->op()) {
        case TokenKind::Bang: {
            if (m_view_value->kind() != WispValueKind::Bool) {
                set_owned_value(std::make_unique<ErrorValue>("Cannot use '!' on a non-boolean.", expr->line()));
                return;
            }
            auto val = static_cast<const BoolValue*>(m_view_value);
            set_owned_value(std::make_unique<BoolValue>(!val->value()));
            break;
        }
        case TokenKind::Minus: {
            if (m_view_value->kind() != WispValueKind::Int) {
                set_owned_value(std::make_unique<ErrorValue>("Cannot use '-' on a non-number.", expr->line()));
                return;
            }
            auto val = static_cast<const IntValue*>(m_view_value);
            set_owned_value(std::make_unique<IntValue>(-val->value()));
            break;
        }
        default: {
             set_owned_value(std::make_unique<ErrorValue>(
                std::format("Invalid operator: {}", token_kind_to_string(expr->op())),
                expr->line()));
            break;
        }
    }
}

void Interpreter::visit_binary_expr(const BinaryExpr* expr) {
    expr->left().accept(*this);
    if (check_err_val()) {
        return;
    }
    auto l_result = m_view_value;

    expr->right().accept(*this);
    if (check_err_val()) {
        return;
    }

    switch (expr->op()) {
        case TokenKind::Plus:
        case TokenKind::Minus:
        case TokenKind::Star:
        case TokenKind::Slash:
            set_owned_value(eval_arithmetic(expr->op(), l_result, m_view_value, expr->line()));
            break;
        case TokenKind::GreaterThan:
        case TokenKind::LessThan:
        case TokenKind::GreaterThanEquals:
        case TokenKind::LessThanEquals:
            set_owned_value(eval_comparison(expr->op(), l_result, m_view_value, expr->line()));
            break;
        case TokenKind::EqualsEquals:
        case TokenKind::NotEquals:
            set_owned_value(eval_equality(expr->op(), l_result, m_view_value, expr->line()));
            break;
        default: {
            set_owned_value(std::make_unique<ErrorValue>(
                std::format("Invalid operator: {}", token_kind_to_string(expr->op())),
                expr->line()));
            break;
        }
    }
}

std::unique_ptr<WispValue> Interpreter::eval_arithmetic(TokenKind op, const WispValue* l, const WispValue* r, std::size_t line) const {
    if (l->kind() == WispValueKind::Int && r->kind() == WispValueKind::Int) {
        auto lv = static_cast<const IntValue*>(l)->value();
        auto rv = static_cast<const IntValue*>(r)->value();
        switch (op) {
            case TokenKind::Plus:  return std::make_unique<IntValue>(lv + rv);
            case TokenKind::Minus: return std::make_unique<IntValue>(lv - rv);
            case TokenKind::Star:  return std::make_unique<IntValue>(lv * rv);
            case TokenKind::Slash: {
                if (rv == 0) return std::make_unique<ErrorValue>("Division by zero.", line);
                return std::make_unique<IntValue>(lv / rv);
            }
            default: break;
        }
    } else if (l->kind() == WispValueKind::Float && r->kind() == WispValueKind::Float) {
        auto lv = static_cast<const FloatValue*>(l)->value();
        auto rv = static_cast<const FloatValue*>(r)->value();
        switch (op) {
            case TokenKind::Plus:  return std::make_unique<FloatValue>(lv + rv);
            case TokenKind::Minus: return std::make_unique<FloatValue>(lv - rv);
            case TokenKind::Star:  return std::make_unique<FloatValue>(lv * rv);
            case TokenKind::Slash: return std::make_unique<FloatValue>(lv / rv);
            default: break;
        }
    }
    return std::make_unique<ErrorValue>(
        std::format("Operands must be both Int or both Float for '{}'.", token_kind_to_string(op)),
        line);
}

std::unique_ptr<WispValue> Interpreter::eval_comparison(TokenKind op, const WispValue* l, const WispValue* r, std::size_t line) const {
    if (l->kind() == WispValueKind::Int && r->kind() == WispValueKind::Int) {
        auto lv = static_cast<const IntValue*>(l)->value();
        auto rv = static_cast<const IntValue*>(r)->value();
        switch (op) {
            case TokenKind::GreaterThan:       return std::make_unique<BoolValue>(lv > rv);
            case TokenKind::LessThan:          return std::make_unique<BoolValue>(lv < rv);
            case TokenKind::GreaterThanEquals: return std::make_unique<BoolValue>(lv >= rv);
            case TokenKind::LessThanEquals:    return std::make_unique<BoolValue>(lv <= rv);
            default: break;
        }
    } else if (l->kind() == WispValueKind::Float && r->kind() == WispValueKind::Float) {
        auto lv = static_cast<const FloatValue*>(l)->value();
        auto rv = static_cast<const FloatValue*>(r)->value();
        switch (op) {
            case TokenKind::GreaterThan:       return std::make_unique<BoolValue>(lv > rv);
            case TokenKind::LessThan:          return std::make_unique<BoolValue>(lv < rv);
            case TokenKind::GreaterThanEquals: return std::make_unique<BoolValue>(lv >= rv);
            case TokenKind::LessThanEquals:    return std::make_unique<BoolValue>(lv <= rv);
            default: break;
        }
    }
    return std::make_unique<ErrorValue>(
        std::format("Operands must be both Int or both Float for '{}'.", token_kind_to_string(op)),
        line);
}

std::unique_ptr<WispValue> Interpreter::eval_equality(TokenKind op, const WispValue* l, const WispValue* r, std::size_t line) const {
    if (l->kind() != r->kind()) {
        return std::make_unique<ErrorValue>(
            std::format("Operands must be the same type for '{}'.", token_kind_to_string(op)),
            line);
    }
    bool result;
    switch (l->kind()) {
        case WispValueKind::Int: {
            auto lv = static_cast<const IntValue*>(l)->value();
            auto rv = static_cast<const IntValue*>(r)->value();
            result = (op == TokenKind::EqualsEquals) ? lv == rv : lv != rv;
            break;
        }
        case WispValueKind::Float: {
            auto lv = static_cast<const FloatValue*>(l)->value();
            auto rv = static_cast<const FloatValue*>(r)->value();
            result = (op == TokenKind::EqualsEquals) ? lv == rv : lv != rv;
            break;
        }
        case WispValueKind::Bool: {
            auto lv = static_cast<const BoolValue*>(l)->value();
            auto rv = static_cast<const BoolValue*>(r)->value();
            result = (op == TokenKind::EqualsEquals) ? lv == rv : lv != rv;
            break;
        }
        case WispValueKind::String: {
            const auto& lv = static_cast<const StringValue*>(l)->value();
            const auto& rv = static_cast<const StringValue*>(r)->value();
            result = (op == TokenKind::EqualsEquals) ? lv == rv : lv != rv;
            break;
        }
        default:
            return std::make_unique<ErrorValue>(
                std::format("Cannot compare values of this type with '{}'.", token_kind_to_string(op)),
                line);
    }
    return std::make_unique<BoolValue>(result);
}

void Interpreter::visit_literal_expr(const LiteralExpr<std::int32_t>* expr) {
    set_owned_value(std::make_unique<IntValue>(expr->value()));
}

void Interpreter::visit_literal_expr(const LiteralExpr<std::string>* expr) {
    set_owned_value(std::make_unique<StringValue>(expr->value()));
}

void Interpreter::visit_literal_expr(const LiteralExpr<float>* expr) {
    set_owned_value(std::make_unique<FloatValue>(expr->value()));
}

void Interpreter::visit_literal_expr(const LiteralExpr<bool>* expr) {
    set_owned_value(std::make_unique<BoolValue>(expr->value()));
}

bool Interpreter::check_err_val() const {
    return m_view_value && m_view_value->kind() == WispValueKind::Error;
}

}   // namespace Wisp

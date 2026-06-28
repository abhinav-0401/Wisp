//
// Created by Abhinav Jha on 23/05/26.
//

#include "ASTPrinter.h"

#include <format>

namespace Wisp {

void ASTPrinter::visit_expr_stmt(const ExprStmt* stmt) {
    stmt->expr().accept(*this);
    m_output += "ExprStmt{ ";
    m_output += m_latest_output;
    m_output += " }\n";
}

void ASTPrinter::visit_print_stmt(const PrintStmt* stmt) {
    m_output += "PrintStmt{ ";
    stmt->expr().accept(*this);
    m_output += " }\n";
}

void ASTPrinter::visit_var_decl_stmt(const VarDeclStmt* stmt) {
    m_output += "VarDeclStmt{ ";
    stmt->init().accept(*this);
    m_output += " }\n";
}

void ASTPrinter::visit_unary_expr(const UnaryExpr* expr) {
    m_output += " UnaryExpr{ ";
    expr->expr().accept(*this);
    m_output += " , ";
    m_output += token_kind_to_string(expr->op());
    m_output += " }";
}

void ASTPrinter::visit_binary_expr(const BinaryExpr* expr) {
    m_output += " BinaryExpr{ ";
    expr->left().accept(*this);
    m_output += " , ";
    m_output += token_kind_to_string(expr->op());
    m_output += " , ";
    expr->right().accept(*this);
    m_output += " }";
}

void ASTPrinter::visit_var_expr(const VarExpr* expr) {
    m_output += std::format("VarExpr{{ {} }}", expr->name());;
}

void ASTPrinter::visit_assign_expr(const AssignExpr* expr) {
    m_output += " AssignExpr{ ";
    m_output += expr->name() + " = ";
    expr->value().accept(*this);
    m_output += " }";
}

void ASTPrinter::visit_literal_expr(const LiteralExpr<std::int32_t>* expr) {
    m_latest_output = std::to_string(expr->value());
    m_output += m_latest_output;
}

void ASTPrinter::visit_literal_expr(const LiteralExpr<std::string>* expr) {
    m_latest_output = expr->value();
    m_output += m_latest_output;
}

void ASTPrinter::visit_literal_expr(const LiteralExpr<float>* expr) {
    m_latest_output = std::to_string(expr->value());
    m_output += m_latest_output;
}

void ASTPrinter::visit_literal_expr(const LiteralExpr<bool>* expr) {
    m_latest_output = std::to_string(expr->value());
    m_output += m_latest_output;
}

const std::string& ASTPrinter::latest_output() const {
    return m_latest_output;
}

const std::string& ASTPrinter::output() const {
    return m_output;
}

void ASTPrinter::reset() {
    m_output.clear();
    m_latest_output.clear();
}

}   // namespace Wisp

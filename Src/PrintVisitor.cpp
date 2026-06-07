//
// Created by Abhinav Jha on 23/05/26.
//

#include "PrintVisitor.h"

namespace Wisp {

void PrintVisitor::visit_expr_stmt(const ExprStmt* stmt, NodeVisitor& visitor) {
    stmt->expr().accept(visitor);
}

void PrintVisitor::visit_unary_expr(const UnaryExpr* expr, NodeVisitor& vistor) {
    m_output += " BinaryExpr{ ";
    expr->expr().accept(vistor);
    m_output += " , ";
    m_output += token_kind_to_string(expr->op());
    m_output += " }";
}

void PrintVisitor::visit_binary_expr(const BinaryExpr* expr, NodeVisitor& vistor) {
    m_output += " BinaryExpr{ ";
    expr->left().accept(vistor);
    m_output += " , ";
    m_output += token_kind_to_string(expr->op());
    m_output += " , ";
    expr->right().accept(vistor);
    m_output += " }";
}

void PrintVisitor::visit_literal_expr(const LiteralExpr<std::int32_t>* expr) {
    m_latest_output = std::to_string(expr->value());
    m_output += m_latest_output;
}

void PrintVisitor::visit_literal_expr(const LiteralExpr<std::string>* expr) {
    // this will create a copy of the string in the AST
    // I don't think I would need something like storing a string_view to this, since I want the PrintVisitor to own
    // its output.
    m_latest_output = expr->value();
    m_output += m_latest_output;
}

void PrintVisitor::visit_literal_expr(const LiteralExpr<float>* expr) {
    m_latest_output = std::to_string(expr->value());
    m_output += m_latest_output;
}

const std::string& PrintVisitor::latest_output() const {
    return m_latest_output;
}

const std::string& PrintVisitor::output() const {
    return m_output;
}

void PrintVisitor::reset() {
    m_output.clear();
    m_latest_output.clear();
}

}   // namespace Wisp
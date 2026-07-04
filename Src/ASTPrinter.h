//
// Created by Abhinav Jha on 23/05/26.
//

#ifndef WISP_PRINTVISITOR_H
#define WISP_PRINTVISITOR_H

#include "AST.h"
#include "NodeVisitor.h"

namespace Wisp {

class ASTPrinter final : public NodeVisitor {
public:
    void visit_literal_expr(const LiteralExpr<std::int32_t>* expr) override;
    void visit_literal_expr(const LiteralExpr<std::string>* expr) override;
    void visit_literal_expr(const LiteralExpr<float>* expr) override;
    void visit_literal_expr(const LiteralExpr<bool>* expr) override;

    void visit_unary_expr(const UnaryExpr* expr) override;
    void visit_binary_expr(const BinaryExpr* expr) override;
    void visit_var_expr(const VarExpr* expr) override;
    void visit_assign_expr(const AssignExpr* expr) override;

    void visit_expr_stmt(const ExprStmt* stmt) override;
    void visit_print_stmt(const PrintStmt* stmt) override;
    void visit_var_decl_stmt(const VarDeclStmt* stmt) override;
    void visit_block_stmt(const BlockStmt* stmt) override;

    const std::string& latest_output() const;
    const std::string& output() const;
    void reset();

private:
    std::string m_output;
    std::string m_latest_output;
};

}   // namespace Wisp

#endif //WISP_PRINTVISITOR_H

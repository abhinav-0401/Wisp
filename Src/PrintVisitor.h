//
// Created by Abhinav Jha on 23/05/26.
//

#ifndef WISP_PRINTVISITOR_H
#define WISP_PRINTVISITOR_H

#include "AST.h"
#include "NodeVisitor.h"

namespace Wisp {

class PrintVisitor final : public NodeVisitor {
public:
    void visit_literal_expr(const LiteralExpr<std::int32_t>* expr) override;
    void visit_literal_expr(const LiteralExpr<std::string>* expr) override;
    void visit_literal_expr(const LiteralExpr<float>* expr) override;

    void visit_unary_expr(const UnaryExpr* expr, NodeVisitor& visitor) override;
    void visit_binary_expr(const BinaryExpr* expr, NodeVisitor& visitor) override;
    void visit_expr_stmt(const ExprStmt* stmt, NodeVisitor& visitor) override;

    const std::string& latest_output() const;
    const std::string& output() const;
    void reset();

private:
    std::string m_output;
    std::string m_latest_output;
};

}   // namespace Wisp

#endif //WISP_PRINTVISITOR_H
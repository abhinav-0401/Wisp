//
// Created by Abhinav Jha on 23/05/26.
//

#ifndef WISP_NODEVISITOR_H
#define WISP_NODEVISITOR_H

#include "WispConcepts.h"

namespace Wisp {

template <AllowedLiteral T>
class LiteralExpr;

class UnaryExpr;
class BinaryExpr;
class VarExpr;
class AssignExpr;

class ExprStmt;
class PrintStmt;
class VarDeclStmt;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit_literal_expr(const LiteralExpr<std::int32_t>* expr) = 0;
    virtual void visit_literal_expr(const LiteralExpr<std::string>* expr) = 0;
    virtual void visit_literal_expr(const LiteralExpr<float>* expr) = 0;
    virtual void visit_literal_expr(const LiteralExpr<bool>* expr) = 0;

    virtual void visit_unary_expr(const UnaryExpr* expr) = 0;
    virtual void visit_binary_expr(const BinaryExpr* expr) = 0;
    virtual void visit_var_expr(const VarExpr* expr) = 0;
    virtual void visit_assign_expr(const AssignExpr* expr) = 0;

    virtual void visit_expr_stmt(const ExprStmt* stmt) = 0;
    virtual void visit_print_stmt(const PrintStmt* stmt) = 0;
    virtual void visit_var_decl_stmt(const VarDeclStmt* stmt) = 0;
};

}   // namespace Wisp

#endif //WISP_NODEVISITOR_H

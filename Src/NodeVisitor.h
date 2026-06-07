//
// Created by Abhinav Jha on 23/05/26.
//

#ifndef WISP_NODEVISITOR_H
#define WISP_NODEVISITOR_H

#include <cstdint>
#include <string>

namespace Wisp {

template <typename T>
concept AllowedLiteral = std::same_as<T, std::int32_t>
                        || std::same_as<T, float>
                        || std::same_as<T, std::string>;

template <AllowedLiteral T>
class LiteralExpr;

class UnaryExpr;
class BinaryExpr;
class ExprStmt;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit_literal_expr(const LiteralExpr<std::int32_t>* expr) = 0;
    virtual void visit_literal_expr(const LiteralExpr<std::string>* expr) = 0;
    virtual void visit_literal_expr(const LiteralExpr<float>* expr) = 0;

    virtual void visit_unary_expr(const UnaryExpr* expr, NodeVisitor& visitor) = 0;
    virtual void visit_binary_expr(const BinaryExpr* expr, NodeVisitor& visitor) = 0;
    virtual void visit_expr_stmt(const ExprStmt* stmt, NodeVisitor& visitor) = 0;
};

}   // namespace Wisp

#endif //WISP_NODEVISITOR_H
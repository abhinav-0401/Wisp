//
// Created by Abhinav Jha on 10/05/26.
//

#ifndef WISP_AST_H
#define WISP_AST_H

#include "Token.h"

#include <memory>
#include <vector>

namespace Wisp {

enum class ASTNodeKind {
    Program,
    UnaryExpr,
    BinaryExpr,
    IntLiteral,
    FloatLiteral,
    StringLiteral,

    ExprStmt,
};

class ASTNode {
public:
    virtual ASTNodeKind kind() const = 0;
    virtual ~ASTNode() = default;
};

class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() = default;
};

class ExprNode : public ASTNode {
public:
    virtual ~ExprNode() = default;
};

class Program : public ASTNode {
public:
    Program() = default;
    ASTNodeKind kind() const override { return ASTNodeKind::Program; }
    void add_stmt(std::unique_ptr<StmtNode> stmt) { m_program.push_back(std::move(stmt)); }

private:
    std::vector<std::unique_ptr<ASTNode>> m_program;
};

class UnaryExpr : public ExprNode {
public:
    UnaryExpr(TokenKind op, std::unique_ptr<ExprNode> expr)
        : m_op(op)
        , m_expr(std::move(expr)) {}

    ASTNodeKind kind() const override { return ASTNodeKind::UnaryExpr; }

private:
    TokenKind m_op;
    std::unique_ptr<ExprNode> m_expr;
};

class BinaryExpr : public ExprNode {
public:
    BinaryExpr(TokenKind op, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        : m_op(op)
        , m_left(std::move(left))
        , m_right(std::move(right)) {}

    ASTNodeKind kind() const override { return ASTNodeKind::BinaryExpr; }

private:
    TokenKind m_op;
    std::unique_ptr<ExprNode> m_left;
    std::unique_ptr<ExprNode> m_right;
};

template <typename T>
concept AllowedLiteral = std::same_as<T, std::int32_t>
                        || std::same_as<T, float>
                        || std::same_as<T, std::string>;

template <AllowedLiteral T>
class LiteralExpr : public ExprNode {
public:
    explicit LiteralExpr(T value)
        : m_value(value) {}

    ASTNodeKind kind() const override {
        if constexpr (std::same_as<T, std::int32_t>) {
            return ASTNodeKind::IntLiteral;
        } else if constexpr (std::same_as<T, float>) {
            return ASTNodeKind::FloatLiteral;
        } else if constexpr (std::same_as<T, std::string>) {
            return ASTNodeKind::StringLiteral;
        }
    }

private:
    T m_value;
};

class ExprStmt : public StmtNode {
public:
    explicit ExprStmt(std::unique_ptr<ExprNode> expr)
        : m_expr(std::move(expr)) {}

    ASTNodeKind kind() const override { return ASTNodeKind::ExprStmt; }

private:
    std::unique_ptr<ExprNode> m_expr;
};

}   // namespace Wisp

#endif //WISP_AST_H
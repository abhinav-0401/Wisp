//
// Created by Abhinav Jha on 10/05/26.
//

#ifndef WISP_AST_H
#define WISP_AST_H

#include "NodeVisitor.h"
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
    virtual void accept(NodeVisitor&) const = 0;
};

class StmtNode : public ASTNode {
public:
    ~StmtNode() override = default;
};

class ExprNode : public ASTNode {
public:
    ~ExprNode() override = default;
};

class Program : public ASTNode {
public:
    Program() = default;

    ASTNodeKind kind() const override { return ASTNodeKind::Program; }

    void add_stmt(std::unique_ptr<StmtNode> stmt) { m_program.push_back(std::move(stmt)); }

    void accept(NodeVisitor& visitor) const override {
        for (auto& stmt : m_program) {
            if (stmt) {
                stmt->accept(visitor);
            }
        }
    }

private:
    std::vector<std::unique_ptr<StmtNode>> m_program;
};

class UnaryExpr : public ExprNode {
public:
    UnaryExpr(TokenKind op, std::unique_ptr<ExprNode> expr)
        : m_op(op)
        , m_expr(std::move(expr)) {}

    ExprNode& expr() const { return *m_expr; }
    TokenKind op() const { return m_op; }

    ASTNodeKind kind() const override { return ASTNodeKind::UnaryExpr; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_unary_expr(this, visitor); }

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

    ExprNode& left() const { return *m_left; }
    ExprNode& right() const { return *m_right; }

    TokenKind op() const { return m_op; }

    ASTNodeKind kind() const override { return ASTNodeKind::BinaryExpr; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_binary_expr(this, visitor); }

private:
    TokenKind m_op;
    std::unique_ptr<ExprNode> m_left;
    std::unique_ptr<ExprNode> m_right;
};


template <AllowedLiteral T>
class LiteralExpr : public ExprNode {
public:
    explicit LiteralExpr(T value)
        : m_value(value) {}

    T value() const { return m_value; }

    ASTNodeKind kind() const override {
        if constexpr (std::same_as<T, std::int32_t>) {
            return ASTNodeKind::IntLiteral;
        } else if constexpr (std::same_as<T, float>) {
            return ASTNodeKind::FloatLiteral;
        } else if constexpr (std::same_as<T, std::string>) {
            return ASTNodeKind::StringLiteral;
        }
    }

    void accept(NodeVisitor& visitor) const override { visitor.visit_literal_expr(this); }

private:
    T m_value;
};

class ExprStmt : public StmtNode {
public:
    explicit ExprStmt(std::unique_ptr<ExprNode> expr)
        : m_expr(std::move(expr)) {}

    ExprNode& expr() const { return *m_expr; }
    ASTNodeKind kind() const override { return ASTNodeKind::ExprStmt; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_expr_stmt(this, visitor); }

private:
    std::unique_ptr<ExprNode> m_expr;
};

}   // namespace Wisp

#endif //WISP_AST_H
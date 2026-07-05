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
    VarExpr,
    AssignExpr,
    CallExpr,

    IntLiteral,
    FloatLiteral,
    StringLiteral,
    BoolLiteral,

    ExprStmt,
    PrintStmt,
    VarDeclStmt,
    BlockStmt,
    IfStmt,
    ForStmt,
    FnDeclStmt,
};

class ASTNode {
public:
    explicit ASTNode(std::size_t line) : m_line(line) {}
    virtual ASTNodeKind kind() const = 0;
    virtual ~ASTNode() = default;
    virtual void accept(NodeVisitor&) const = 0;
    std::size_t line() const { return m_line; }

private:
    std::size_t m_line;
};

class StmtNode : public ASTNode {
public:
    explicit StmtNode(std::size_t line) : ASTNode(line) {}
    ~StmtNode() override = default;
};

class ExprNode : public ASTNode {
public:
    explicit ExprNode(std::size_t line) : ASTNode(line) {}
    ~ExprNode() override = default;
};

class Program : public ASTNode {
public:
    Program() : ASTNode(1) {}

    ASTNodeKind kind() const override { return ASTNodeKind::Program; }

    void add_stmt(std::unique_ptr<StmtNode> stmt) { m_program.push_back(std::move(stmt)); }

    void accept(NodeVisitor& visitor) const override {
        for (auto& stmt : m_program) {
            stmt->accept(visitor);
        }
    }

private:
    std::vector<std::unique_ptr<StmtNode>> m_program;
};

class UnaryExpr : public ExprNode {
public:
    UnaryExpr(TokenKind op, std::unique_ptr<ExprNode> expr, std::size_t line)
        : ExprNode(line)
        , m_op(op)
        , m_expr(std::move(expr)) {}

    ExprNode& expr() const { return *m_expr; }
    TokenKind op() const { return m_op; }

    ASTNodeKind kind() const override { return ASTNodeKind::UnaryExpr; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_unary_expr(this); }

private:
    TokenKind m_op;
    std::unique_ptr<ExprNode> m_expr;
};

class BinaryExpr : public ExprNode {
public:
    BinaryExpr(TokenKind op, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right, std::size_t line)
        : ExprNode(line)
        , m_op(op)
        , m_left(std::move(left))
        , m_right(std::move(right)) {}

    ExprNode& left() const { return *m_left; }
    ExprNode& right() const { return *m_right; }

    TokenKind op() const { return m_op; }

    ASTNodeKind kind() const override { return ASTNodeKind::BinaryExpr; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_binary_expr(this); }

private:
    TokenKind m_op;
    std::unique_ptr<ExprNode> m_left;
    std::unique_ptr<ExprNode> m_right;
};

class VarExpr : public ExprNode {
public:
    VarExpr(std::string name, std::size_t line)
        : ExprNode(line)
        , m_name(std::move(name)) {}

    ASTNodeKind kind() const override { return ASTNodeKind::VarExpr; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_var_expr(this); }
    const std::string& name() const { return m_name; }

private:
    std::string m_name;
};

class AssignExpr : public ExprNode {
public:
    AssignExpr(std::string name, std::unique_ptr<ExprNode> value, std::size_t line)
        : ExprNode(line)
        , m_name(std::move(name))
        , m_value(std::move(value)) {}

    ASTNodeKind kind() const override { return ASTNodeKind::AssignExpr; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_assign_expr(this); }
    const std::string& name() const { return m_name; }
    ExprNode& value() const { return *m_value; }

private:
    std::string m_name;
    std::unique_ptr<ExprNode> m_value;
};

class CallExpr : public ExprNode {

};

template <AllowedLiteral T>
class LiteralExpr : public ExprNode {
public:
    LiteralExpr(T value, std::size_t line)
        : ExprNode(line)
        , m_value(value) {}

    const T& value() const { return m_value; }

    ASTNodeKind kind() const override {
        if constexpr (std::same_as<T, std::int32_t>) {
            return ASTNodeKind::IntLiteral;
        } else if constexpr (std::same_as<T, float>) {
            return ASTNodeKind::FloatLiteral;
        } else if constexpr (std::same_as<T, std::string>) {
            return ASTNodeKind::StringLiteral;
        } else if constexpr (std::same_as<T, bool>) {
            return ASTNodeKind::BoolLiteral;
        }
    }

    void accept(NodeVisitor& visitor) const override { visitor.visit_literal_expr(this); }

private:
    T m_value;
};

class ExprStmt : public StmtNode {
public:
    explicit ExprStmt(std::unique_ptr<ExprNode> expr)
        : StmtNode(expr->line())
        , m_expr(std::move(expr)) {}

    ExprNode& expr() const { return *m_expr; }
    ASTNodeKind kind() const override { return ASTNodeKind::ExprStmt; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_expr_stmt(this); }

private:
    std::unique_ptr<ExprNode> m_expr;
};

class PrintStmt : public StmtNode {
public:
    explicit PrintStmt(std::unique_ptr<ExprNode> expr, std::size_t line)
        : StmtNode(line)
        , m_expr(std::move(expr)) {}

    ExprNode& expr() const { return *m_expr; }
    ASTNodeKind kind() const override { return ASTNodeKind::PrintStmt; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_print_stmt(this); }

private:
    std::unique_ptr<ExprNode> m_expr;
};

class VarDeclStmt : public StmtNode {
public:
    VarDeclStmt(std::string name, std::unique_ptr<ExprNode> init, bool is_mutable, std::size_t line)
        : StmtNode(line)
        , m_name(name)
        , m_init(std::move(init))
        , m_is_mutable(is_mutable) {}

    ExprNode& init() const { return *m_init; }
    bool is_mutable() const { return m_is_mutable; }
    const std::string& name() const { return m_name; }
    ASTNodeKind kind() const override { return ASTNodeKind::VarDeclStmt; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_var_decl_stmt(this); }
private:
    std::string m_name;
    std::unique_ptr<ExprNode> m_init;
    bool m_is_mutable;
};

class BlockStmt : public StmtNode {
public:
    BlockStmt(std::size_t line, std::vector<std::unique_ptr<StmtNode>> stmts)
        : StmtNode(line)
        , m_stmts(std::move(stmts)) {}

    void add_stmt(std::unique_ptr<StmtNode> stmt) { m_stmts.push_back(std::move(stmt)); }
    const std::vector<std::unique_ptr<StmtNode>>& stmts() const { return m_stmts; }
    ASTNodeKind kind() const override { return ASTNodeKind::BlockStmt; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_block_stmt(this); }

private:
    std::vector<std::unique_ptr<StmtNode>> m_stmts;
};

class IfStmt : public StmtNode {
public:
    IfStmt(std::size_t line,
        std::unique_ptr<ExprNode> cond,
        std::unique_ptr<BlockStmt> then_block,
        std::unique_ptr<StmtNode> else_block)
            : StmtNode(line)
            , m_cond(std::move(cond))
            , m_then_block(std::move(then_block))
            , m_else_block(std::move(else_block)) {}

    const BlockStmt& then_branch() const { return *m_then_block; }
    const StmtNode* else_branch() const { return m_else_block.get(); }
    const ExprNode& cond() const { return *m_cond; }
    ASTNodeKind kind() const override { return ASTNodeKind::IfStmt; }
    void accept(NodeVisitor& visitor) const override { visitor.visit_if_stmt(this); }

private:
    std::unique_ptr<ExprNode> m_cond;
    std::unique_ptr<BlockStmt> m_then_block;
    // since an else stmt may be followed by either an if stmt again or simply a block stmt
    std::unique_ptr<StmtNode> m_else_block;
};

class ForStmt : public StmtNode {

};

class FnDeclStmt : public StmtNode {

};

}   // namespace Wisp

#endif //WISP_AST_H

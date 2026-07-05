//
// Created by Abhinav Jha on 07/06/26.
//

#ifndef WISP_INTERPRETER_H
#define WISP_INTERPRETER_H

#include "AST.h"
#include "CompilationUnit.h"
#include "Environment.h"
#include "NodeVisitor.h"
#include "WispValue.h"

#include <memory>

namespace Wisp {

class Interpreter final : public NodeVisitor {
public:
    explicit Interpreter(CompilationUnit& comp_unit);

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
    void visit_if_stmt(const IfStmt* stmt) override;

private:
    bool check_err_val() const;
    void report_runtime_error();
    std::unique_ptr<WispValue> eval_arithmetic(TokenKind op, const WispValue* l, const WispValue* r, std::size_t line) const;
    std::unique_ptr<WispValue> eval_comparison(TokenKind op, const WispValue* l, const WispValue* r, std::size_t line) const;
    std::unique_ptr<WispValue> eval_equality(TokenKind op, const WispValue* l, const WispValue* r, std::size_t line) const;

    void set_owned_value(std::unique_ptr<WispValue> value) {
        m_owned_value = std::move(value);
        m_view_value = m_owned_value.get();
    }

    void set_view_value(const WispValue* value) {
        m_view_value = value;
        m_owned_value = nullptr;
    }

private:
    std::unique_ptr<WispValue> m_owned_value = nullptr;
    const WispValue* m_view_value = nullptr;
    std::shared_ptr<Environment> m_current_env = nullptr;
    CompilationUnit& m_comp_unit;
};

}   // namespace Wisp

#endif //WISP_INTERPRETER_H

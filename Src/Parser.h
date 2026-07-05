//
// Created by Abhinav Jha on 10/05/26.
//

#ifndef WISP_PARSER_H
#define WISP_PARSER_H

#include "AST.h"
#include "CompilationUnit.h"

#include <memory>
#include <optional>
#include <string>

namespace Wisp {

class Parser {
public:
    explicit Parser(CompilationUnit& comp_unit)
        : m_comp_unit(comp_unit)
        , m_current(0) {}

    void parse_comp_unit();

private:
    void parse_program();
    std::unique_ptr<StmtNode> parse_stmt();
    std::unique_ptr<PrintStmt> parse_print_stmt();
    std::unique_ptr<ExprStmt> parse_expr_stmt();
    std::unique_ptr<VarDeclStmt> parse_var_decl_stmt();
    std::unique_ptr<BlockStmt> parse_block_stmt();
    std::unique_ptr<IfStmt> parse_if_stmt();

    std::unique_ptr<ExprNode> parse_expr();
    std::unique_ptr<ExprNode> parse_assignment();
    std::unique_ptr<ExprNode> parse_logical();
    std::unique_ptr<ExprNode> parse_term();
    std::unique_ptr<ExprNode> parse_factor();
    std::unique_ptr<ExprNode> parse_unary();
    std::unique_ptr<ExprNode> parse_primary();
    std::unique_ptr<ExprNode> parse_literal();
    std::optional<const Token*> current_token() const;
    void advance();
    bool match(TokenKind kind, std::string msg = "");
    bool is_eof() const {
        return m_current >= m_comp_unit.tokens.size() || m_comp_unit.tokens[m_current].kind == TokenKind::Eof;
    }

private:
    CompilationUnit& m_comp_unit;
    std::size_t m_current;
};

}   // namespace Wisp

#endif //WISP_PARSER_H
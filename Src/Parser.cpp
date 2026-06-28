//
// Created by Abhinav Jha on 10/05/26.
//

#include "Parser.h"

#include "AST.h"
#include "CompilationUnit.h"
#include "Token.h"

#include <charconv>
#include <format>
#include <memory>
#include <optional>
#include <string>

namespace Wisp {

void Parser::parse_comp_unit() {
    if (m_comp_unit.tokens.empty()) {
        std::string msg = "No tokens found. Files must contain Wisp source code.";
        m_comp_unit.diagnostics.push_back(Diagnostic{ 1, std::move(msg) });
        return;
    }

    parse_program();
}

void Parser::parse_program() {
    std::unique_ptr<Program> program = std::make_unique<Program>();

    while (!is_eof()) {
        auto stmt = parse_stmt();
        if (!stmt) {
            break;
        }
        program->add_stmt(std::move(stmt));
    }

    m_comp_unit.program = std::move(program);
}

std::unique_ptr<StmtNode> Parser::parse_stmt() {
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return nullptr;
    }

    switch (curr_token.value()->kind) {
        case TokenKind::Print:
            return parse_print_stmt();
        case TokenKind::Var:
        case TokenKind::Let:
            return parse_var_decl_stmt();
        default:
            return parse_expr_stmt();
    }
}

std::unique_ptr<PrintStmt> Parser::parse_print_stmt() {
    auto line = current_token().value()->line;
    advance();

    auto expr = parse_expr();
    if (!expr) {
        return nullptr;
    }

    if (!match(TokenKind::Semicolon)) {
        return nullptr;
    }
    return std::make_unique<PrintStmt>(std::move(expr), line);
}

std::unique_ptr<ExprStmt> Parser::parse_expr_stmt() {
    auto expr = parse_expr();
    if (!expr) {
        return nullptr;
    }

    if (!match(TokenKind::Semicolon)) {
        return nullptr;
    }
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<VarDeclStmt> Parser::parse_var_decl_stmt() {
    auto is_mutable = current_token().value()->kind == TokenKind::Var;
    auto line = current_token().value()->line;
    advance();

    auto name_tok = current_token();
    if (!match(TokenKind::Ident)) {
        m_comp_unit.diagnostics.push_back(Diagnostic{
            .line = current_token().value()->line,
            .message = "Expected identifier" });
        return nullptr;
    }

    auto name = std::string(name_tok.value()->lexeme);

    if (!match(TokenKind::Equals)) {
        m_comp_unit.diagnostics.push_back(Diagnostic{
            .line = current_token().value()->line,
            .message = "Expected equals sign" });
        return nullptr;
    }

    auto value = parse_expr();
    if (!match(TokenKind::Semicolon)) {
        return nullptr;
    }

    return std::make_unique<VarDeclStmt>(std::move(name), std::move(value), is_mutable, line);
}

std::unique_ptr<ExprNode> Parser::parse_expr() {
    switch (current_token().value()->kind) {
        case TokenKind::Ident:
            return parse_ident_expr();
        default:
            return parse_logical();
    }
}

std::unique_ptr<ExprNode> Parser::parse_ident_expr() {
    auto name = std::string(current_token().value()->lexeme);
    advance();

    const auto* curr_tok = current_token().value();
    if (curr_tok->kind == TokenKind::Equals) {
        advance();
        auto value = parse_expr();
        if (!value) {
            return nullptr;
        }
        return std::make_unique<AssignExpr>(std::move(name), std::move(value), curr_tok->line);
    }

    if (curr_tok->kind == TokenKind::Semicolon) {
        return std::make_unique<VarExpr>(std::move(name), curr_tok->line);
    }

    m_comp_unit.diagnostics.push_back(Diagnostic{
        .line = curr_tok->line,
        .message = "Expected identifier expression or assign expression" });
    return nullptr;
}


std::unique_ptr<ExprNode> Parser::parse_logical() {
    auto left = parse_term();
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return left;
    }

    auto kind = curr_token.value()->kind;
    while (
        kind == TokenKind::GreaterThan
        || kind == TokenKind::LessThan
        || kind == TokenKind::GreaterThanEquals
        || kind == TokenKind::LessThanEquals
        || kind == TokenKind::EqualsEquals
        || kind == TokenKind::NotEquals) {

        auto op = kind;
        auto op_line = curr_token.value()->line;
        advance();
        auto right = parse_term();
        if (!left || !right) {
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right), op_line);

        curr_token = current_token();
        if (!curr_token.has_value() ) {
            break;
        }
        kind = curr_token.value()->kind;
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parse_term() {
    auto left = parse_factor();
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return left;
    }

    auto kind = curr_token.value()->kind;
    while (kind == TokenKind::Plus || kind == TokenKind::Minus) {
        auto op = kind;
        auto op_line = curr_token.value()->line;
        advance();
        auto right = parse_factor();
        if (!left || !right) {
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right), op_line);

        curr_token = current_token();
        if (!curr_token.has_value() ) {
            break;
        }
        kind = curr_token.value()->kind;
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parse_factor() {
    auto left = parse_unary();
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return left;
    }

    auto kind = curr_token.value()->kind;
    while (kind == TokenKind::Star || kind == TokenKind::Slash) {
        auto op = kind;
        auto op_line = curr_token.value()->line;
        advance();
        auto right = parse_unary();
        if (!left || !right) {
            return nullptr;
        }
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right), op_line);

        curr_token = current_token();
        if (!curr_token.has_value() ) {
            break;
        }
        kind = curr_token.value()->kind;
    }

    return left;
}

std::unique_ptr<ExprNode> Parser::parse_unary() {
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return nullptr;
    }

    auto kind = curr_token.value()->kind;
    if (kind == TokenKind::Bang || kind == TokenKind::Minus) {
        auto op = kind;
        auto op_line = curr_token.value()->line;
        advance();
        auto expr = parse_unary(); // Recurse for things like !!true
        if (!expr) {
            return nullptr;
        }
        return std::make_unique<UnaryExpr>(op, std::move(expr), op_line);
    }

    return parse_primary();
}

std::unique_ptr<ExprNode> Parser::parse_primary() {
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return nullptr;
    }

    auto kind = curr_token.value()->kind;
    if (kind == TokenKind::LeftParen) {
        advance();
        auto expr = parse_expr();
        if (!match(TokenKind::RightParen)) {
            return nullptr;
        }
        return expr;
    }
    return parse_literal();
}

std::unique_ptr<ExprNode> Parser::parse_literal() {
    auto curr_token = current_token();
    if (!curr_token.has_value() ) {
        return nullptr;
    }

    switch (auto kind = curr_token.value()->kind; kind) {
        case TokenKind::IntLiteral: {
            std::int32_t value;
            auto line = curr_token.value()->line;
            auto result = std::from_chars(curr_token.value()->lexeme.begin(), curr_token.value()->lexeme.end(), value);
            if (result.ec != std::errc()) {
                m_comp_unit.diagnostics.push_back(Diagnostic{ .line = line, .message = "Invalid integer literal" });
                advance();
                return nullptr;
            }
            advance();
            return std::make_unique<LiteralExpr<std::int32_t>>(value, line);
        }
        case TokenKind::FloatLiteral: {
            float value;
            auto line = curr_token.value()->line;
            auto result = std::from_chars(curr_token.value()->lexeme.begin(), curr_token.value()->lexeme.end(), value);
            if (result.ec != std::errc()) {
                m_comp_unit.diagnostics.push_back(Diagnostic{ .line = line, .message = "Invalid float literal" });
                advance();
                return nullptr;
            }
            advance();
            return std::make_unique<LiteralExpr<float>>(value, line);
        }
        case TokenKind::StringLiteral: {
            auto line = curr_token.value()->line;
            auto value = std::string(curr_token.value()->lexeme);
            advance();
            return std::make_unique<LiteralExpr<std::string>>(value, line);
        }
        default:
            m_comp_unit.diagnostics.push_back(Diagnostic{ .line = curr_token.value()->line, .message = "Invalid literal type" });
            return nullptr;
    }
}

std::optional<const Token*> Parser::current_token() const {
    if (is_eof()) {
        return std::nullopt;
    }
    return &m_comp_unit.tokens[m_current];
}

void Parser::advance() {
    if (is_eof()) {
        return;
    }
    m_current++;
}

bool Parser::match(TokenKind kind, std::string msg) {
    if (m_current >= m_comp_unit.tokens.size()) {
        return false;
    }

    auto curr_token = current_token().value();
    if (curr_token->kind != kind) {
        if (msg.empty()) {
            msg = std::format(
                "Expected token of kind {0}, found {1}",
                token_kind_to_string(kind),
                token_kind_to_string(curr_token->kind));
        }
        m_comp_unit.diagnostics.push_back(Diagnostic{ .line = curr_token->line, .message = msg });
        return false;
    }

    advance();
    return true;
}


}   // namespace Wisp
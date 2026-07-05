//
// Created by Abhinav Jha on 26/04/26.
//

#include "Lexer.h"

#include <cstddef>
#include <format>
#include <optional>
#include <unordered_map>

namespace Wisp {

void Lexer::lex_comp_unit() {
    for (Token token = next_token(); token.kind != TokenKind::Eof; token = next_token()) {
        if (token.kind == TokenKind::Error) {
            m_comp_unit.diagnostics.push_back(
                Diagnostic{ token.line, std::string(std::format("Unknown token found: {0}", token.lexeme)) }
            );
        }
        m_comp_unit.tokens.push_back(token);
    }
    m_comp_unit.tokens.push_back(Token{TokenKind::Eof, "", m_line});
}

Token Lexer::next_token() {
    skip_whitespace();

    m_start = m_current;

    if (is_eof()) {
        return Token{TokenKind::Eof, "", m_line};
    }

    switch (const char c = advance()) {
        // Single-character tokens
        case '(': return make_token(TokenKind::LeftParen);
        case ')': return make_token(TokenKind::RightParen);
        case '{': return make_token(TokenKind::LeftCurly);
        case '}': return make_token(TokenKind::RightCurly);
        case ':': return make_token(TokenKind::Colon);
        case ';': return make_token(TokenKind::Semicolon);
        case ',': return make_token(TokenKind::Comma);
        case '+': return make_token(TokenKind::Plus);
        case '-': return make_token(TokenKind::Minus);
        case '*': return make_token(TokenKind::Star);
        case '/': return make_token(TokenKind::Slash);

        // One or two character tokens
        case '>': {
            if (match('=')) {
                return make_token(TokenKind::GreaterThanEquals);
            }
            return make_token(TokenKind::GreaterThan);
        }

        case '<': {
            if (match('=')) {
                return make_token(TokenKind::LessThanEquals);
            }
            return make_token(TokenKind::LessThan);
        }

        case '=': {
            if (match('=')) {
                return make_token(TokenKind::EqualsEquals);
            }
            return make_token(TokenKind::Equals);
        }

        case '!': {
            if (match('=')) {
                return make_token(TokenKind::NotEquals);
            }
            return make_token(TokenKind::Bang);
        }

        case '"': return make_string();

        default: {
            if (std::isalpha(c) || c == '_') {
                return make_ident();
            }
            if (std::isdigit(c)) {
                return make_num();
            }
            return Token{TokenKind::Error, "Unexpected character.", m_line};
        }
    }
}

char Lexer::advance() {
    if (is_eof()) {
        return '\0';
    }
    return m_comp_unit.source[m_current++];
}

bool Lexer::match(const char expected) {
    if (is_eof() || m_comp_unit.source[m_current] != expected) {
        return false;
    }
    m_current++;
    return true;
}

Token Lexer::make_token(const TokenKind kind) const {
    const auto lexeme = std::string_view(m_comp_unit.source).substr(m_start, m_current - m_start);
    return Token{kind, lexeme, m_line};
}

namespace {

const std::unordered_map<std::string_view, TokenKind> s_keywords = {
    {"let", TokenKind::Let},
    {"var", TokenKind::Var},
    {"fn", TokenKind::Fn},
    {"if", TokenKind::If},
    {"else", TokenKind::Else},
    {"for", TokenKind::For},
    {"return", TokenKind::Return},
    {"struct", TokenKind::Struct},
    {"class", TokenKind::Class},
    {"none", TokenKind::None},
    {"true", TokenKind::True},
    {"false", TokenKind::False},
    {"print", TokenKind::Print},

    {"Int", TokenKind::TypeInt},
    {"Float", TokenKind::TypeFloat},
    {"Bool", TokenKind::TypeBool},
    {"String", TokenKind::TypeString},
    {"Void", TokenKind::TypeVoid}
};

std::optional<TokenKind> keyword_kind(const std::string_view lexeme) {
    auto it = s_keywords.find(lexeme);
    if (it != s_keywords.end()) {
        return it->second;
    }
    return std::nullopt;
}

}

Token Lexer::make_ident() {
    while (std::isalnum(current_char()) || current_char() == '_') {
        advance();
    }
    const auto lexeme = std::string_view(m_comp_unit.source).substr(m_start, m_current - m_start);
    const auto type = keyword_kind(lexeme);
    if (type.has_value()) {
        return Token{type.value(), lexeme, m_line};
    }
    return Token{TokenKind::Ident, lexeme, m_line};
}

Token Lexer::make_num() {
    auto kind = TokenKind::IntLiteral;
    while (std::isdigit(current_char())) {
        advance();
    }
    if (current_char() == '.' && std::isdigit(peek())) {
        advance();
        while (std::isdigit(current_char())) {
            advance();
        }

        kind = TokenKind::FloatLiteral;
    }
    const auto lexeme = std::string_view(m_comp_unit.source).substr(m_start, m_current - m_start);
    return Token{kind, lexeme, m_line};
}

Token Lexer::make_string() {
    while (current_char() != '"') {
        if (is_eof()) {
            return Token{TokenKind::Error, "Unterminated string.", m_line};
        }
        if (current_char() == '\n') {
            m_line++;
        }
        advance();
    }
    advance();
    const auto lexeme = std::string_view(m_comp_unit.source).substr(m_start + 1, m_current - m_start - 2);
    return Token{TokenKind::StringLiteral, lexeme, m_line};
}

void Lexer::skip_whitespace() {
    while (true) {
        char c = current_char();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                m_line++;
                advance();
                break;
            case '/':
                if (peek() != '/') {
                    return;     // a division operator, not a comment
                }
                // line comment: consume until end of line, leaving the
                // '\n' for the next iteration so m_line is counted correctly
                while (current_char() != '\n' && !is_eof()) {
                    advance();
                }
                break;
            default:
                return;
        }
    }
}

char Lexer::current_char() const {
    if (is_eof()) {
        return '\0';
    }
    return m_comp_unit.source[m_current];
}

char Lexer::peek() const {
    if (m_current + 1 >= m_comp_unit.source.size()) {
        return '\0';
    }

    return m_comp_unit.source[m_current + 1];
}

bool Lexer::is_eof() const {
    return m_current >= m_comp_unit.source.size();
}

}   // namespace Wisp

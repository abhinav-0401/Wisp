//
// Created by Abhinav Jha on 26/04/26.
//

#ifndef WISP_LEXER_H
#define WISP_LEXER_H

#include "CompilationUnit.h"
#include "Token.h"

class Lexer {
public:
    explicit Lexer(CompilationUnit& comp_unit)
        : m_comp_unit(comp_unit)
        , m_start(0)
        , m_current(0)
        , m_line(1)
    {}
    void lex_comp_unit();

private:
    Token next_token();
    char current_char() const;
    char peek() const;
    bool is_eof() const;
    char advance();
    bool match(char expected);
    Token make_token(TokenKind kind) const;
    Token make_ident();
    Token make_num();
    Token make_string();
    void skip_whitespace();

    CompilationUnit& m_comp_unit;
    std::size_t m_start;
    std::size_t m_current;
    std::size_t m_line;
};

#endif //WISP_LEXER_H
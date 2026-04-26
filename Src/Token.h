//
// Created by Abhinav Jha on 26/04/26.
//

#ifndef WISP_TOKEN_H
#define WISP_TOKEN_H

#include <string_view>

enum class TokenKind {
    // Special
    Eof,
    Error,

    // Single-Character Punctuation
    LeftParen,    // (
    RightParen,   // )
    LeftCurly,    // {
    RightCurly,   // }
    Colon,        // :
    Semicolon,    // ;
    Comma,        // ,

    // Operators
    Plus,         // +
    Minus,        // -
    Asterisk,     // *
    Slash,        // /
    Equals,       // =
    EqualsEquals, // ==
    Bang,         // !
    NotEquals,   // !=
    GreaterThan,  // >
    LessThan,     // <

    // Literals
    Identifier,
    IntLiteral,
    FloatLiteral,
    StringLiteral,

    // Keywords
    Let,
    Var,
    Fn,
    If,
    Else,
    While,
    Return,
    Struct,
    Class,
    None,
    True,
    False,

    // Built-in Types
    TypeInt,
    TypeFloat,
    TypeBool,
    TypeString,
    TypeVoid
};

constexpr std::string_view token_kind_to_string(const TokenKind kind) {
    switch (kind) {
        case TokenKind::Eof: return "Eof";
        case TokenKind::Error: return "Error";

        case TokenKind::LeftParen: return "LeftParen";
        case TokenKind::RightParen: return "RightParen";
        case TokenKind::LeftCurly: return "LeftCurly";
        case TokenKind::RightCurly: return "RightCurly";
        case TokenKind::Colon: return "Colon";
        case TokenKind::Semicolon: return "Semicolon";
        case TokenKind::Comma: return "Comma";

        case TokenKind::Plus: return "Plus";
        case TokenKind::Minus: return "Minus";
        case TokenKind::Asterisk: return "Asterisk";
        case TokenKind::Slash: return "Slash";
        case TokenKind::Equals: return "Equals";
        case TokenKind::EqualsEquals: return "EqualsEquals";
        case TokenKind::Bang: return "BangEquals";
        case TokenKind::NotEquals: return "NotEquals";
        case TokenKind::GreaterThan: return "GreaterThan";
        case TokenKind::LessThan: return "LessThan";

        case TokenKind::Identifier: return "Identifier";
        case TokenKind::IntLiteral: return "IntLiteral";
        case TokenKind::FloatLiteral: return "FloatLiteral";
        case TokenKind::StringLiteral: return "StringLiteral";

        case TokenKind::Let: return "Let";
        case TokenKind::Var: return "Var";
        case TokenKind::Fn: return "Fn";
        case TokenKind::If: return "If";
        case TokenKind::Else: return "Else";
        case TokenKind::While: return "While";
        case TokenKind::Return: return "Return";
        case TokenKind::Struct: return "Struct";
        case TokenKind::Class: return "Class";
        case TokenKind::None: return "None";
        case TokenKind::True: return "True";
        case TokenKind::False: return "False";

        case TokenKind::TypeInt: return "TypeInt";
        case TokenKind::TypeFloat: return "TypeFloat";
        case TokenKind::TypeBool: return "TypeBool";
        case TokenKind::TypeString: return "TypeString";
        case TokenKind::TypeVoid: return "TypeVoid";

        default: break;
    }
    return "Unknown";
}

struct Token {
    TokenKind kind;
    std::string_view lexeme;
    std::size_t line;
};

#endif //WISP_TOKEN_H
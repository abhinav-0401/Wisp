#include "CompilationUnit.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"

#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
    using namespace Wisp;

    // std::string test_str = "var let x mytest fn = 78;\nif 3+==4;/&13-8!5.44;\n";
    std::string test_str = "2 + 3;";

    auto test_comp_unit = CompilationUnit{
        .filename = "TestFileString",
        .source = std::move(test_str),
        .tokens = std::vector<Token>(),
        .diagnostics = std::vector<Diagnostic>()
    };

    auto lexer = Lexer{ test_comp_unit };
    lexer.lex_comp_unit();

    for (const auto& token : test_comp_unit.tokens) {
        std::cout << token_kind_to_string(token.kind) << " " << token.lexeme << " " << token.line << "\n";
    }

    for (const auto&[line, message] : test_comp_unit.diagnostics) {
        std::cout << std::format("[{0}.wisp Line Number {1}]", test_comp_unit.filename, line) << ": " << message << "\n";
    }

    auto parser = Parser{ test_comp_unit };
    parser.parse_comp_unit();

    if (test_comp_unit.has_errors()) {
        for (const auto&[line, message] : test_comp_unit.diagnostics) {
            std::cout << std::format("[{0}.wisp Line Number {1}]", test_comp_unit.filename, line) << ": " << message << "\n";
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#include "CompilationUnit.h"
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "Token.h"

#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
    using namespace Wisp;

    std::string test_str = "(2 + 3;";

    auto test_comp_unit = CompilationUnit{
        .filename = "TestFileString",
        .source = std::move(test_str),
        .tokens = std::vector<Token>(),
        .diagnostics = std::vector<Diagnostic>()
    };

    auto lexer = Lexer{ test_comp_unit };
    lexer.lex_comp_unit();

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

    std::cout << "Parsing successful!\n";

    auto print_visitor = PrintVisitor{};
    test_comp_unit.program->accept(print_visitor);

    std::cout << print_visitor.output() << "\n";

    return EXIT_SUCCESS;
}

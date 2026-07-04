#include "CompilationUnit.h"
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "Interpreter.h"
#include "Token.h"

#include <cstdlib>
#include <format>
#include <iostream>
#include <vector>

static void print_diagnostics(const Wisp::CompilationUnit& comp_unit) {
    for (const auto& [line, message] : comp_unit.diagnostics) {
        std::cout << std::format("[{}.wisp Line {}]: {}\n", comp_unit.filename, line, message);
    }
}

static void print_lexer_tokens(const Wisp::CompilationUnit& comp_unit) {
    for (const auto& token : comp_unit.tokens) {
        std::cout << std::format("Token: {0} ({1})\n", token.lexeme, Wisp::token_kind_to_string(token.kind));
    }
}

int main() {
    using namespace Wisp;

    std::string test_str = "print 5 * (2 + 1);\n var x = 5;\n print x;\n let y = x / 2;\n print y;\n x = \"something\"; print x;";
    std::string test_block = "var x = \"hello\";\n print x;\n { print \"hello from inside\";\n var y = 10;\n y = 11;\n print y;\n print x;\n }";

    auto test_comp_unit = CompilationUnit{
        .filename = "TestFileString",
        .source = std::move(test_block),
        .tokens = std::vector<Token>(),
        .diagnostics = std::vector<Diagnostic>()
    };

    auto lexer = Lexer{ test_comp_unit };
    lexer.lex_comp_unit();

    if (test_comp_unit.has_errors()) {
        print_diagnostics(test_comp_unit);
        return EXIT_FAILURE;
    }

    print_lexer_tokens(test_comp_unit);

    auto parser = Parser{ test_comp_unit };
    parser.parse_comp_unit();

    if (test_comp_unit.has_errors()) {
        print_diagnostics(test_comp_unit);
        return EXIT_FAILURE;
    }

    std::cout << "Parsing successful!\n";

    auto print_visitor = ASTPrinter{};
    test_comp_unit.program->accept(print_visitor);
    std::cout << print_visitor.output() << "\n";

    auto interpreter = Interpreter{ test_comp_unit };
    test_comp_unit.program->accept(interpreter);

    if (test_comp_unit.has_errors()) {
        print_diagnostics(test_comp_unit);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

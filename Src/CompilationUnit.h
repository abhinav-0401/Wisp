//
// Created by Abhinav Jha on 25/04/26.
//

#ifndef WISP_COMPILATION_UNIT_H
#define WISP_COMPILATION_UNIT_H

#include "AST.h"
#include "Token.h"

#include <memory>
#include <string>
#include <vector>

namespace Wisp {

struct Diagnostic {
    std::size_t line;
    std::string message;
};

struct CompilationUnit {
    std::string filename = "source file";
    std::string source;
    std::vector<Token> tokens;
    std::unique_ptr<Program> program;
    std::vector<Diagnostic> diagnostics;

    bool has_errors() const {
        return !diagnostics.empty();
    }
};

}   // namespace Wisp

#endif //WISP_COMPILATION_UNIT_H

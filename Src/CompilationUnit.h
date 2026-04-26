//
// Created by Abhinav Jha on 25/04/26.
//

#ifndef WISP_COMPILATION_UNIT_H
#define WISP_COMPILATION_UNIT_H

#include "Token.h"

#include <string>
#include <vector>

struct Diagnostic {
    std::size_t line;
    std::string message;
};

struct CompilationUnit {
    std::string filename = "source file";
    std::string source;
    std::vector<Token> tokens;
    std::vector<Diagnostic> diagnostics;

    bool has_errors() const {
        return !diagnostics.empty();
    }
};

#endif //WISP_COMPILATION_UNIT_H
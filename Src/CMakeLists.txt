target_sources(Wisp PRIVATE
               ASTPrinter.cpp
               CompilationUnit.cpp
               Environment.cpp
               Interpreter.cpp
               Lexer.cpp
               Main.cpp
               Parser.cpp

               AST.h
               ASTPrinter.h
               CompilationUnit.h
               Environment.h
               Interpreter.h
               Lexer.h
               NodeVisitor.h
               Parser.h
               Token.h
               WispConcepts.h
               WispValue.h
)

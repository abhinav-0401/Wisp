target_sources(Wisp PRIVATE
               Lexer.cpp
               Main.cpp
               Parser.cpp
               PrintVisitor.cpp

               AST.h
               CompilationUnit.h
               Lexer.h
               NodeVisitor.h
               Parser.h
               PrintVisitor.h
               Token.h
)
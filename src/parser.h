#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "tokeniser.h"
#include "ast.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

    bool isAtEnd();
    Token peek();
    Token advance();
    bool match(TokenType type);
    void error(const std::string& message);

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parseProgram();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseUnary();    
    std::unique_ptr<Expr> parsePrimary();
};

#endif // PARSER_H
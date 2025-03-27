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
    bool isEvaluateMode;



    bool isAtEnd();
    Token peek();
    Token advance();
    bool match(TokenType type);
    void error(const Token& token, const std::string& message);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type);

    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parsePrintStatement();
    std::unique_ptr<Stmt> parseVarDeclaration();
    std::unique_ptr<Stmt> parseBlock();

public:
   Parser(const std::vector<Token>& tokens, bool isEvaluateMode = false);
    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseAssignment();
};

#endif // PARSER_H
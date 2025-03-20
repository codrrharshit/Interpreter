#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens, bool isEvaluateMode)
    : tokens(tokens), isEvaluateMode(isEvaluateMode) {}

bool Parser::isAtEnd() {
    return current >= tokens.size();
}

Token Parser::peek() {
    if (isAtEnd()) return tokens.back();
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const Token& token, const std::string& message) {
    std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << std::endl;
    
    exit(65);
}

// Parse the whole program, collecting all statements
std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();

     while (current < tokens.size()-1) {  // Keep parsing until EOF
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        
    }
    
    return program;
}

// Parse a statement (right now, only handling `print` statements)
std::unique_ptr<Stmt> Parser::parseStatement() {
    if (current >= tokens.size()) return nullptr;
    if (peek().type == TokenType::KEYWORD && peek().lexeme == "print") {
        match(TokenType::KEYWORD);  // Now we only consume "print"
        return parsePrintStatement();
    }
    auto expr = parseExpression();

    if(!isEvaluateMode){
      if (!match(TokenType::SEMICOLON)) {
        error(tokens[current - 1], "Expected ';' after expression.");
    }  
    }
    
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

// Parse a `print` statement
std::unique_ptr<Stmt> Parser::parsePrintStatement() {
    auto expr = parseExpression();
    if (!match(TokenType::SEMICOLON)) {
        error(peek(), "Expected ';' after print statement.");
    }
    return std::make_unique<PrintStmt>(std::move(expr));
}

// Parse expressions
std::unique_ptr<Expr> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) ||
           match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        
        std::string op = tokens[current - 1].lexeme;
        auto right = parseTerm(); 
       if(match( TokenType::LESS_EQUAL)){
        std::cout<<right;
       }
        if (!right) {
            error(peek(), "Missing right-hand expression after comparison operator.");
        }
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// Handles + and -
std::unique_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handles * and /
std::unique_ptr<Expr> Parser::parseFactor() {
    auto expr = parseUnary();

    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handles - and !
std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::MINUS) || match(TokenType::BANG)) {  
        Token op = tokens[current - 1];  
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    return parsePrimary();
}

// Handles literals, identifiers, and grouping
std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        double value = std::stod(tokens[current - 1].lexeme);
        return std::make_unique<LiteralExpr>(value);
    } else if (match(TokenType::STRING)) {
        std::string rawString = tokens[current - 1].lexeme;
        if (rawString.front() == '"' && rawString.back() == '"') {
            rawString = rawString.substr(1, rawString.length() - 2);
        }
        return std::make_unique<LiteralExpr>(rawString);
    } else if (match(TokenType::KEYWORD)) {
        if (tokens[current - 1].lexeme == "true") {
            return std::make_unique<LiteralExpr>(true);
        } else if (tokens[current - 1].lexeme == "false") {
            return std::make_unique<LiteralExpr>(false);
        } else if (tokens[current - 1].lexeme == "nil") {
            return std::make_unique<LiteralExpr>(NilValue());
        }
        return std::make_unique<LiteralExpr>(tokens[current - 1].lexeme);
    } else if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(tokens[current - 1]); 
    } else if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::RIGHT_PAREN)) {
            error(tokens[current - 1], "Expected ')' after expression.");
        }
        return std::make_unique<GroupingExpr>(std::move(expr));
    }
    

    error(peek(), "Expect expression.");
    return nullptr;
}

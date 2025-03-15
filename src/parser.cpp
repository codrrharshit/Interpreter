#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

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
    throw std::runtime_error(message);  // Throw an exception instead of exiting
}

std::unique_ptr<Expr> Parser::parseProgram() {
    try {
        return parseExpression();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        exit(65);  
        
    }
}

std::unique_ptr<Expr> Parser::parseExpression() {
    return parseEquality();


}

std::unique_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();  // Parse left-hand side first

    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();  // Parse lower-precedence terms first

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) ||
           match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseTerm();  // Parse the next term
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// Handles + and -
std::unique_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();  // First parse multiplication/division

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handles * and /
std::unique_ptr<Expr> Parser::parseFactor() {
    auto expr = parseUnary();  // Parse unary before factor

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
        auto right = parseUnary();  // Recursively parse operand
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    return parsePrimary();  // If no unary operator, parse primary expressions
}

// Handles literals, keywords, and grouping
std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        double value = std::stod(tokens[current-1].lexeme);
       
        return std::make_unique<LiteralExpr>(value);
    } else if (match(TokenType::STRING)) {
        std::string rawString = tokens[current - 1].lexeme;

        // Remove surrounding quotes if they exist
        if (rawString.front() == '"' && rawString.back() == '"') {
            rawString = rawString.substr(1, rawString.length() - 2);
        }
        
        return std::make_unique<LiteralExpr>(rawString);
    } else if (match(TokenType::KEYWORD)) {
        if (tokens[current - 1].lexeme == "true") {
            return std::make_unique<LiteralExpr>("true");
        } else if (tokens[current - 1].lexeme == "false") {
            return std::make_unique<LiteralExpr>("false");
        } else if (tokens[current - 1].lexeme == "nil") {
            return std::make_unique<LiteralExpr>("nil");
        }
        else {
            return std::make_unique<LiteralExpr>(tokens[current-1].lexeme);
        }
        // error(tokens[current - 1 ],"Unexpected keyword.");
        return nullptr;
    }else if (match(TokenType::IDENTIFIER)){
        return std::make_unique<VariableExpr>(tokens[current-1]); 
    }
    
    else if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::RIGHT_PAREN)) {
            error(tokens[current-1], "Expected ')' after expression.");
            return nullptr;  // Ensure function returns a value
           
        }
        return std::make_unique<GroupingExpr>(std::move(expr));
    } 

    error(peek(), "Expect expression.");
    return nullptr;
}

#include "parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens, bool isEvaluateMode)
    : tokens(tokens), isEvaluateMode(isEvaluateMode) {}

bool Parser::isAtEnd() {
    return current >= tokens.size();
}
bool Parser::check(TokenType type)  {
    return !isAtEnd() && peek().type == type;
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

// Consume a token of expected type or throw an error
Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    error(peek(), message);
    return Token{type, "","", -1};  // Dummy return to satisfy compiler
}

// Parse the whole program, collecting all statements
std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();

    while (current < tokens.size() - 1) {  // Keep parsing until EOF
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }

    return program;
}

// Parse a statement (handling var declarations, assignments, print, and expressions)
std::unique_ptr<Stmt> Parser::parseStatement() {
    if (current >= tokens.size()) return nullptr;

    // Handle variable declaration (var x = 10;)
    if (peek().type == TokenType::KEYWORD && peek().lexeme == "var") {
        return parseVarDeclaration();
    }

    if (peek().type == TokenType::KEYWORD && peek().lexeme == "print") {
        match(TokenType::KEYWORD);
        return parsePrintStatement();
    }

    if (peek().type==TokenType::LEFT_BRACE) {
        return parseBlock();  // Call parseBlock() when encountering '{'
    }

    auto expr = parseExpression();

    // Handle assignment statements (x = 42;)
    if (match(TokenType::EQUAL)) {
        auto value = parseExpression();
        if (!match(TokenType::SEMICOLON)) {
            error(tokens[current - 1], "Expected ';' after assignment.");
        }
        return std::make_unique<AssignStmt>(dynamic_cast<VariableExpr*>(expr.get())->name, std::move(value));
    }

    if (!isEvaluateMode && !match(TokenType::SEMICOLON)) {
        error(tokens[current - 1], "Expected ';' after expression.");
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

// Parse a variable declaration (`var x = 10;`)
std::unique_ptr<Stmt> Parser::parseVarDeclaration() {
    match(TokenType::KEYWORD);  // Consume 'var'
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = parseExpression();  // Parse assigned value
    }

    if (!match(TokenType::SEMICOLON)) {
        error(peek(), "Expected ';' after variable declaration.");
    }

    return std::make_unique<VarDeclStmt>(name.lexeme, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::parseBlock() {
    std::vector<std::unique_ptr<Stmt>> statements;

    // Consume the '{' (assuming current token is already '{')
    consume(TokenType::LEFT_BRACE, "Expect '{' at the beginning of a block.");

    // Keep parsing statements until we hit '}'
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());  // Parse each statement
    }

    // Consume the '}'
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return std::make_unique<BlockStmt>(std::move(statements));
}
// Parse expressions
std::unique_ptr<Expr> Parser::parseExpression() {
    return parseAssignment();
}

// Handle assignment expressions (`x = 5;`)
std::unique_ptr<Expr> Parser::parseAssignment() {
    auto expr = parseEquality();

    if (match(TokenType::EQUAL)) {
        Token equals = tokens[current - 1];
        auto value = parseAssignment();

        if (auto variableExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(variableExpr->name, std::move(value));
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

// Handle `==` and `!=`
std::unique_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handle `<`, `<=`, `>`, `>=`
std::unique_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) ||
           match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseTerm();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handle `+` and `-`
std::unique_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handle `*` and `/`
std::unique_ptr<Expr> Parser::parseFactor() {
    auto expr = parseUnary();

    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        std::string op = tokens[current - 1].lexeme;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// Handle `-` and `!`
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
    } else if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(tokens[current - 1].lexeme);  // Handle variables
    } else if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    error(peek(), "Expect expression.");
    return nullptr;
}

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector> 

enum class TokenType {
    IDENTIFIER, KEYWORD, NUMBER, STRING,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, STAR, SLASH,
    EQUAL, EQUAL_EQUAL, BANG, BANG_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    EOF_TOKEN, ERROR,TRUE,FALSE,NIL,UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line;

    Token(TokenType type, std::string lexeme, std::string literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}
};

int tokenize(const std::string& file_contents ,std::vector<Token>& tokens);

#endif // TOKENIZER_H

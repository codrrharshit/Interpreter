#include "tokeniser.h"
#include "parser.h"
#include "evaluator.h"
#include <fstream>
#include <iostream>
#include <sstream>



using namespace std;

std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    switch (type) {
        case TokenType::IDENTIFIER: os << "IDENTIFIER"; break;
        case TokenType::KEYWORD: os << "KEYWORD"; break;
        case TokenType::NUMBER: os << "NUMBER"; break;
        case TokenType::STRING: os << "STRING"; break;
        case TokenType::EOF_TOKEN: os << "EOF"; break;
        case TokenType::LEFT_PAREN: os << "LEFT_PAREN"; break;
        case TokenType::RIGHT_PAREN: os << "RIGHT_PAREN"; break;
        case TokenType::LEFT_BRACE: os << "LEFT_BRACE"; break;
        case TokenType::RIGHT_BRACE: os << "RIGHT_BRACE"; break;
        case TokenType::COMMA: os << "COMMA"; break;
        case TokenType::DOT: os << "DOT"; break;
        case TokenType::MINUS: os << "MINUS"; break;
        case TokenType::PLUS: os << "PLUS"; break;
        case TokenType::SEMICOLON: os << "SEMICOLON"; break;
        case TokenType::STAR: os << "STAR"; break;
        case TokenType::SLASH: os << "SLASH"; break;
        case TokenType::EQUAL: os << "EQUAL"; break;
        case TokenType::EQUAL_EQUAL: os << "EQUAL_EQUAL"; break;
        case TokenType::BANG: os << "BANG"; break;
        case TokenType::BANG_EQUAL: os << "BANG_EQUAL"; break;
        case TokenType::LESS: os << "LESS"; break;
        case TokenType::LESS_EQUAL: os << "LESS_EQUAL"; break;
        case TokenType::GREATER: os << "GREATER"; break;
        case TokenType::GREATER_EQUAL: os << "GREATER_EQUAL"; break;
        // Add cases for all TokenType values
        default: os << "UNKNOWN"; break;
    }
    return os;
}


std::string read_file_contents(const std::string& filename);

int main(int argc, char *argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    // std::cerr << "Logs from your program will appear here!" << std::endl;

    if (argc < 3) {
        std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
        return 1;
    }

    const std::string command = argv[1];
    vector<Token>tokens ;
    int result ;
    string file_contents = read_file_contents(argv[2]);
    result= tokenize(file_contents,tokens);


    if (command == "tokenize") {
        
        for (const Token &token:tokens){
            if(token.type!=TokenType::KEYWORD){
                cout<<token.type<<" "<<token.lexeme<<" "<<token.literal<<endl;
            }
            else if (token.type == TokenType::EOF_TOKEN){
                cout<<"EOF  null"<<endl;
            }
            else{
                string keyword=token.lexeme;
                transform(keyword.begin(),keyword.end(),keyword.begin(),::toupper);
                cout<<keyword<<" "<<token.lexeme<<" "<<token.literal<<endl;
            }
        }
    } 

    
  
    else if (command=="parse"){
        Parser parser(tokens);
    auto ast = parser.parseProgram();
        if (ast) {
            std::cout << ast->toString() << std::endl;
        }
    }
    else if (command =="evaluate"){
        Parser parser(tokens);
        auto ast = parser.parseProgram();
        Evaluator evaluator;
        std::cout<<evaluator.evaluate(ast)<<std::endl;

    }
    else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }

    return result;
}

std::string read_file_contents(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error reading file: " << filename << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

#include "tokeniser.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

unordered_set<string> keywords={"and","class","else","false","for","fun","if","nil","or","print","return","super","this","true","var","while"};
// function for scanning identifiers 
vector<Token> scannedIdentifier( const string &input, int &i, int &line) {
    vector<Token> tokens;
    string identifier = "";

    // Check for the initials
    if (isalpha(input[i]) || input[i] == '_') {
        identifier += input[i];
        i++;
        while (i < input.length() && (isalnum(input[i]) || input[i] == '_')) {
            identifier += input[i];
           
            i++;
        }

        if (keywords.find(identifier) != keywords.end()) {
            tokens.push_back(Token(TokenType::KEYWORD, identifier, "null", line)); // Add keyword token
        } else {
            tokens.push_back(Token(TokenType::IDENTIFIER, identifier, "null", line)); // Add identifier token
        }
    }
    i--; // Adjust index after scanning
    return tokens;
}

int tokenize(const std::string& file_contents ,std::vector<Token>& tokens){
        bool haderror=false ;
        int line=1;
        
        // Uncomment this block to pass the first stage
        // 
        if (!file_contents.empty()) {

            for (int  i = 0; i < file_contents.length(); i++) {
                char ch = file_contents[i];
        
                switch (ch) {
                    case '(': tokens.push_back(Token(TokenType::LEFT_PAREN, "(", "null", line)); break;
                case ')': tokens.push_back(Token(TokenType::RIGHT_PAREN, ")", "null", line)); break;
                case '{': tokens.push_back(Token(TokenType::LEFT_BRACE, "{", "null", line)); break;
                case '}': tokens.push_back(Token(TokenType::RIGHT_BRACE, "}", "null", line)); break;
                case ',': tokens.push_back(Token(TokenType::COMMA, ",", "null", line)); break;
                case '.': tokens.push_back(Token(TokenType::DOT, ".", "null", line)); break;
                case '-': tokens.push_back(Token(TokenType::MINUS, "-", "null", line)); break;
                case '+': tokens.push_back(Token(TokenType::PLUS, "+", "null", line)); break;
                case ';': tokens.push_back(Token(TokenType::SEMICOLON, ";", "null", line)); break;
                case '*': tokens.push_back(Token(TokenType::STAR, "*", "null", line)); break;
                    //case '\n': line++; break; // Increment line number on newline
                    
                    // Handling `=` and `==`
                    case '=':
                        if (i + 1 < file_contents.length() && file_contents[i + 1] == '=') {
                            tokens.push_back(Token(TokenType::EQUAL_EQUAL, "==", "null", line));
                            i++; // Skip next character since we already consumed it
                        } else {
                            tokens.push_back(Token(TokenType::EQUAL, "=", "null", line));
                        }
                        break;
        
                    case '!':
                        if(i+1<file_contents.length() && file_contents[i+1]=='='){
                            tokens.push_back(Token(TokenType::BANG_EQUAL, "!=", "null", line));
                            i++;
                            break;
                        }
                        else {
                            tokens.push_back(Token(TokenType::BANG, "!", "null", line));
                            break;
                        }

                    case '<':
                        if(i+1<file_contents.length() && file_contents[i+1]=='='){
                            tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", "null", line));
                            i++;
                            break;
                        }
                        else {
                            tokens.push_back(Token(TokenType::LESS, "<", "null", line));
                            break;
                        }

                    case '>':
                        if(i+1<file_contents.length() && file_contents[i+1]=='='){
                            tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", "null", line));
                            i++;
                            break;
                        }
                        else {
                            tokens.push_back(Token(TokenType::GREATER, ">", "null", line));
                            break;
                        }

                    case '/':
                        if(i+1<file_contents.length() && file_contents[i+1]=='/'){
                            i++;
                            // we have to skip everything 
                           while(i<file_contents.length() && file_contents[i]!='\n'){
                                i++;
                           }
                           line++;
                        }
                        else {
                            tokens.push_back(Token(TokenType::SLASH, "/", "null", line));
                            
                        }
                        break;

                    case ' ': break;
                    case '\t':break;
                    case '\n':
                        line++;
                        break;

                    // scanning the string literal 
                    case '"':{
                        string s="";
                        i++;
                        while(i<file_contents.length() && file_contents[i]!='"'){
                            if(file_contents[i]=='\n'){
                                line++;
                            }
                            s.push_back(file_contents[i]);
                            i++;
                        }

                        if(i>=file_contents.length()){
                            haderror=true;
                            cerr << "[line " << line << "] Error: Unterminated string." << endl;
                            break;
                        }

                        tokens.push_back(Token(TokenType::STRING, "\"" + s + "\"", s, line));
                        break;
                    }

                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9': {
                        string num;
                        size_t start = i;
                        bool hasDecimal = false;
        
                        while (i < file_contents.length() && (isdigit(file_contents[i]) || file_contents[i] == '.')) {
                            if (file_contents[i] == '.') {
                                if (hasDecimal) break; // Prevent multiple decimal points
                                hasDecimal = true;
                            }
                            num += file_contents[i];
                            i++;
                        }
                        i--; // Roll back one step as loop overshoots
        
                        
                        bool isInteger = true;
                        string decimalPart;
                        if (hasDecimal) {
                            size_t decimalPos = num.find('.');
                            decimalPart = num.substr(decimalPos + 1);
                            for (char c : decimalPart) {
                                if (c != '0') {
                                    isInteger = false;
                                    break;
                                }
                            }
                        }
                        string literal;
                        if (isInteger) {
                            // Print as an integer
                            literal = to_string(stoi(num)) + ".0";
                        } else {
                            // Print as a floating-point number with the original precision
                            literal = to_string(stoi(num)) + "." + decimalPart ;
                        }

                        tokens.push_back(Token(TokenType::NUMBER,num,literal,line));
                        break;
                    }


                        
                    default:
                        if(isalpha(ch)|| ch=='_'){
                            vector<Token> idTokens = scannedIdentifier(file_contents, i, line);
                            tokens.insert(tokens.end(), idTokens.begin(), idTokens.end());  
                        }
                        else {
                            cerr << "[line "<<line <<"] Error: Unexpected character: " << ch << endl;
                            haderror = true;
                             
                        }
                        break;
                }
            }
            tokens.push_back(Token(TokenType::EOF_TOKEN, "", "null", line));
            return haderror ? 65 : 0;
        }
        tokens.push_back(Token(TokenType::EOF_TOKEN, "", "null", line));
        return 0;
        
}
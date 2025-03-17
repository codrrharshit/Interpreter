#include "evaluator.h"
#include <stdexcept>
#include <cctype>
#include <cmath>

// complex data types 

std::string formatNumbers( const std::string &str){
    try
    {
        size_t pos;
        double num= std::stod(str,&pos);
        if(pos!=str.size()){
            return str;
        }


        if(num== static_cast<int>(num)){
            return std::to_string(static_cast<int>(num));
        }

        std::ostringstream out;
        out<<std::fixed<<std::setprecision(6)<<num;
        std::string formatted= out.str();

        formatted.erase(formatted.find_last_not_of('0') + 1, std::string::npos);
        if (formatted.back() == '.') formatted.pop_back();
        return formatted;
    }
    catch(const std::invalid_argument&)
    {
        return str;
    }
    
}

std::string Evaluator::evaluate(std::unique_ptr<Expr>& expr) {
    return evaluateExpr(expr.get()).value;
}

Evalstr Evaluator::evaluateExpr(Expr* expr) {
    if (auto binary = dynamic_cast<BinaryExpr*>(expr)) {
        return evaluateBinary(binary);
    } else if (auto literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (std::holds_alternative<std::string>(literal->value)) {
            return {std::get<std::string>(literal->value),false};
              // Extract string correctly
        } else if (std::holds_alternative<double>(literal->value)) {
            double num = std::get<double>(literal->value);
            if (num == static_cast<int>(num)) {
                return {std::to_string(static_cast<int>(num)),true}; // Return as integer string
            }
            std::ostringstream out;
            out << std::fixed << std::setprecision(3) << num; // Format with 3 decimal places
            std::string str = out.str();
            str.erase(str.find_last_not_of('0') + 1, std::string::npos); // Remove trailing zeros
            if (!str.empty() && str.back() == '.') str.pop_back(); // Remove trailing decimal
            return {str,true};
            }
    } else if (auto unary = dynamic_cast<UnaryExpr*>(expr)) {
        return evaluateUnary(unary);
    }
    else if (auto grouping = dynamic_cast<GroupingExpr*>(expr)) {
        return evaluateExpr(grouping->expression.get());  
    } 
    throw std::runtime_error("Unknown expression.");
}

Evalstr Evaluator::evaluateBinary(BinaryExpr* expr) {
    Evalstr LEFT = evaluateExpr(expr->left.get());
    Evalstr RIGHT = evaluateExpr(expr->right.get());

    std::string left= LEFT.value;
    std::string right=RIGHT.value;


    if(expr->op=="=="){
        if(LEFT.wasNumber && RIGHT.wasNumber){
            return {left==right?"true":"false",false};
        }
        else if(!LEFT.wasNumber && !RIGHT.wasNumber){
            return {left==right?"true":"false",false};
        }
        else {
             return {"false",false};
        }
    }

    if(expr->op=="!="){
        if(LEFT.wasNumber && RIGHT.wasNumber){
            return {left!=right?"true":"false",false};
        }
        else if(!LEFT.wasNumber && !RIGHT.wasNumber){
            return {left!=right?"true":"false",false};
        }
        else {
             return {"false",false};
        }
    }


    if (isNumber(left) && isNumber(right)) {
        double leftNum = std::stod(left);
        double rightNum = std::stod(right);
        if (expr->op == "+") return {formatNumbers(std::to_string(leftNum + rightNum)),true};
        if (expr->op == "-") return {formatNumbers( std::to_string(leftNum - rightNum)),true};
        if (expr->op == "*") return {formatNumbers( std::to_string(leftNum * rightNum)),true};
        if (expr->op == "/") return {rightNum != 0 ? formatNumbers(std::to_string(leftNum / rightNum) ): "ERROR",true};
        if( expr->op ==">") return {leftNum>rightNum?"true" : "false",false};
        if( expr->op == ">=") return {leftNum>= rightNum ?"true" :"false",false};
        if( expr->op == "<") return {leftNum< rightNum ?"true" :"false",false};
        if( expr->op == "<=") return {leftNum<= rightNum ?"true" :"false",false};
    }
    if (expr->op == "+") {
        return {left + right,false};  // Concatenation
    }
   
    throw std::runtime_error("Invalid binary operation.");
}

Evalstr Evaluator::evaluateUnary(UnaryExpr* expr) {
    Evalstr RIGHT= evaluateExpr(expr->right.get());
    std::string right = RIGHT.value;
    size_t pos= right.find('.');
    if(expr->op.lexeme=="-") {
        if(RIGHT.wasNumber){
          if(pos==std::string::npos){
            return {std::to_string(-std::stoi(right)),true};
        }  
        }
        else {
            std::cerr << "Operand must be a number.\n[line " << expr->op.line << "]\n";
            exit(70); // Ensures correct runtime error code
        }
        
    }

   // if (expr->op.lexeme == "-") return std::to_string(-std::stod(right));
   
        if (expr->op.lexeme == "!") {
            // Convert numbers to booleans
            if (right == "nil" || right == "false") {
                return {"true",false};
            }
            if (right == "true") {
                return {"false",false};
            }
            
            if (right != "true" && right != "false") {
                double num = std::stod(right);
                right = (num == 0) ? "false" : "true";
            }
            return {(right == "true") ? "false" : "true",true};
        }
    

    throw std::runtime_error("Unsupported unary operator.");
}

bool Evaluator::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    char* end;
    std::strtod(str.c_str(), &end);
    
    return *end == '\0'; 
}

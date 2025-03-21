#include "evaluator.h"
#include <stdexcept>
#include <cctype>
#include <cmath>

// complex data types 
Evaluator::Evaluator(bool isEvaluatedMode) : isEvaluatedMode(isEvaluatedMode) {}

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


void Evaluator::evaluateStmt( const std::unique_ptr<Stmt>& stmt) {
    if (auto printStmt = dynamic_cast<PrintStmt*>(stmt.get())) {
        evaluatePrint(printStmt);
    }
    else if(auto expressionStmt= dynamic_cast<ExpressionStmt*>(stmt.get())){
        evaluateExpression(expressionStmt);
    }
    else if (auto varStmt = dynamic_cast<VarDeclStmt*>(stmt.get())) {
        evaluateVariable(varStmt);
    }
    else {
        throw std::runtime_error("Unknown statement type.");
    }
}
void Evaluator::evaluateProgram(const std::unique_ptr<Program>& program) {
    for (const auto& stmt : program->statements) {
        evaluateStmt(stmt);
    }
}


void Evaluator::evaluateExpression(ExpressionStmt *stmt){
    Evalstr result=evaluateExpr(stmt->expression.get());
    if(isEvaluatedMode){
        std::cout<<result.value<<std::endl;
    }
}

void Evaluator::evaluatePrint(PrintStmt* stmt) {
    Evalstr result = evaluateExpr(stmt->expression.get());
    std::cout << result.value << std::endl;
}
void Evaluator::evaluateVariable(VarDeclStmt* stmt) {
    if(stmt->initializer.get()==nullptr){
        Evalstr value={"nil","nil"};
        variables[stmt->name]=value;
    }
    else {
       Evalstr value = evaluateExpr(stmt->initializer.get());
        variables[stmt->name] = value;  // Store variable in map 
    }
    
}

Evalstr Evaluator::evaluateExpr(Expr* expr) {
    if (auto binary = dynamic_cast<BinaryExpr*>(expr)) {
        return evaluateBinary(binary);
    } else if (auto literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (std::holds_alternative<std::string>(literal->value)) {

            return {std::get<std::string>(literal->value),"string"};
              
        } else if (std::holds_alternative<double>(literal->value)) {
            double num = std::get<double>(literal->value);
            if (num == static_cast<int>(num)) {
                return {std::to_string(static_cast<int>(num)),"double"}; // Return as integer string
            }
            std::ostringstream out;
            out << std::fixed << std::setprecision(3) << num; // Format with 3 decimal places
            std::string str = out.str();
            str.erase(str.find_last_not_of('0') + 1, std::string::npos); // Remove trailing zeros
            if (!str.empty() && str.back() == '.') str.pop_back(); // Remove trailing decimal
            return {str,"double"};
            }
            else if(std::holds_alternative<bool>(literal->value)){
                return {std::get<bool>(literal->value)==true ?"true":"false","bool"};
            }
            else if(std::holds_alternative<NilValue>(literal->value)){
                return {"nil", "nil"};
            }
    } else if (auto unary = dynamic_cast<UnaryExpr*>(expr)) {
        return evaluateUnary(unary);
    }
    else if (auto grouping = dynamic_cast<GroupingExpr*>(expr)) {
        return evaluateExpr(grouping->expression.get());  
    } 

    else if (auto varExpr = dynamic_cast<VariableExpr*>(expr)) {
        if (variables.find(varExpr->name) == variables.end()) {
            std::cerr<<"Undefined Variable"<<varExpr->name<<std::endl;
            exit(70);
        }
        return variables[varExpr->name];
    }
    else if (auto assignExpr = dynamic_cast<AssignExpr*>(expr)) {
        if (variables.find(assignExpr->name) == variables.end()) {
            std::cerr << "Undefined variable '" << assignExpr->name << "'." << std::endl;
            exit(70);
        }
        
        Evalstr value = evaluateExpr(assignExpr->value.get()); // Evaluate RHS first
        variables[assignExpr->name] = value; // Assign the value
    
        return value; // Return assigned value for chaining
    }
    throw std::runtime_error("Unknown expression.");
}

Evalstr Evaluator::evaluateBinary(BinaryExpr* expr) {
    Evalstr LEFT = evaluateExpr(expr->left.get());
    Evalstr RIGHT = evaluateExpr(expr->right.get());

    std::string left= LEFT.value;
    std::string right=RIGHT.value;


    if(expr->op=="=="){
        if(LEFT.datatype=="double" && RIGHT.datatype=="double"){
            return {left==right?"true":"false","bool"};
        }
        else if(LEFT.datatype=="string" && RIGHT.datatype=="string"){
            return {left==right?"true":"false","bool"};
        }
       else if (LEFT.datatype=="bool" && RIGHT.datatype=="bool"){
         return {left==right?"true":"false","bool"};
       }
        else {
             return {"false","bool"};
        }
    }

    if(expr->op=="!="){
        if(LEFT.datatype=="double" && RIGHT.datatype=="double"){
            return {left!=right?"true":"false","bool"};
        }
        else if(LEFT.datatype=="string" && RIGHT.datatype=="string"){
            return {left!=right?"true":"false","bool"};
        }
        else if (LEFT.datatype=="bool" && RIGHT.datatype=="bool"){
            return {left!=right?"true":"false","bool"};
          }
        else {
             return {"false","bool"};
        }
    }


    if (isNumber(left) && isNumber(right)) {
        double leftNum = std::stod(left);
        double rightNum = std::stod(right);
        if (expr->op == "+") return {formatNumbers(std::to_string(leftNum + rightNum)),"double"};
        if (expr->op == "-") return {formatNumbers( std::to_string(leftNum - rightNum)),"double"};
        if (expr->op == "*") return {formatNumbers( std::to_string(leftNum * rightNum)),"double"};
        if (expr->op == "/") return {rightNum != 0 ? formatNumbers(std::to_string(leftNum / rightNum) ): "ERROR","double"};
        if( expr->op ==">") return {leftNum>rightNum?"true" : "false","bool"};
        if( expr->op == ">=") return {leftNum>= rightNum ?"true" :"false","bool"};
        if( expr->op == "<") return {leftNum< rightNum ?"true" :"false","bool"};
        if( expr->op == "<=") return {leftNum<= rightNum ?"true" :"false","bool"};
    }
    if (expr->op == "+") {
        if(LEFT.datatype!="string" || RIGHT.datatype!="string"){
            std::cerr<<"Operands must be numbers. \n[line 1]\n";
            exit(70);
        }
        return {left + right,"string"};  // Concatenation
    }else {
        std::cerr<<"Operands must be numbers. \n[line 1]\n";
        exit(70);
    }
    
   
    // throw std::runtime_error("Invalid binary operation.");
}

Evalstr Evaluator::evaluateUnary(UnaryExpr* expr) {
    Evalstr RIGHT= evaluateExpr(expr->right.get());
    std::string right = RIGHT.value;
    size_t pos= right.find('.');
    if(expr->op.lexeme=="-") {
        if(RIGHT.datatype=="double"){
          if(pos==std::string::npos){
            return {std::to_string(-std::stoi(right)),"double"};
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
                return {"true","string"};
            }
            if (right == "true") {
                return {"false","string"};
            }
            
               
                double num = std::stod(right);
                right = (num == 0) ? "false" : "true";
            

            return {(right == "true") ? "false" : "true","double"};
        }
    

    throw std::runtime_error("Unsupported unary operator.");
}

bool Evaluator::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    char* end;
    std::strtod(str.c_str(), &end);
    
    return *end == '\0'; 
}

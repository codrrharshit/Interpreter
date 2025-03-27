#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include <unordered_map>

struct Evalstr {
    std::string value;
    std::string datatype;
};

class Evaluator {
private:
    bool isEvaluatedMode;
    Evalstr evaluateExpr(Expr* expr);
    Evalstr evaluateBinary(BinaryExpr* expr);
    Evalstr evaluateUnary(UnaryExpr* expr);
    bool isNumber(const std::string& str);
    void evaluatePrint(PrintStmt* stmt);
    void evaluateBlock(BlockStmt *stmt);
    void evaluateExpression(ExpressionStmt * stmt);

public:

    Evaluator(bool isEvaluatedMode=false);
    std::unordered_map<std::string, Evalstr> variables; 
    std::string evaluate(std::unique_ptr<Expr>& expr);
    void evaluateStmt( const std::unique_ptr<Stmt>& stmt);
    void evaluateVariable(VarDeclStmt* stmt);
    void evaluateProgram(const std::unique_ptr<Program>& program);
};

#endif

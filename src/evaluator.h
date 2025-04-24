#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include "environment.h"
#include <unordered_map>
#include <iomanip>

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
    void evaluateIf(IfStmt* stmt);
    void evaluateExpression(ExpressionStmt * stmt);
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;

public:
    Evaluator(bool isEvaluatedMode=false);
    std::string evaluate(std::unique_ptr<Expr>& expr);
    void evaluateStmt(const std::unique_ptr<Stmt>& stmt);
    void evaluateVariable(VarDeclStmt* stmt);
    void evaluateProgram(const std::unique_ptr<Program>& program);
};

#endif

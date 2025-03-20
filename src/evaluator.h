#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

struct Evalstr {
    std::string value;
    std::string datatype;
};

class Evaluator {
private:
    Evalstr evaluateExpr(Expr* expr);
    Evalstr evaluateBinary(BinaryExpr* expr);
    Evalstr evaluateUnary(UnaryExpr* expr);
    bool isNumber(const std::string& str);
    void evaluatePrint(PrintStmt* stmt);
    void evaluateExpression(ExpressionStmt * stmt);

public:
    std::string evaluate(std::unique_ptr<Expr>& expr);
    void evaluateStmt( const std::unique_ptr<Stmt>& stmt);
    void evaluateProgram(const std::unique_ptr<Program>& program);
};

#endif

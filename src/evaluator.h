#ifndef EVALUATOR_H
#define EVALUATOR_H


#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

class Evaluator
{
private:
    std::string evaluateExpr(Expr* expr);
    std::string evaluateBinary(BinaryExpr* expr);
    std::string evaluateUnary(UnaryExpr* expr);
    bool isNumber(const std::string& str);
public:
   std::string evaluate(std::unique_ptr<Expr>&expr);
};




#endif 
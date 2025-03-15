#include "evaluator.h"
#include <stdexcept>
#include <cctype>

std::string Evaluator::evaluate(std::unique_ptr<Expr>& expr) {
    return evaluateExpr(expr.get());
}

std::string Evaluator::evaluateExpr(Expr* expr) {
    if (auto binary = dynamic_cast<BinaryExpr*>(expr)) {
        return evaluateBinary(binary);
    } else if (auto literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (std::holds_alternative<std::string>(literal->value)) {
            return std::get<std::string>(literal->value);  // Extract string correctly
        } else if (std::holds_alternative<double>(literal->value)) {
            return std::to_string(std::get<double>(literal->value));  // Extract number correctly
        }
    } else if (auto unary = dynamic_cast<UnaryExpr*>(expr)) {
        return evaluateUnary(unary);
    }
    throw std::runtime_error("Unknown expression.");
}

std::string Evaluator::evaluateBinary(BinaryExpr* expr) {
    std::string left = evaluateExpr(expr->left.get());
    std::string right = evaluateExpr(expr->right.get());

    if (expr->op == "==") return left == right ? "true" : "false";
    if (expr->op == "!=") return left != right ? "true" : "false";

    if (isNumber(left) && isNumber(right)) {
        double leftNum = std::stod(left);
        double rightNum = std::stod(right);
        if (expr->op == "+") return std::to_string(leftNum + rightNum);
        if (expr->op == "-") return std::to_string(leftNum - rightNum);
        if (expr->op == "*") return std::to_string(leftNum * rightNum);
        if (expr->op == "/") return rightNum != 0 ? std::to_string(leftNum / rightNum) : "ERROR";
    }
    throw std::runtime_error("Invalid binary operation.");
}

std::string Evaluator::evaluateUnary(UnaryExpr* expr) {
    std::string right = evaluateExpr(expr->right.get());

    if (expr->op.lexeme == "-") return std::to_string(-std::stod(right));
    if (expr->op.lexeme == "!") return right == "true" ? "false" : "true";

    throw std::runtime_error("Unsupported unary operator.");
}

bool Evaluator::isNumber(const std::string& str) {
    return !str.empty() && (std::isdigit(str[0]) || str[0] == '-');
}

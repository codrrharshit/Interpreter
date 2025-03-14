#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

// Base class for all AST nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string toString() const = 0;
};

// Literal expressions (e.g., numbers, strings, booleans, nil)
class LiteralExpr : public Expr {
public:
    std::string value;

    explicit LiteralExpr(const std::string& value) : value(value) {}

    std::string toString() const override {
        return value;
    }
};

// Binary expressions (e.g., 2 + 3)
class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    std::string op;

    BinaryExpr(std::unique_ptr<Expr> left, std::string op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::string toString() const override {
        return "(" + op + " " + left->toString() + " " + right->toString() + ")";
    }
};

// Grouping expression 
class GroupingExpr : public Expr {
    public:
        std::unique_ptr<Expr> expression;
    
        explicit GroupingExpr(std::unique_ptr<Expr> expr)
            : expression(std::move(expr)) {}
    
        std::string toString() const override {
            return "(group " + expression->toString() + ")";
        }
    };
    

#endif // AST_H
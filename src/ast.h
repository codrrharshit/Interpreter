#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <iomanip>

// Base class for all AST nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string toString() const = 0;
};

// Literal expressions (e.g., numbers, strings, booleans, nil)
class LiteralExpr : public Expr {
    public:
        std::variant<std::string, double> value;  // Store strings and numbers correctly
    
        explicit LiteralExpr(const std::string& value) : value(value) {}
        explicit LiteralExpr(double value) : value(value) {}
    
        std::string toString() const override {
            if (std::holds_alternative<double>(value)) {
                std::ostringstream out;
                out << std::fixed << std::setprecision(6) << std::get<double>(value);
                std::string str = out.str();
                
                // Remove trailing zeros
                str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                if (str.back() == '.') str.pop_back(); // Remove trailing decimal if needed
                return str;
            }
            return std::get<std::string>(value);
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
    

class UnaryExpr : public Expr {
        public:
            Token op;
            std::unique_ptr<Expr> right;
        
            UnaryExpr(Token op, std::unique_ptr<Expr> expr)
                : op(op), right(std::move(expr)) {}
        
            std::string toString() const override {
                return "(" + op.lexeme + " " + right->toString() + ")";
            }
        };
        

#endif // AST_H
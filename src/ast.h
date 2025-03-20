#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <iomanip>
#include "tokeniser.h"
#include <fstream>



class NilValue {
    public:
        bool operator==(const NilValue&) const { return true; }  // All NilValues are equal
        bool operator!=(const NilValue&) const { return false; } // Nil is never different from another Nil
    
        friend std::ostream& operator<<(std::ostream& os, const NilValue&) {
            return os << "nil";
        }
    };
// Base class for all AST nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string toString() const = 0;
};

// Literal expressions (e.g., numbers, strings, booleans, nil)
class LiteralExpr : public Expr {
    public:
        std::variant<std::string,bool,double,NilValue > value;  // Store strings and numbers correctly
    
        explicit LiteralExpr(const std::string& value) : value(value) {
        }
        explicit LiteralExpr(bool value) : value(value){}
        explicit LiteralExpr(double value) : value(value) {
        }
        explicit LiteralExpr(NilValue value):value(value) {}
        std::string toString() const override {
            if (std::holds_alternative<double>(value)) {
                double num = std::get<double>(value);
                if (num == static_cast<int>(num)) {
                    return std::to_string(static_cast<int>(num))+".0"; // Return as integer string
                }
                std::ostringstream out;
                out << std::fixed << std::setprecision(3) << num; // Format with 3 decimal places
                std::string str = out.str();
                str.erase(str.find_last_not_of('0') + 1, std::string::npos); // Remove trailing zeros
                if (!str.empty() && str.back() == '.') str.pop_back(); // Remove trailing decimal
                return str;
            } else if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value); // Return string as-is
            }
            else if(std::holds_alternative<bool>(value)){
                return std::get<bool>(value)==true ?"true":"false";
            }
            else if (std::holds_alternative<NilValue>(value)){
                return "nil";
            }
            return "Unknown";
         } // Fallback for unexpected cases
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

 class VariableExpr : public Expr {
    public:
        std::string name;
            
        explicit VariableExpr(std::string name) : name(std::move(name)) {}
            
        std::string toString() const override {
            return name;
        }
 };

 class AssignExpr : public Expr {
    public:
        std::string name;
        std::unique_ptr<Expr> value;
    
        AssignExpr(std::string name, std::unique_ptr<Expr> value)
            : name(std::move(name)), value(std::move(value)) {}
    
        std::string toString() const override {
            return "(" + name + " = " + value->toString() + ")";
        }
    };
struct Stmt{
    virtual ~Stmt()= default;
    virtual std::string toString() const = 0;
};

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expression;
    PrintStmt(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}
    std::string toString() const override {
        return "(print " + expression->toString() + ")";
    }
};

class ExpressionStmt : public Stmt {
    public:
        std::unique_ptr<Expr> expression;
    
        ExpressionStmt(std::unique_ptr<Expr> expr) 
            : expression(std::move(expr)) {}
    
        std::string toString() const override {
            return  expression->toString() ;
         }
    };

    class VarDeclStmt : public Stmt {
        public:
            std::string name;
            std::unique_ptr<Expr> initializer;
        
            VarDeclStmt(std::string name, std::unique_ptr<Expr> initializer)
                : name(std::move(name)), initializer(std::move(initializer)) {}
        
            std::string toString() const override {
                return "(var " + name + " = " + (initializer ? initializer->toString() : "nil") + ")";
            }
        };
        
        // **Variable assignment (e.g., `x = 42;`)**
        class AssignStmt : public Stmt {
        public:
            std::string name;
            std::unique_ptr<Expr> value;
        
            AssignStmt(std::string name, std::unique_ptr<Expr> value)
                : name(std::move(name)), value(std::move(value)) {}
        
            std::string toString() const override {
                return "(" + name + " = " + value->toString() + ")";
            }
        };

struct Program {
    std::vector<std::unique_ptr<Stmt>> statements;

    std::string toString() const {
        std::ostringstream out;
        for (const auto& stmt : statements) {
            out << stmt->toString() << "\n";  // Assuming `Stmt` has `toString()`
        }
        return out.str();
    }
};



#endif // AST_H
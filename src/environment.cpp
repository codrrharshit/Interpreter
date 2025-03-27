#include "environment.h"
#include <stdexcept>

Environment::Environment(std::shared_ptr<Environment> parent)
    : enclosing(parent) {}

void Environment::define(const std::string& name, const std::string& value) {
    values[name] = value;
}

std::string Environment::get(const std::string& name, int line) {
    auto it = values.find(name);
    if (it != values.end()) {
        return it->second;
    }
    
    if (enclosing) {
        return enclosing->get(name, line);
    }
    
    throw std::runtime_error("Undefined variable '" + name + 
                           (line != -1 ? "' at line " + std::to_string(line) : "'"));
}

void Environment::assign(const std::string& name, const std::string& value, int line) {
    auto it = values.find(name);
    if (it != values.end()) {
        values[name] = value;
        return;
    }
    
    if (enclosing) {
        enclosing->assign(name, value, line);
        return;
    }
    
    throw std::runtime_error("Undefined variable '" + name + 
                           (line != -1 ? "' at line " + std::to_string(line) : "'"));
}
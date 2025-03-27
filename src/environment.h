#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

class Environment {
public:
    std::shared_ptr<Environment> enclosing;
    
    Environment(std::shared_ptr<Environment> parent = nullptr);

    void define(const std::string& name, const std::string& value);
    std::string get(const std::string& name, int line = -1);  // Added line parameter
    void assign(const std::string& name, const std::string& value, int line = -1);  // Added line parameter

private:
    std::unordered_map<std::string, std::string> values;
};

#endif
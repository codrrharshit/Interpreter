#include "runner.h"

void Runner::run(const std::unique_ptr<Program>& program) {
    Evaluator evaluator;
    evaluator.evaluateProgram(program);
}


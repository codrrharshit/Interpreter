#ifndef RUNNER_H
#define RUNNER_H

#include "evaluator.h"
#include <vector>
#include <memory>

class Runner {
public:
    void run(const std::unique_ptr<Program>& program );
};

#endif // RUNNER_H
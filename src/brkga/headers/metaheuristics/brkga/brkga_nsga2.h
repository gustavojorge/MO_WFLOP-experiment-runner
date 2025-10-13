#ifndef BRKGA_NSGA2_H
#define BRKGA_NSGA2_H

#include <vector>
#include "../../headers/global_modules/generate_initial_population/generate_rSolution.h"


std::vector<Solution> brkga_nsga2(std::vector<Solution>& population);

extern ofstream infoRunBrkga;

#endif // BRKGA_NSGA2_H

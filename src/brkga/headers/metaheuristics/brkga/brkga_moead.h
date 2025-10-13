

#ifndef BRKGA_MOEAD_H
#define BRKGA_MOEAD_H

#include <vector>
#include "../../headers/global_modules/generate_initial_population/generate_rSolution.h"



std::vector<Solution> elitism_moead(std::vector<Solution>& population, int elite_size);

std::vector<Solution> brkga_moead(std::vector<Solution>& population);

extern ofstream infoRunBrkga;

#endif //BRKGA_MOEAD_H

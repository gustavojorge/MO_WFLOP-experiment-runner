#ifndef MOEAD_PLS_H
#define MOEAD_PLS_H

#include <vector>
#include <utility>
#include "../../global_modules/generate_initial_population/generate_rSolution.h"

void moead_pls(std::vector<Solution>& population);

extern ofstream infoRun;

#endif 
#ifndef MOEAD_HYBRID_H
#define MOEAD_HYBRID_H

#include <vector>
#include <utility>
#include "../../global_modules/generate_initial_population/generate_rSolution.h"

void moead_hybrid(std::vector<Solution>& population, 
    function<vector<Solution*>*(vector<Solution*>)>);

extern ofstream infoRun;

#endif 
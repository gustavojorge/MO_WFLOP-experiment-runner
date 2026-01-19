#include "../../headers/global_modules/generate_initial_population/generate_rSolution.h"
#include "../../headers/global_modules/dominates.h"

bool dominates(const Solution& sol1, const Solution& sol2) {
    return (sol1.fitness.first < sol2.fitness.first && sol1.fitness.second > sol2.fitness.second);
}
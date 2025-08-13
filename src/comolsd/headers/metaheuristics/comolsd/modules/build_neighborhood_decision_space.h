#ifndef BUILD_NEIGHBORHOOD_DECISION_SPACE_H
#define BUILD_NEIGHBORHOOD_DECISION_SPACE_H

#include <vector>
#include "../../../global_modules/generate_initial_population/generate_rSolution.h"

std::vector<Solution> build_neighborhood_decision_space(const Solution& parent, int number_of_neighbors);

#endif

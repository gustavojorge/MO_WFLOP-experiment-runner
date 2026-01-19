#ifndef DECODEALTERNATIVE_BRKGA_H
#define DECODEALTERNATIVE_BRKGA_H

#include <vector>
#include "../../../headers/wind_farm_entities/turbine.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"

Solution decodeAlternative_brkga(const std::vector<double>& chromosome);

#endif
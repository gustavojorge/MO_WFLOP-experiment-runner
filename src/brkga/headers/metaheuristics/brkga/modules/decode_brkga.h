#ifndef DECODE_BRKGA_H
#define DECODE_BRKGA_H

#include <vector>
#include "../../../headers/wind_farm_entities/turbine.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"

Solution decode_brkga(const std::vector<double>& chromosome);

#endif





#ifndef CROSSOVER_BRKGA_H
#define CROSSOVER_BRKGA_H

#include <vector>
#include "../generate_initial_population/population.h"

std::vector<double> crossoverAux_brkga(const Solution &parent1, const Solution &parent2, double bias);

Solution crossover_brkga(const Solution& parent1, const Solution& parent2, double bias);

#endif // CROSSOVER_BRKGA_H
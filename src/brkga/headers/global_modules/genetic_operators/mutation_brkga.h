#ifndef MUTATION_BRKGA_H
#define MUTATION_BRKGA_H

#include <vector>
#include "../generate_initial_population/population.h"


void mutationAux_brkga(std::vector<double>& chromosome, double mutation_rate);


Solution mutation_brkga(Solution& offspring, double mutation_rate);

#endif // MUTATION_BRKGA_H
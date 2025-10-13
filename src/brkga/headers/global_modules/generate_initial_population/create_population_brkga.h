
#ifndef CREATE_POPULATION_BRKGA_H
#define CREATE_POPULATION_BRKGA_H

#include <vector>
#include "../../headers/global_modules/generate_initial_population/generate_rSolution.h"


using namespace std;

extern int chromosome_size;

vector<Solution> create_initial_population_brkga(int size_population);


#endif //CREATE_POPULATION_BRKGA_H

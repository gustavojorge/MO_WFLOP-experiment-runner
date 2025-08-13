#include "../../../../headers/metaheuristics/comolsd/modules/make_population_pointers.h"

using namespace std;

vector<Solution*> make_population_pointers(vector<Solution>& population) {
    vector<Solution*> pointer_population;
    pointer_population.reserve(population.size());

    for (Solution& sol : population) {
        pointer_population.push_back(&sol);
    }

    return pointer_population;
}

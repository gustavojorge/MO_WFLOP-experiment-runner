#ifndef NSGA2_PLS_H
#define NSGA2_PLS_H

#include "../../global_modules/generate_initial_population/generate_rSolution.h"
#include "./nsga2.h"
#include <vector>
using namespace std;

vector<Solution*> nsga2_pls(vector<Solution>& pop);

extern ofstream infoRunNSGA2;

#endif 
#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "../genetic_operators/mutation.h"
#include "../isEqual.h"

vector<Solution*> getNeighborhood(Solution *s, int num_neighbors);

#endif
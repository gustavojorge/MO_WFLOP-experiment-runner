#include "../../headers/global_modules/local_search/neighborhood.h"

bool inNeighborhood(Solution * s, vector<Solution*> neighborhood){
    for(int j = 0; j < neighborhood.size(); j++){
            if(isEqual(*s, *neighborhood[j])){
                return true;
            }
        }
}

vector<Solution*> getNeighborhood(Solution *s, int num_neighbors){
    vector<Solution *> neighbors;
    neighbors.push_back(s);
    Solution newSol;
    for(int i = 0; i < num_neighbors + 1; i++){
        newSol = *s;

        while(inNeighborhood(&newSol, neighbors)){
            mutation(newSol);
        }

        neighbors.push_back(&newSol);
    }

    neighbors.erase(neighbors.begin());

    return neighbors;
}
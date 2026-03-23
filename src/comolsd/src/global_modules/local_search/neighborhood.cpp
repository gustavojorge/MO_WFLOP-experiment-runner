#include "../../headers/global_modules/local_search/neighborhood.h"

bool inNeighborhood(Solution * s, vector<Solution*> & neighborhood){
    for(int j = 0; j < neighborhood.size(); j++){
        if(isEqual(*s, *neighborhood[j])){
            return true;
        }
    }

    return false;
}

vector<Solution*> getNeighborhood(Solution *s, int num_neighbors){
    vector<Solution *> neighbors;

    Solution sol;

    for(int i = 0; i < num_neighbors + 1; i++){
        while(inNeighborhood(&sol, neighbors)){
            sol = *s;
            mutation(sol);
        }
        neighbors.push_back(new Solution(sol));
    }

    // delete *neighbors.begin();
    neighbors.erase(neighbors.begin());

    return neighbors;
}
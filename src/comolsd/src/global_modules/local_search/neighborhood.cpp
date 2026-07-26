#include "../../headers/global_modules/local_search/neighborhood.h"

bool inNeighborhood(Solution s, vector<Solution*> & neighborhood){
    for(int j = 0; j < neighborhood.size(); j++){
        if(isEqual(s, *neighborhood[j])){
            return true;
        }
    }

    return false;
}

vector<Solution*> getNeighborhood(Solution *s, int num_neighbors){
    vector<Solution *> neighbors;
    
    Solution sol = *s;

    neighbors.push_back(new Solution(sol));

    for(int i = 0; i < num_neighbors; i++){
        sol = *s;
        mutation(sol);

        bool result = inNeighborhood(sol, neighbors);
        if(result){
            while(result){
                sol = *s;
                mutation(sol);

                result = inNeighborhood(sol, neighbors);
            }
        }
        neighbors.push_back(new Solution(sol));
    }

    Solution * toDelete = *neighbors.begin();

    neighbors.erase(neighbors.begin());
    delete toDelete;

    return neighbors;
}
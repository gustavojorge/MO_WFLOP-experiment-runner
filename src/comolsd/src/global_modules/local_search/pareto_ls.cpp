#include "../../headers/global_modules/local_search/pareto_ls.h"
#include "../../headers/instance_info.h"

Solution * pareto_ls(Solution* s){
    ParetoSetLS* p = new ParetoSetLS();

    p->adicionarSol(s);

    list<pair<Solution *, bool>>::iterator it = p->getBegin();
    while(!p->allExplored()){
        it = p->getBegin();
        for(it = p->getBegin(); it != p->getEnd(); it++){
            // cout << &it << " " << p->getSize() << endl;

            if(!(it->second)){
                vector<Solution *> neighborhood = getNeighborhood(it->first, 200);
                for(int i = 0; i < neighborhood.size(); i++){
                    p->adicionarSol(neighborhood[i]);
                }
                it->second = true; //It is possible that *it is not the same at the end of neighborhood insertion

                for(auto p: neighborhood){
                    delete p;
                }
                neighborhood.clear();
                break;
            }
        }
    }

    int randInt = rand() % p->getSize();
    return p->getSolucao(randInt).first;
}

int main(int argc, char* argv[]){
    get_instance_info(argc, argv);

    Solution s = generate_solution();
    Solution * sol = new Solution(s);

    cout << sol->fitness.first << " " << sol->fitness.second << endl;

    sol = pareto_ls(sol);
    
    cout << sol->fitness.first << " " << sol->fitness.second << endl;
}
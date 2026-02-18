#include "../../headers/global_modules/local_search/pareto_ls.h"

void pareto_ls(Solution* s){
    ParetoSet* p = new ParetoSet();
    vector<bool> checked;
    bool remain = true;

    p->adicionarSol(s);
    checked.push_back(false);

    while(remain){
        for(int i = 0; i < checked.size(); i++){
            if(!checked[i]){
                Solution* sol2Check = p->getSolucao(i);
                vector<Solution *> neighbors = getNeighborhood(sol2Check, 100);
                for (Solution * sol : neighbors){
                    
                }
            }
        }
    }
}
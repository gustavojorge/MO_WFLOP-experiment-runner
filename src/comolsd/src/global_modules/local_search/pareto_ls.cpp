#include "../../headers/global_modules/local_search/pareto_ls.h"
#include "../../headers/instance_info.h"

vector<Solution *> pareto_ls(vector<Solution*> population){
    ParetoSetLS* p = new ParetoSetLS();

    for(int i = 0; i < population.size(); i++){
        p->adicionarSol(population[i]);
    }

    list<pair<Solution *, bool>>::iterator it = p->getBegin();
    while(!p->allExplored()){
        it = p->getRandomUnex();

        vector<Solution *> neighborhood = getNeighborhood(it->first, 10);

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

    vector<Solution *> result;

    for(it = p->getBegin(); it != p->getEnd(); it++){
        result.push_back((it)->first);
    }

    return result;
}

int main(int argc, char* argv[]){
    get_instance_info(argc, argv);

    auto initpop = create_initial_population(100);

    vector<Solution*> pop;

    for(int i = 0; i < initpop.size(); i++){
        pop.push_back(new Solution(initpop[i]));
    }

    pop = pareto_ls(pop);
    
    cout << "=========================ARCHIVE=========================";
    for(int i = 0; i < pop.size(); i++){
        cout << pop[i]->fitness.first << " " << pop[i]->fitness.second << endl;
    }
}
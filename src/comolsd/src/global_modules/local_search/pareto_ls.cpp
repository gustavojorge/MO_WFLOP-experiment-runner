#include "../../headers/global_modules/local_search/pareto_ls.h"
#include "../../headers/instance_info.h"

vector<Solution *> * pareto_ls(vector<Solution*> population){
    ParetoSetLS* p = new ParetoSetLS();

    for(int i = 0; i < population.size(); i++){
        p->adicionarSol(population[i]);
    }

    // cout << "=========================ARCHIVE=========================" << endl;
    // for(auto a = p->getBegin(); a != p->getEnd(); a++){
    //     cout << a->first->fitness.first << " " << a->first->fitness.second << endl;
    // }

    pair<Solution *, bool> * it;

    while(!(p->allExplored())){
        it = p->getRandomUnex();

        vector<Solution *> neighborhood = getNeighborhood(it->first, 100);

        for(int i = 0; i < neighborhood.size(); i++){
            p->adicionarSol(neighborhood[i]);
        }
        it->second = true; //It is possible that *it is not the same at the end of neighborhood insertion

        for(auto p: neighborhood){
            delete p;
        }
        neighborhood.clear();
    }

    vector<Solution *> * result;

    for(auto i = p->getBegin(); i != p->getEnd(); i++){
        result->push_back((i)->first);
    }

    return result;
}

// int main(int argc, char* argv[]){
//     get_instance_info(argc, argv);

//     auto initpop = create_initial_population(100);

//     vector<Solution*> pop;

//     for(int i = 0; i < initpop.size(); i++){
//         Solution * s = new Solution;
//         *s = initpop[i];
//         pop.push_back(s);
//     }

//     pop = pareto_ls(pop);
    
// }
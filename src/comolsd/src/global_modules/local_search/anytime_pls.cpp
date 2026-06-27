#include "../../headers/global_modules/local_search/pareto_ls.h"
#include "../../headers/instance_info.h"

vector<Solution *> * anytime_pls(vector<Solution*> population){
    ParetoSetLS* p = new ParetoSetLS();

    int neighborhood_size = 163;

    for(int i = 0; i < population.size(); i++){
        p->adicionarSol(population[i]);
    }

    // cout << "=========================ARCHIVE=========================" << endl;
    // for(auto a = p->getBegin(); a != p->getEnd(); a++){
        //     cout << a->first->fitness.first << " " << a->first->fitness.second << endl;
        // }
        
    Solution * selected;

    // Stop on first solution "improved"
    while(!(p->allExplored()) && countRevalue < stop_criteria){
        selected = p->getNext();

        vector<Solution *> neighborhood = getNeighborhood(selected, neighborhood_size);

        bool added = false;

        for(int i = 0; i < neighborhood.size(); i++){
            if(dominatesP(*neighborhood[i], *selected)){
                p->adicionarSol(neighborhood[i]);
                added = true;
                break;
            }
        }
        
        if(!added){
            for(int i = 0; i < neighborhood.size(); i++){
                if(p->adicionarSol(neighborhood[i]));{
                    break;
                }
            }
        }

        p->markExplored(selected);

        for(auto p: neighborhood){
            delete p;
        }
        
        neighborhood.clear();
        countRevalue -= (neighborhood_size - 1);
    }

    p->unexploreAll();


    // Stop on neighborhood fully explored
    while(!(p->allExplored()) && countRevalue < stop_criteria){
        selected = p->getNext();

        vector<Solution *> neighborhood = getNeighborhood(selected, neighborhood_size);

        bool added = false;

        for(int i = 0; i < neighborhood.size(); i++){
            if(dominatesP(*neighborhood[i], *selected)){
                p->adicionarSol(neighborhood[i]);
                added = true;
            }
        }
        
        if(!added){
            for(int i = 0; i < neighborhood.size(); i++){
                p->adicionarSol(neighborhood[i]);
            }
        }

        p->markExplored(selected);

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
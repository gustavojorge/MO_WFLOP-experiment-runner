#include "../../headers/global_modules/local_search/local_search_type.h"
#include "../../headers/global_modules/local_search/pareto_ls.h"
#include "../../headers/global_modules/local_search/anytime_pls.h"
#include <stdexcept>

using namespace std;

LocalSearchType parseLocalSearchType(const string& typeStr){
    if(typeStr == "pls") return LocalSearchType::ParetoLS;
    if(typeStr == "apls") return LocalSearchType::AnytimePLS;
    if(typeStr == "none") return LocalSearchType::None;
    throw invalid_argument("Unknown local search type: " + typeStr);
}

function<vector<Solution*>*(vector<Solution*>)> getLocalSearchFunction(LocalSearchType type){
    switch(type){
        case LocalSearchType::ParetoLS:
            return pareto_ls;
        case LocalSearchType::AnytimePLS:
            return anytime_pls;
        case LocalSearchType::None:
        default:
            return [](vector<Solution*> pop) -> vector<Solution*>* {
                return new vector<Solution*>(pop);
            };
    }
}
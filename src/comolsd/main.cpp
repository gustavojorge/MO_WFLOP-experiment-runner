#include "./headers/main.h"
#include "./headers/globals.h"
#include "./headers/global_modules/local_search/local_search_type.h"

#include <iostream>
#include <string>
using namespace std;

int countRevalue = 0;

BoundedParetoSet * pareto = new BoundedParetoSet();
int stop_criteria = 1000000;
string algorithm = "nsga2_hybrid";
string local_search_type = "apls";
string instance = "A";
string root_folder = "./";

int main(int argc, char* argv[]){

    if(argc == 2){
        instance = argv[1];
    } else if (argc > 2){
        instance = argv[1];
        root_folder = argv[2];
    }
    if(argc > 5){
        algorithm = argv[5];
    }
    if(argc > 6){
        local_search_type = argv[6];
    }

    string path;

    int num_neighbors = 10;

    get_instance_info(argc, argv);

    int sum = 0;
    for(auto elem : turbines_per_zone)
        sum += elem;

    cout << endl;
    cout << "Number of subproblems: " << SIZE_OF_POPULATION << endl;
    cout << "Number of neighbors: " << num_neighbors << endl;
    cout << "Number of fixed turbines: " << fixd.size() << endl;
    cout << "Number of mobile turbines: " << sum << endl;
    cout << "Wind: " << wind << endl;
    cout << "Angle: " << angle << endl;

    cout << "Run time:" << endl;
    
    auto population = create_initial_population(SIZE_OF_POPULATION);

    LocalSearchType lsType = parseLocalSearchType(local_search_type);
    auto lsFunction = getLocalSearchFunction(lsType);

    if (algorithm == "nsga2_hybrid"){
        nsga2_hybrid(population, lsFunction);
    } else if (algorithm == "moead_hybrid"){
        moead_hybrid(population, lsFunction);
    } else {
        cerr << "Unknown algorithm: " << algorithm << endl;
        return 1;
    }
}

#include <utility>
#include <vector>
#include <iostream>
#include <ctime> 
#include <algorithm>
#include <numeric>
#include <random>


#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"
#include "../../../headers/metaheuristics/brkga/modules/decode_brkga.h"

using namespace std;



// Gera a população inicial de cromossomos e decodifica para soluções
vector<Solution> create_initial_population_brkga(int size_population) {
    vector<Solution> population;

    int chromosome_size = 0;
    for (int z = 0; z < num_zones; z++) {
        chromosome_size += foundations[z].size();
    }

    //  Gera a população
    for (int i = 0; i < size_population; i++) {
        // Gera um cromossomo aleatório
        vector<double> chromosome(chromosome_size);
        for (int j = 0; j < chromosome_size; j++) {
            chromosome[j] = (double) rand() / RAND_MAX; // número em [0,1]
        }

        // Decodifica o cromossomo para uma solução
        Solution sol = decode_brkga(chromosome);
        Solution* sol_ptr = new Solution(sol);
        pareto->adicionarSol(sol_ptr);

        countRevalue++;

        if(countRevalue % 100000 == 0){
            string path = instance + "_" + algorithm + "_" + to_string(countRevalue) + ".txt";

            pareto->printAllSolutions(root_folder + path);

            if(countRevalue >= stop_criteria){
                pareto->printAllSolutionsLayout(root_folder + instance + "_" + algorithm + "_layout.txt");
            }
        }

        delete sol_ptr;

        // Avalia fitness
        sol.fitness.first = calculate_cost(sol);
        sol.fitness.second = calculate_power(sol);

        population.push_back(sol);
    }

    return population;
}

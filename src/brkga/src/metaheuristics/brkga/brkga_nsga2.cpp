#include <utility>
#include <vector>
#include <iostream>
#include <ctime> 
#include <algorithm>
#include <numeric>
#include <random>
#include <fstream>

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"

#include "../../../headers/metaheuristics/brkga/modules/decode_brkga.h"
#include "../../../headers/metaheuristics/brkga/modules/update_population_brkga.h"

#include "../../../headers/metaheuristics/brkga/brkga_moead.h"

#include "../../../headers/metaheuristics/nsga2/modules/non_dominated_sorting.h"
#include "../../../headers/metaheuristics/nsga2/modules/binary_tournament.h"
#include "../../../headers/metaheuristics/nsga2/modules/crowding_distance.h"
#include "../../../headers/metaheuristics/nsga2/modules/arena_non_dominated_sorting.h"
#include "../../../headers/metaheuristics/nsga2/nsga2.h"

#include "../../../headers/global_modules/genetic_operators/mutation_brkga.h"
#include "../../../headers/global_modules/genetic_operators/crossover_brkga.h"

#include "../../../headers/global_modules/dominates.h"
#include "../../../headers/global_modules/isEqual.h"

using namespace std;

// Função auxiliar para evitar adicionar soluções duplicadas a uma lista temporária
void add_if_not_duplicate(vector<Solution*>& population, Solution* solution){
    for (auto& existing_solution : population) {
        if (isEqual(*existing_solution, *solution)) {
            delete solution;
            return;          
        }
    }
    population.push_back(solution);
}


vector<Solution* > brkga_nsga2(vector<Solution>& initial_population) {

    // Inicializa população (vetor de ponteiros)
    vector<Solution*> * population = new vector<Solution*>();

    for(auto& sol : initial_population){
        Solution * s = new Solution(sol);
        add_if_not_duplicate(*population, s);
    }

    // Inicialização RNG
    default_random_engine re{(unsigned)time(nullptr)};
    uniform_real_distribution<double> dist(0.0, 1.0);

    // Parâmetros BRKGA
    int size_population = (int)population->size();
    double cross_prob = 0.6;
    double mutation_prob = 0.5;
    double bias = 0.5;
    int stop_criteria = 1000000;

    // chromosome_size (verificação)
    int chromosome_size = 0;
    for (int z = 0; z < num_zones; z++) chromosome_size += foundations[z].size();

    int generation = 0;
    ofstream infoRunBrkga(root_folder + "infoRun.txt");

    // Loop principal
    while (countRevalue < stop_criteria) {

        infoRunBrkga << "Generation " << generation
                     << " | Evaluations: " << countRevalue
                     << " | Gridsize (Pareto): " << pareto->getSize()
                     << endl;

        // offspring temporário
        vector<Solution*> * offspring_population = new vector<Solution*>();

        for(int i = 0; i < size_population; i++){
            // Seleção de pais
            Solution** parents = binary_tournament(*population);


            // Cria cópias dos pais para operar
            Solution child1 = *parents[0];
            Solution child2 = *parents[1];

            // Crossover (retorna Solution)
            if (dist(re) < cross_prob) {
                child1 = crossover_brkga(*parents[0], *parents[1], bias);
                child2 = crossover_brkga(*parents[1], *parents[0], bias);
            } else {
                // Seleciona (aleatoriamente) dois indivíduos da população
                int r1 = rand() % size_population;
                int r2 = rand() % size_population;
                child1 = *(*population)[r1];
                child2 = *(*population)[r2];
            }

            // Mutation (agora retorna Solution)
            if (dist(re) < mutation_prob) {
                child1 = mutation_brkga(child1, mutation_prob);
            }
            if (dist(re) < mutation_prob) {
                child2 = mutation_brkga(child2, mutation_prob);
            }

            // Adiciona à população offspring (aloca novos objetos)
            add_if_not_duplicate(*offspring_population, new Solution(child1));
            add_if_not_duplicate(*offspring_population, new Solution(child2));

        }

        // Merge: population U offspring -> total_population
        vector<Solution*> * total_population = new vector<Solution*>();
        for (auto i : *population) total_population->push_back(new Solution(*i));
        for (auto i : *offspring_population) {
            total_population->push_back(new Solution(*i));
            delete i;
        }
        delete offspring_population;

        // Non-dominated sorting (arena)
        vector<vector<Solution*> *> * fronts = new vector<vector<Solution*>*>();
        *fronts = arena_non_dominated_sorting(*total_population);

        // limpa total_population
        for (auto i : *total_population) delete i;
        delete total_population;

        // limpa population antiga
        for (auto i : *population) delete i;
        population->clear();

        // Reconstrói população a partir dos fronts (NSGA-II procedure)
        for (auto& front : *fronts) {
            if ((int)population->size() + (int)front->size() <= size_population) {
                for (auto solution : *front) population->push_back(new Solution(*solution));
            } else {
                auto * front_sorted = new vector<Solution*>();
                *front_sorted = crowding_distance(*front);

                int remaining_spots = size_population - (int)population->size();
                for (int j = 0; j < remaining_spots && j < (int)front_sorted->size(); ++j) {
                    population->push_back(new Solution(*(*front_sorted)[j]));
                }

                delete front_sorted;
                break;
            }
        }

        // libera memória dos fronts
        for (auto front : *fronts) {
            for (auto solution : *front) delete solution;
            delete front;
        }
        delete fronts;

        generation++;
    }

    infoRunBrkga.close();

    // Retorna vetor de Solutions (cópia para fora do heap)
    vector<Solution*> result = *population;

    return *population;
}


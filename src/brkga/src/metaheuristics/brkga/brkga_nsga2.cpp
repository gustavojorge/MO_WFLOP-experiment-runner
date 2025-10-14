#include <utility>
#include <vector>
#include <iostream>
#include <ctime> 
#include <algorithm>
#include <numeric>
#include <random>
#include <fstream> // Adicionado para ofstream

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"

#include "../../../headers/metaheuristics/brkga/modules/decode_brkga.h"
#include "../../../headers/metaheuristics/brkga/modules/update_population_brkga.h"

#include "../../../headers/metaheuristics/brkga/brkga_moead.h"

#include "../../../headers/metaheuristics/nsga2/modules/non_dominated_sorting.h"
#include "../../../headers/metaheuristics/nsga2/modules/crowding_distance.h"
#include "../../../headers/metaheuristics/nsga2/nsga2.h"

#include "../../../headers/global_modules/genetic_operators/mutation.h"
#include "../../../headers/global_modules/genetic_operators/crossover.h"

#include "../../../headers/global_modules/dominates.h"
#include "../../../headers/global_modules/isEqual.h"

using namespace std;

// Função auxiliar para evitar adicionar soluções duplicadas a uma lista temporária
void add_if_not_duplicate(vector<Solution>& target_vector, const Solution& new_solution) {
    for (const auto& sol : target_vector) {
        if (isEqual(sol, new_solution)) {
            return;
        }
    }
    target_vector.push_back(new_solution);
}


vector<Solution> brkga_nsga2(vector<Solution>& initial_population) {

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    // --- Parâmetros do BRKGA ---
     int size_population = initial_population.size();
     double elite_fraction = 0.3;
     double mutant_fraction = 0.2;
     double bias = 0.7;
     int stop_criteria = 1000000;

     int num_elite = static_cast<int>(elite_fraction * size_population);
     int num_mutants = static_cast<int>(mutant_fraction * size_population);
     int num_offspring = size_population - num_elite - num_mutants;

    vector<Solution> population = initial_population;

    // Calculado diretamente a partir das fontes de dados, sem depender da população inicial.
    int chromosome_size = 0;
    for (int z = 0; z < num_zones; z++) {
        chromosome_size += foundations[z].size();
    }
    
    int generation = 0;
    ofstream infoRunBrkga(root_folder + "infoRun.txt");

    while (countRevalue < stop_criteria) {
        
        infoRunBrkga << "Generation " << generation << " | Revalues: " << countRevalue << " | Gridsize: " << pareto->getSize() << endl;

        // --- 1. SELEÇÃO DE PAIS (ELITE E NÃO-ELITE) ---
        vector<Solution*> population_ptr;
        for (auto& sol : population) {
            population_ptr.push_back(&sol);
        }

        auto fronts = non_dominated_sorting(population_ptr);
        
        vector<Solution> elite_parents;
        vector<Solution> non_elite_parents;

        for (auto& front_ptr : fronts) {
            if (elite_parents.size() >= num_elite) break;
            
            vector<Solution*>& front = *front_ptr;
            if (elite_parents.size() + front.size() <= num_elite) {
                for(auto sol_ptr : front) elite_parents.push_back(*sol_ptr);
            } else {
                auto sorted_front = crowding_distance(front);
                int remaining = num_elite - elite_parents.size();
                for (int i = 0; i < remaining && i < sorted_front.size(); ++i) {
                    elite_parents.push_back(*sorted_front[i]);
                }
            }
        }
        
        for(auto& sol : population){
            bool is_elite = false;
            for(auto& elite_sol : elite_parents){
                if(isEqual(sol, elite_sol)){
                    is_elite = true;
                    break;
                }
            }
            if(!is_elite){
                non_elite_parents.push_back(sol);
            }
        }

        // Garante que temos pais não-elite para o crossover, caso a população inteira seja elite
        if (non_elite_parents.empty()) {
            non_elite_parents = population;
        }

        // --- 2. GERAÇÃO DA NOVA POPULAÇÃO (OFFSPRING) ---
        vector<Solution> offspring_population;

        // 2.1. Crossover
        uniform_int_distribution<> dis_elite(0, elite_parents.size() - 1);
        uniform_int_distribution<> dis_nonelite(0, non_elite_parents.size() - 1);

        for (int i = 0; i < num_offspring; ++i) {
             Solution& parent_e = elite_parents[dis_elite(gen)];
             Solution& parent_n = non_elite_parents[dis_nonelite(gen)];

            vector<double> child_chromo(chromosome_size);
            for (int j = 0; j < chromosome_size; ++j) {
                child_chromo[j] = (dis(gen) < bias) ? parent_e.chromosome[j] : parent_n.chromosome[j];
            }

            Solution offspring_sol = decode_brkga(child_chromo);
            
            Solution* offspring_ptr = new Solution(offspring_sol);
            pareto->adicionarSol(offspring_ptr);
            delete offspring_ptr;
            countRevalue++;

            add_if_not_duplicate(offspring_population, offspring_sol);
        }

        // 2.2. Mutantes
        for (int i = 0; i < num_mutants; ++i) {
            vector<double> chromo(chromosome_size);
            for (int j = 0; j < chromosome_size; ++j) {
                chromo[j] = dis(gen);
            }
            
            Solution mutant_sol = decode_brkga(chromo);
            
            Solution* mutant_ptr = new Solution(mutant_sol);
            pareto->adicionarSol(mutant_ptr);
            delete mutant_ptr;
            countRevalue++;
            
            add_if_not_duplicate(offspring_population, mutant_sol);
        }
        
        // 2.3 A elite é copiada diretamente para a próxima população
        for(auto& elite_sol : elite_parents){
            add_if_not_duplicate(offspring_population, elite_sol);
        }


        // --- 3. SELEÇÃO DA PRÓXIMA GERAÇÃO ---
        vector<Solution*> combined_pop_ptr;
        for (auto& sol : offspring_population) {
            combined_pop_ptr.push_back(&sol);
        }

        auto new_fronts = non_dominated_sorting(combined_pop_ptr);
        
        population.clear();
        for (auto& front_ptr : new_fronts) {
            if (population.size() >= size_population) break;

            vector<Solution*>& front = *front_ptr;
            if (population.size() + front.size() <= size_population) {
                for(auto sol_ptr : front) population.push_back(*sol_ptr);
            } else {
                auto sorted_front = crowding_distance(front);
                int remaining = size_population - population.size();
                for (int i = 0; i < remaining && i < sorted_front.size(); ++i) {
                    population.push_back(*sorted_front[i]);
                }
            }
        }
        
        if(countRevalue % 100000 == 0 || countRevalue >= stop_criteria){
            string path = instance + "_" + algorithm + "_" + to_string(countRevalue) + ".txt";
            pareto->printAllSolutions(root_folder + path); 

            if(countRevalue >= stop_criteria){
                pareto->printAllSolutionsLayout(root_folder + instance + "_" + algorithm + "_layout.txt");
            }
        }

        generation++;
    }

    infoRunBrkga.close();

    return population;

}

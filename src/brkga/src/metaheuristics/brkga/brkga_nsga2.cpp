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
#include "../../../headers/metaheuristics/brkga/modules/update_population_brkga.h"

#include "../../../headers/metaheuristics/brkga/brkga_moead.h"

#include "../../../headers/metaheuristics/nsga2/modules/non_dominated_sorting.h"
#include "../../../headers/metaheuristics/nsga2/modules/crowding_distance.h"



#include "../../../headers/global_modules/genetic_operators/mutation.h"
#include "../../../headers/global_modules/genetic_operators/crossover.h"

#include "../../../headers/global_modules/dominates.h"
#include "../../../headers/global_modules/isEqual.h"

using namespace std;




vector<Solution> brkga_nsga2(vector<Solution>& population) {

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    
    
    
    // BRKGA parameters
    int size_population = population.size();
    double elite_fraction = 0.3;     // fração da elite
    double mutant_fraction = 0.1;    // fração de mutantes
    double bias = 0.9;
    int stop_criteria = 1000000;


    int num_elite = elite_fraction * size_population;
    int num_mutants = mutant_fraction * size_population;

    int chromosome_size = 0;
    for (int z = 0; z < num_zones; z++) {
        chromosome_size += foundations[z].size();
    }


    int generation = 0;

    ofstream infoRunBrkga(root_folder + "infoRun.txt");

    while (countRevalue < stop_criteria) {
        
        infoRunBrkga << "Generation " << generation << " | Revalues: " << countRevalue << " | GridSize: " << pareto->getSize() << endl;


        vector<Solution*> population_ptr;
        for (auto& sol : population) {
            population_ptr.push_back(&sol);
        }

        auto fronts_ptr = non_dominated_sorting(population_ptr);
        vector<Solution> elite;
        int k = 0;
        for (auto& front_ptr : fronts_ptr) {
            vector<Solution*> &front = *front_ptr;
            if(front.size() + elite.size() <= num_elite) {
                for (auto sol_ptr : front) {
                    elite.push_back(*sol_ptr); // copia o objeto apontado
                }
            }
            else {
                auto front_sorted_ptr = crowding_distance(front);
                int remaining_spots = num_elite - elite.size();
                for (int i = 0; i < remaining_spots && i < front_sorted_ptr.size(); i++) {
                    elite.push_back(*front_sorted_ptr[i]);
                }
                break;
        }
        k++;
    }


        // Mutantes (novos cromossomos aleatórios)
        vector<Solution> mutants;
        for (int i = 0; i < num_mutants; i++) {
            vector<double> chromo;
            for (int j = 0; j < chromosome_size; j++) {
                chromo.push_back(dis(gen));
            }
            mutants.push_back(decode_brkga(chromo)); // decodificação necessária
        }

        // Descendentes
        vector<Solution> offspring;
        while (offspring.size() < size_population - num_elite - num_mutants) {
            // escolhe aleatoriamente um pai da elite e um não-elite
            uniform_int_distribution<> dis_elite(0, num_elite - 1);
            uniform_int_distribution<> dis_nonelite(num_elite, size_population - 1);

            Solution parent_e = population[dis_elite(gen)];
            Solution parent_n = population[dis_nonelite(gen)];

            vector<double> child_chromo(chromosome_size);
            for (int j = 0; j < chromosome_size; j++) {
                double r = dis(gen);
                child_chromo[j] = (r < bias) ? parent_e.chromosome[j] : parent_n.chromosome[j];
            }
            offspring.push_back(decode_brkga(child_chromo));
        }

        // Nova população = elite + mutantes + descendentes
        population.clear();
        population.insert(population.end(), elite.begin(), elite.end());
        population.insert(population.end(), mutants.begin(), mutants.end());
        population.insert(population.end(), offspring.begin(), offspring.end());


        generation++;
    }

    updatePopulationBrkga(population);

    infoRunBrkga.close();




    return population;
}

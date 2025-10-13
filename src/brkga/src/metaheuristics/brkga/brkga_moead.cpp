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


#include "../../../headers/metaheuristics/moead/modules/generate_weight_vectors.h"
#include "../../../headers/metaheuristics/moead/modules/generate_neighborhood.h"
#include "../../../headers/metaheuristics/moead/modules/get_best_z_point.h"
#include "../../../headers/metaheuristics/moead/modules/tchebycheff.h"
#include "../../../headers/metaheuristics/moead/modules/updateEP.h"

#include "../../../headers/global_modules/genetic_operators/mutation.h"
#include "../../../headers/global_modules/genetic_operators/crossover.h"

#include "../../../headers/global_modules/dominates.h"
#include "../../../headers/global_modules/isEqual.h"

using namespace std;



vector<Solution> elitism_moead(vector<Solution>& population, int elite_size) {
    int size_population = population.size();

    // Build up External Population (EP)
    list<Solution*> EP;
    for (auto& sol : population) {
        updateEP(EP, &sol);
    }

    // Generate weight vector (lambda)
    vector<pair<double, double>> lambda_vector = build_weight_vector(size_population);

    // Generate neighborhood
    int number_of_neighbors = min(150, size_population-1);
    vector<vector<int>> neighborhood = build_neighborhood(number_of_neighbors, lambda_vector, size_population);

    // Calculate z-point (ideal point)
    pair<double, double> z_point;
    if (EP.empty()) {
        z_point = get_best_z_point(population);
    } else {
        // Convert EP (list<Solution*>) to vector<Solution>
        vector<Solution> ep_vec;
        for (auto ptr : EP) {
            ep_vec.push_back(*ptr);
        }
        z_point = get_best_z_point(ep_vec);
    }

    // Calculate Tchebycheff values
    vector<double> tch_vector(size_population);
    for (int i = 0; i < size_population; i++) {
        tch_vector[i] = calculate_gte(population[i].fitness, lambda_vector[i], z_point);
    }

    // Selecting the greatest values (Tchebycheff minors values)
    vector<int> indices(size_population);
    iota(indices.begin(), indices.end(), 0);

    sort(indices.begin(), indices.end(), [&](int a, int b) {
        return tch_vector[a] < tch_vector[b];
    });

    // Construindo elite com base na diversidade (usando EP + Tchebycheff)
    vector<Solution> elite;
    elite.reserve(elite_size);


    for (int i = 0; i < size_population && elite.size() < elite_size; i++) {
        bool dominated = false;
        for (auto& e : elite) {
            if (dominates(e, population[indices[i]]) ||
                isEqual(e, population[indices[i]])) {
                dominated = true;
                break;
                }
        }
        if (!dominated) elite.push_back(population[indices[i]]);
    }

    // Se não atingiu o tamanho, completa com os melhores Tchebycheff restantes
    int i = 0;
    while (elite.size() < elite_size && i < size_population) {
        elite.push_back(population[indices[i++]]);
    }

    return elite;
}



vector<Solution> brkga_moead(vector<Solution>& population) {

    
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

        // Ordena população pela estrategia de decomposicao do MOEAD
        vector<Solution> elite = elitism_moead(population, num_elite);


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

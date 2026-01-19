// brkga_moead.cpp
#include <utility>
#include <vector>
#include <iostream>
#include <ctime> 
#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_set>
#include <fstream>

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"
#include "../../../headers/global_modules/generate_initial_population/create_population_brkga.h"

#include "../../../headers/metaheuristics/brkga/modules/decode_brkga.h"

#include "../../../headers/metaheuristics/moead/modules/generate_weight_vectors.h"
#include "../../../headers/metaheuristics/moead/modules/generate_neighborhood.h"
#include "../../../headers/metaheuristics/moead/modules/get_best_z_point.h"
#include "../../../headers/metaheuristics/moead/modules/tchebycheff.h"

#include "../../../headers/global_modules/genetic_operators/mutation_brkga.h"
#include "../../../headers/global_modules/genetic_operators/crossover_brkga.h"

#include "../../../headers/global_modules/dominates.h"
#include "../../../headers/global_modules/isEqual.h"

using namespace std;

vector<Solution> brkga_moead(vector<Solution>& population) {

    // RNG
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> uniform01(0.0, 1.0);

    // Parâmetros
    const double elite_fraction = 0.20;
    const double mutant_fraction = 0.10;
    const double bias = 0.7;
    const double mutation_prob = 0.70;
    const int number_of_neighbors = 10;

    const int total_population = population.size();
    if (total_population <= 0) return population;

    // TAMANHO REAL DO CROMOSSOMO PARA ESSA INSTÂNCIA
    const size_t CHROMOSOME_SIZE = population[0].chromosome.size();

    // Vetores de peso e vizinhança (MOEA/D)
    vector<pair<double,double>> lambda_vector = build_weight_vector(total_population);
    vector<vector<int>> neighborhood = build_neighborhood(number_of_neighbors, lambda_vector, total_population);

    // z point
    pair<double,double> z_point = get_best_z_point(population);

    int target_elite   = max(1, static_cast<int>(round(elite_fraction * total_population)));
    int target_mutants = static_cast<int>(floor(mutant_fraction * total_population));

    if (target_elite + target_mutants > total_population) {
        target_mutants = total_population - target_elite;
    }

    int generation = 0;
    ofstream infoRunBrkga(root_folder + "infoRun.txt");

    while (countRevalue < stop_criteria) {

        infoRunBrkga << "Generation " << generation
                     << " | Revalues: " << countRevalue
                     << " | GridSize: " << pareto->getSize()
                     << endl;

        // ==================================
        // 1. ELITES escolhidos pelo MOEA/D
        // Usar vizinhança em vez de toda população (mais eficiente e alinhado com MOEA/D)
        // ==================================
        vector<int> elite_indices;
        elite_indices.reserve(target_elite);

        unordered_set<int> elite_set;

        // Usar vizinhança para seleção (O(n*k) em vez de O(n²))
        for (int j = 0; j < (int)lambda_vector.size() && (int)elite_indices.size() < target_elite; ++j) {
            double best_g = numeric_limits<double>::infinity();
            int best_idx = -1;

            // Buscar na vizinhança do subproblema j (mais eficiente e alinhado com MOEA/D)
            for (int neighbor_idx : neighborhood[j]) {
                double g = calculate_gte(population[neighbor_idx].fitness, lambda_vector[j], z_point);
                if (g < best_g) {
                    best_g = g;
                    best_idx = neighbor_idx;
                }
            }
            
            // Se não encontrou na vizinhança ou vizinhança muito pequena, buscar na população toda
            if (best_idx == -1 || neighborhood[j].size() < 3) {
                for (int i = 0; i < total_population; ++i) {
                    double g = calculate_gte(population[i].fitness, lambda_vector[j], z_point);
                    if (g < best_g) {
                        best_g = g;
                        best_idx = i;
                    }
                }
            }

            if (best_idx != -1 && elite_set.insert(best_idx).second) {
                elite_indices.push_back(best_idx);
            }
        }

        // Preenchimento por soma dos objetivos (caso falte elite)
        if ((int)elite_indices.size() < target_elite) {

            vector<int> idxs(total_population);
            iota(idxs.begin(), idxs.end(), 0);

            sort(idxs.begin(), idxs.end(), [&](int a, int b){
                double sa = population[a].fitness.first + population[a].fitness.second;
                double sb = population[b].fitness.first + population[b].fitness.second;
                return sa > sb;
            });

            for (int id : idxs) {
                if (elite_set.insert(id).second) {
                    elite_indices.push_back(id);
                    if ((int)elite_indices.size() >= target_elite)
                        break;
                }
            }
        }

        // Lista dos não elitistas
        vector<int> non_elite_indices;
        non_elite_indices.reserve(total_population - elite_indices.size());

        for (int i = 0; i < total_population; ++i) {
            if (elite_set.find(i) == elite_set.end()) {
                non_elite_indices.push_back(i);
            }
        }

        if (non_elite_indices.empty()) {
            for (int i = 0; i < total_population; ++i)
                non_elite_indices.push_back(i);
        }

        // ==================================
        // 2. Criar nova população (BRKGA)
        // ==================================
        vector<Solution> new_population;
        new_population.reserve(total_population);

        // ELITISTAS
        for (int idx : elite_indices) {
            new_population.push_back(population[idx]);
        }

        int need_children = total_population - new_population.size() - target_mutants;
        if (need_children < 0) need_children = 0;

        uniform_int_distribution<> pick_elite(0, max(0, (int)elite_indices.size() - 1));
        uniform_int_distribution<> pick_nonelite(0, max(0, (int)non_elite_indices.size() - 1));
        uniform_int_distribution<> pick_subproblem(0, max(0, total_population - 1));

        // ===============================
        // CROSSOVERS
        // Usar vizinhança para seleção de pais (alinhado com MOEA/D)
        // ===============================
        for (int i = 0; i < need_children; ++i) {
            // Selecionar subproblema aleatório e usar sua vizinhança
            int subproblem_idx = pick_subproblem(gen);
            
            Solution& parentElite = population[ elite_indices[pick_elite(gen)] ];  // Default
            Solution& parentNonElite = population[ non_elite_indices[pick_nonelite(gen)] ];  // Default
            
            // 70% das vezes usar vizinhança, 30% usar elite/non-elite global
            if (uniform01(gen) < 0.7 && !neighborhood[subproblem_idx].empty()) {
                // Selecionar pais da vizinhança do subproblema
                uniform_int_distribution<> pick_neighbor(0, neighborhood[subproblem_idx].size() - 1);
                int idx1 = neighborhood[subproblem_idx][pick_neighbor(gen)];
                int idx2 = neighborhood[subproblem_idx][pick_neighbor(gen)];
                while (idx1 == idx2 && neighborhood[subproblem_idx].size() > 1) {
                    idx2 = neighborhood[subproblem_idx][pick_neighbor(gen)];
                }
                parentElite = population[idx1];
                parentNonElite = population[idx2];
            }

            Solution child = crossover_brkga(parentElite, parentNonElite, bias);

            
            if (child.chromosome.size() != CHROMOSOME_SIZE) {
                child.chromosome.resize(CHROMOSOME_SIZE);
                for (auto &g : child.chromosome)
                    if (g < 0.0 || g > 1.0)
                        g = uniform01(gen);
            }

            if (uniform01(gen) < mutation_prob) {
                mutation_brkga(child, mutation_prob);
            }

            new_population.push_back(std::move(child));
        }

        // ===============================
        // MUTANTES
        // ===============================
        for (int i = 0; i < target_mutants; ++i) {

            Solution mutant = population[ non_elite_indices[pick_nonelite(gen)] ];

            mutation_brkga(mutant, mutation_prob);

            if (mutant.chromosome.size() != CHROMOSOME_SIZE) {
                mutant.chromosome.resize(CHROMOSOME_SIZE);
                for (auto &g : mutant.chromosome)
                    if (g < 0.0 || g > 1.0)
                        g = uniform01(gen);
            }

            new_population.push_back(std::move(mutant));
        }

        // Preencher até o tamanho se necessário (gerando novas soluções aleatórias)
        int max_attempts = total_population * 3;
        int attempts = 0;
        while ((int)new_population.size() < total_population && attempts < max_attempts) {
            attempts++;
            vector<Solution> temp = create_initial_population_brkga(1);

            for (auto &s : temp) {
                if (s.chromosome.size() != CHROMOSOME_SIZE)
                    s.chromosome.resize(CHROMOSOME_SIZE);

                if ((int)new_population.size() < total_population)
                    new_population.push_back(std::move(s));
            }
        }
        
        // Se ainda não atingiu o tamanho, duplicar soluções existentes (último recurso)
        if ((int)new_population.size() < total_population && !new_population.empty()) {
            int needed = total_population - (int)new_population.size();
            for (int i = 0; i < needed && i < (int)new_population.size(); i++) {
                new_population.push_back(new_population[i]);
            }
        }

        if ((int)new_population.size() > total_population) {
            new_population.resize(total_population);
        }

        // ==================================
        // 3. Atualizar z_point com população atual + Pareto
        // ==================================
        // Atualizar com população atual primeiro
        for (const auto& sol : new_population) {
            z_point.first  = max(z_point.first,  sol.fitness.first);
            z_point.second = max(z_point.second, sol.fitness.second);
        }
        // Depois atualizar com Pareto (pode ter soluções melhores)
        for (const auto sol : pareto->getElementos()) {
            z_point.first  = max(z_point.first,  sol->fitness.first);
            z_point.second = max(z_point.second, sol->fitness.second);
        }

        population = std::move(new_population);
        generation++;
    }

    infoRunBrkga.close();

    return population;
}



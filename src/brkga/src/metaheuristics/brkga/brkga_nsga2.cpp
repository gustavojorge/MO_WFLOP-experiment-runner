#include <utility>
#include <vector>
#include <iostream>
#include <ctime> 
#include <algorithm>
#include <numeric>
#include <random>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <set>

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"
#include "../../../headers/global_modules/generate_initial_population/create_population_brkga.h"

#include "../../../headers/metaheuristics/brkga/modules/decode_brkga.h" 
#include "../../../headers/metaheuristics/brkga/modules/update_population_brkga.h"

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

// ========================================================================
// SELEÇÃO DE ELITE USANDO FAST NONDOMINATED SORTING + CROWDING DISTANCE
// ========================================================================
vector<Solution*> select_elite_nsga2(vector<Solution*>& population, int elite_size) {
    vector<Solution*> elite;
    
    if (population.empty()) return elite;
    
    // Fast nondominated sorting
    vector<vector<Solution*>*>* fronts = new vector<vector<Solution*>*>();
    *fronts = arena_non_dominated_sorting(population);
    
    // Adiciona soluções não-dominadas até atingir elite_size
    for (auto& front : *fronts) {
        if ((int)elite.size() >= elite_size) break;
        
        if ((int)elite.size() + (int)front->size() <= elite_size) {
            // Front cabe inteiro
            for (auto solution : *front) {
                elite.push_back(new Solution(*solution));
            }
        } else {
            // Front não cabe - usa crowding distance
            vector<Solution*>* front_sorted = new vector<Solution*>();
            *front_sorted = crowding_distance(*front);
            
            int remaining = elite_size - (int)elite.size();
            for (int i = 0; i < remaining && i < (int)front_sorted->size(); ++i) {
                elite.push_back(new Solution(*(*front_sorted)[i]));
            }
            
            delete front_sorted;
            break;
        }
    }
    
    // Libera memória dos fronts
    for (auto front : *fronts) {
        for (auto solution : *front) {
            delete solution;
        }
        delete front;
    }
    delete fronts;
    
    return elite;
}

// ========================================================================
// GERA MUTANTES (SOLUÇÕES ALEATÓRIAS)
// ========================================================================
vector<Solution*> generate_mutants(int mutant_size) {
    vector<Solution*> mutants;
    mutants.reserve(mutant_size);
    
    vector<Solution> random_pop = create_initial_population_brkga(mutant_size);
    
    for (auto& sol : random_pop) {
        Solution* s = new Solution(sol);
        mutants.push_back(s);
        // countRevalue já foi incrementado em create_initial_population_brkga
    }
    
    return mutants;
}

// ========================================================================
// GERA OFFSPRING VIA CROSSOVER E MUTAÇÃO (USANDO FUNÇÕES AUXILIARES)
// ========================================================================
vector<Solution*> generate_offspring(
    vector<Solution*>& elite,
    vector<Solution*>& non_elite,
    int offspring_size,
    double rho_e,
    double mutation_prob) {
    
    vector<Solution*> offspring;
    offspring.reserve(offspring_size);
    
    default_random_engine re{(unsigned)time(nullptr)};
    uniform_real_distribution<double> dist(0.0, 1.0);
    uniform_int_distribution<> pick_elite(0, max(0, (int)elite.size() - 1));
    uniform_int_distribution<> pick_non_elite(0, max(0, (int)non_elite.size() - 1));
    
    for (int i = 0; i < offspring_size; ++i) {
        if (elite.empty() || non_elite.empty()) break;
        
        
        Solution& parent_elite = *elite[pick_elite(re)];
        Solution& parent_non_elite = *non_elite[pick_non_elite(re)];
        
        // CROSSOVER (já incrementa countRevalue e salva checkpoints)
        Solution child = crossover_brkga(parent_elite, parent_non_elite, rho_e);
        
        // MUTAÇÃO (já incrementa countRevalue e salva checkpoints)
        if (dist(re) < mutation_prob) {
            child = mutation_brkga(child, mutation_prob);
        }
        
        Solution* offspring_ptr = new Solution(child);
        offspring.push_back(offspring_ptr);
    }
    
    return offspring;
}

// ========================================================================
// ALGORITMO BRKGA MULTI-OBJETIVO COM ELITISMO NSGA-II
// ========================================================================
vector<Solution*> brkga_nsga2(vector<Solution>& initial_population) {

    // --- Inicializa população ---
    vector<Solution*> population_ptrs;
    population_ptrs.reserve(initial_population.size());

    for (auto &sol : initial_population) {
        Solution* s = new Solution(sol);
        population_ptrs.push_back(s);
    }

    int p = (int)population_ptrs.size(); // Tamanho da população
    
    // --- PARÂMETROS RECOMENDADOS ---
    double pe_fraction = 0.20;  // 0.10p a 0.25p (20% da população)
    double pm_fraction = 0.15;  // 0.10p a 0.30p (15% da população)
    double rho_e = 0.70;         // 0.5 a 0.8
    double mutation_prob = 0.7;  // Taxa de mutação
    
    int pe = max(1, (int)round(pe_fraction * p));   // Elite size
    int pm = max(1, (int)round(pm_fraction * p));   // Mutant size
    int pc = p - pe - pm;                            // Crossover offspring size
    
    int generation = 0;

    if (population_ptrs.empty()) return population_ptrs;
    const size_t CHROMOSOME_SIZE = population_ptrs[0]->chromosome.size();
    if (CHROMOSOME_SIZE == 0) {
        cerr << "[brkga_nsga2] ERROR: chromosome size is zero\n";
        return population_ptrs;
    }

    ofstream infoRunBrkga(root_folder + "infoRun.txt");
    
    cout << "\n========== BRKGA-NSGA2 HYBRID ==========" << endl;
    cout << "Population size: " << p << endl;
    cout << "Elite size (pe): " << pe << endl;
    cout << "Mutant size (pm): " << pm << endl;
    cout << "Offspring size (pc): " << pc << endl;
    cout << "Rho_e (bias): " << rho_e << endl;
    cout << "========================================\n" << endl;
    
    // Loop principal
    while (countRevalue < stop_criteria) {

        infoRunBrkga << "Generation " << generation
                     << " | Revalues: " << countRevalue
                     << " | GridSize: " << pareto->getSize()
                     << endl;

        // =========================================================
        // PARTE 1: SELECIONAR ELITE (pe soluções preferidas)
        // =========================================================
        vector<Solution*> elite = select_elite_nsga2(population_ptrs, pe);
        
        if (elite.empty() && !population_ptrs.empty()) {
            elite.push_back(new Solution(*population_ptrs[0]));
        }
        
        // =========================================================
        // PARTE 2: GERAR MUTANTES (pm soluções aleatórias)
        // =========================================================
        vector<Solution*> mutants = generate_mutants(pm);
        
        // =========================================================
        // PARTE 3: GERAR OFFSPRING VIA CROSSOVER + MUTAÇÃO
        // =========================================================
        // Identifica não-elite da população atual
        set<Solution*> elite_ptr_set(elite.begin(), elite.end());
        vector<Solution*> non_elite;
        
        for (auto* sol : population_ptrs) {
            bool is_in_elite = false;
            for (auto* e : elite) {
                if (isEqual(*sol, *e)) {
                    is_in_elite = true;
                    break;
                }
            }
            if (!is_in_elite) {
                non_elite.push_back(sol);
            }
        }
        
        if (non_elite.empty()) {
            non_elite = elite;
        }
        
        // Gera offspring 
        vector<Solution*> offspring = generate_offspring(elite, non_elite, pc, rho_e, mutation_prob);
        
        // =========================================================
        // MONTAR NOVA POPULAÇÃO: Elite + Mutants + Offspring
        // =========================================================
        vector<Solution*> new_population;
        new_population.reserve(p);
        
        // Adiciona elite
        for (auto* sol : elite) {
            new_population.push_back(new Solution(*sol));
        }
        
        // Adiciona mutantes
        for (auto* sol : mutants) {
            new_population.push_back(sol);
        }
        
        // Adiciona offspring
        for (auto* sol : offspring) {
            new_population.push_back(sol);
        }
        
        // =========================================================
        // SELEÇÃO PARA PRÓXIMA ITERAÇÃO (Fast Nondominated Sorting)
        // =========================================================
        if ((int)new_population.size() > p) {
            vector<vector<Solution*>*>* fronts = new vector<vector<Solution*>*>();
            *fronts = arena_non_dominated_sorting(new_population);
            
            vector<Solution*> selected_population;
            
            for (auto& front : *fronts) {
                if ((int)selected_population.size() >= p) break;
                
                if ((int)selected_population.size() + (int)front->size() <= p) {
                    for (auto solution : *front) {
                        selected_population.push_back(new Solution(*solution));
                    }
                } else {
                    vector<Solution*>* front_sorted = new vector<Solution*>();
                    *front_sorted = crowding_distance(*front);
                    
                    int remaining = p - (int)selected_population.size();
                    for (int i = 0; i < remaining && i < (int)front_sorted->size(); ++i) {
                        selected_population.push_back(new Solution(*(*front_sorted)[i]));
                    }
                    
                    delete front_sorted;
                    break;
                }
            }
            
            // Libera memória
            for (auto front : *fronts) {
                for (auto solution : *front) {
                    delete solution;
                }
                delete front;
            }
            delete fronts;
            
            for (auto sol : new_population) {
                delete sol;
            }
            
            population_ptrs = selected_population;
        } else {
            for (auto sol : population_ptrs) {
                delete sol;
            }
            population_ptrs = new_population;
        }
        
        // Libera elite
        for (auto sol : elite) {
            delete sol;
        }
        
        generation++;
    }

    infoRunBrkga.close();
    return population_ptrs;
}


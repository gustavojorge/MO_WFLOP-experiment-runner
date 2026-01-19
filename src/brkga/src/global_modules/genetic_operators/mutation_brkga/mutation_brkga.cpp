#include <utility>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <random>

#include "../../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../../headers/metaheuristics/brkga/modules/decode_brkga.h"
#include "../../../../headers/metaheuristics/brkga/modules/decodeAlternative_brkga.h"
#include "../../../../headers/global_modules/dominates.h"

using namespace std;

void mutationAux_brkga(vector<double>& chromosome, double mutation_rate) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 1.0);

    for (auto& gene : chromosome) {
        if (dist(gen) < mutation_rate) {
            // Reatribui gene aleatório ou levemente perturbado
            gene = dist(gen);
        }
    }
}

Solution mutation_brkga(Solution& offspring, double mutation_rate) {
    mutationAux_brkga(offspring.chromosome, mutation_rate);
    Solution mutated = decode_brkga(offspring.chromosome);
    Solution mutated2 = decodeAlternative_brkga(offspring.chromosome);
    Solution* sol_ptr;

    if (dominates(mutated2, mutated)) {
        sol_ptr = new Solution(mutated2);
        pareto->adicionarSol(sol_ptr);
        countRevalue++;
        mutated = mutated2;
    }
    else if (dominates(mutated, mutated2)) {
        sol_ptr = new Solution(mutated);
        pareto->adicionarSol(sol_ptr);
        countRevalue++;
    }
    else {
        if (rand() % 2 == 0) {
            sol_ptr = new Solution(mutated);
            pareto->adicionarSol(sol_ptr);
            countRevalue++;
        } else {
            sol_ptr = new Solution(mutated2);
            pareto->adicionarSol(sol_ptr);
            countRevalue++;
            mutated = mutated2;
        }
    }

    if (countRevalue % 100000 == 0) {
        string path = instance + "_" + algorithm + "_" + to_string(countRevalue) + ".txt";
        pareto->printAllSolutions(root_folder + path);
        if (countRevalue >= stop_criteria)
            pareto->printAllSolutionsLayout(root_folder + instance + "_" + algorithm + "_layout.txt");
    }

    delete sol_ptr;

    return mutated;
}

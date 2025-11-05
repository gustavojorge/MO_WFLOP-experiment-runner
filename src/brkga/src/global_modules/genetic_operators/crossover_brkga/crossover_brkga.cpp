#include <utility>
#include <vector>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <random>

#include "../../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../../headers/metaheuristics/brkga/modules/decode_brkga.h"

using namespace std;

vector<double> crossoverAux_brkga(const Solution& parent1, const Solution& parent2, double bias) {
    size_t n = parent1.chromosome.size();
    vector<double> offspring(n);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 1.0);

    for (size_t i = 0; i < n; ++i) {
        double r = dist(gen);
        // Escolhe gene de p1 ou p2
        offspring[i] = (r < bias) ? parent1.chromosome[i] : parent2.chromosome[i];

        // Pequeno ruído para evitar clones idênticos
        offspring[i] += 0.05 * (dist(gen) - 0.5);
        offspring[i] = min(1.0, max(0.0, offspring[i]));
    }

    return offspring;
}

Solution crossover_brkga(const Solution& parent1, const Solution& parent2, double bias) {
    vector<double> offspring_chromosome = crossoverAux_brkga(parent1, parent2, bias);

    Solution offspring_solution = decode_brkga(offspring_chromosome);

    // Adiciona ao Pareto
    pareto->adicionarSol(new Solution(offspring_solution));
    countRevalue++;

    if (countRevalue % 100000 == 0) {
        string path = instance + "_" + algorithm + "_" + to_string(countRevalue) + ".txt";
        pareto->printAllSolutions(root_folder + path);
        if (countRevalue >= stop_criteria)
            pareto->printAllSolutionsLayout(root_folder + instance + "_" + algorithm + "_layout.txt");
    }

    return offspring_solution;
}

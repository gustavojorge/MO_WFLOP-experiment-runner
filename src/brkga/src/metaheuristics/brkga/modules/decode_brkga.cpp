#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"

using namespace std;

Solution decode_brkga(const vector<double>& chromosome) {
    Solution brkgaSol; // solução a retornar (vazia por padrão)

    // Segurança: expected size
    int expected_size = 0;
    for (int z = 0; z < num_zones; z++) {
        expected_size += (int)foundations[z].size();
    }

    if ((int)chromosome.size() != expected_size) {
        cerr << "[decode_brkga] ERROR: expected_size=" << expected_size
             << ", chromosome.size()=" << chromosome.size() << "\n";

        brkgaSol.chromosome = chromosome;
        brkgaSol.grid = vector<vector<int>>(num_zones); // vazio por zona
        brkgaSol.turbines = vector<vector<Turbine>>(num_zones);
        brkgaSol.fitness.first = numeric_limits<double>::infinity();
        brkgaSol.fitness.second = -numeric_limits<double>::infinity();
        return brkgaSol;
    }

    // Inicializa contêineres
    vector<vector<Turbine>> turbines(num_zones);
    for (int z = 0; z < num_zones; z++) {
        int tz = turbines_per_zone[z];
        if (tz < 0) tz = 0;
        turbines[z].resize(tz);
    }

    vector<vector<int>> solution_grid(num_zones);
    for (int z = 0; z < num_zones; z++) {
        int fp = (int)foundations[z].size();
        solution_grid[z] = vector<int>(fp, -1);
    }

    Turbine base_turbine;
    base_turbine.diameter = 240;
    base_turbine.height = 150;
    base_turbine.power = power;
    base_turbine.thrust_coef = thrust_coef;

    int pos_chrom = 0;
    int id = (int)fixd.size();

    // Montagem por zona
    for (int z = 0; z < num_zones; z++) {
        int n_positions = (int)foundations[z].size();
        int n_turbines = min(turbines_per_zone[z], n_positions);
        if (n_positions <= 0) continue;

        // Cria pares (chave, índice)
        vector<pair<double,int>> keys;
        keys.reserve(n_positions);
        for (int i = 0; i < n_positions; ++i) {
            if (pos_chrom >= (int)chromosome.size()) {
                cerr << "[decode_brkga] Warning: pos_chrom exceeded at zone " << z << "\n";
                break;
            }
            keys.emplace_back(chromosome[pos_chrom++], i);
        }

        if ((int)keys.size() != n_positions) {
            // problema no cromossomo; preenche o restante com chaves grandes (baixa prioridade)
            for (int k = (int)keys.size(); k < n_positions; ++k)
                keys.emplace_back(1.0, k);
        }

        sort(keys.begin(), keys.end(),
             [](const pair<double,int>& a, const pair<double,int>& b){ return a.first < b.first; });

        // Posiciona as turbinas nas n_turbines primeiras prioridades
        for (int i = 0; i < n_turbines && i < (int)keys.size(); ++i) {
            int elmn = keys[i].second;
            if (elmn < 0 || elmn >= n_positions) {
                cerr << "[decode_brkga] Warning: elmn out of range z=" << z << " elmn=" << elmn << "\n";
                continue;
            }

            Turbine t = base_turbine;
            t.id = id++;
            t.index = elmn;
            t.zone = z;
            t.x = foundations[z][elmn].x;
            t.y = foundations[z][elmn].y;

            solution_grid[z][elmn] = t.id;
            if (i < (int)turbines[z].size()) turbines[z][i] = t;
            else turbines[z].push_back(t);
        }
    }

    // Monta a solução final
    brkgaSol.chromosome = chromosome;
    brkgaSol.grid = move(solution_grid);
    brkgaSol.turbines = move(turbines);

    // Avaliação com checagens defensivas
    try {
        brkgaSol.fitness.first = calculate_cost(brkgaSol);
    } catch (...) {
        cerr << "[decode_brkga] Exception in calculate_cost, assigning INF\n";
        brkgaSol.fitness.first = numeric_limits<double>::infinity();
    }

    try {
        brkgaSol.fitness.second = calculate_power(brkgaSol);
    } catch (...) {
        cerr << "[decode_brkga] Exception in calculate_power, assigning -INF\n";
        brkgaSol.fitness.second = -numeric_limits<double>::infinity();
    }

    return brkgaSol;
}











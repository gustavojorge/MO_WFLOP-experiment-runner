#include <vector>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"


using namespace std;



Solution decode_brkga(const vector<double>& chromosome) {
    int id = fixd.size();

    // Calcula o tamanho esperado do cromossomo
    int expected_size = 0;
    for (int z = 0; z < num_zones; z++) {
        expected_size += foundations[z].size();
    }

    // Valida se o cromossomo recebido é do tamanho correto
    if ((int)chromosome.size() != expected_size) {
        cerr << "[decode_brkga] Erro: tamanho do cromossomo inválido. "
             << "Esperado = " << expected_size
             << ", recebido = " << chromosome.size() << endl;
        exit(1);
    }

    // Vetores de turbinas por zona
    vector<vector<Turbine>> turbines(num_zones);
    for(int i = 0; i < num_zones; i++){
        turbines[i].resize(turbines_per_zone[i]);
    }

    // Grid da solução
    vector<vector<int>> solution_grid(num_zones);
    for(int i = 0; i < num_zones; i++){
        solution_grid[i].resize(foundations[i].size(), -1); // -1 = vazio
    }

    Turbine t;
    t.diameter = 240;
    t.height = 150;

    int pos_chrom = 0; // posição no vetor de chaves

    for(int z = 0; z < num_zones; z++){
        int n_positions = foundations[z].size();
        int n_turbines  = turbines_per_zone[z];

        // cria pares (chave, indice)
        vector<pair<double,int>> keys;
        for (int i = 0; i < n_positions; i++) {
            keys.push_back({chromosome[pos_chrom++], i});
        }

        // ordena pelas chaves (menor chave = mais prioritário)
        sort(keys.begin(), keys.end());

        // seleciona as primeiras n_turbines foundations
        for(int i = 0; i < n_turbines; i++) {
            int elmn = keys[i].second; // índice da foundation escolhida

            t.id = id++;
            solution_grid[z][elmn] = t.id;
            t.index = elmn;
            t.x = foundations[z][elmn].x;
            t.y = foundations[z][elmn].y;
            t.power = power;
            t.thrust_coef = thrust_coef;
            t.zone = z;
            turbines[z][i] = t;
        }
    }

    // monta a solução final
    Solution brkgaSol;
    brkgaSol.chromosome = chromosome;
    brkgaSol.grid = solution_grid;
    brkgaSol.turbines = turbines;
    brkgaSol.fitness.first = calculate_cost(brkgaSol);
    brkgaSol.fitness.second = calculate_power(brkgaSol);

    return brkgaSol;
}

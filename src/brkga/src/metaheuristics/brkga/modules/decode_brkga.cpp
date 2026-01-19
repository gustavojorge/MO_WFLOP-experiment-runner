#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>
#include <unordered_set>

#include "../../../headers/global_modules/generate_initial_population/population.h"
#include "../../../headers/global_modules/generate_initial_population/generate_rSolution.h"
#include "decode_brkga.h"

using namespace std;


// Small helper to check finiteness and clamp gene if necessary
static inline double safe_gene(double g) {
    if (!isfinite(g)) return 1.0;
    if (g < 0.0) return 0.0;
    if (g > 1.0) return 1.0;
    return g;
}

// Basic, fast validation (no external deps) — returns true if solution seems valid
static bool basicValidateSolution(const Solution &sol) {
    // turbines size per zone must match turbines_per_zone
    for (int z = 0; z < num_zones; ++z) {
        int expected = turbines_per_zone[z] < 0 ? 0 : turbines_per_zone[z];
        if ((int)sol.turbines[z].size() != expected) return false;
    }

    // grid dimensions match foundations sizes
    for (int z = 0; z < num_zones; ++z) {
        if ((int)sol.grid[z].size() != (int)foundations[z].size()) return false;
    }

    // Each turbine index must point to a valid foundation and grid mapping must be consistent
    unordered_set<int> seen_ids;
    for (int z = 0; z < num_zones; ++z) {
        for (int i = 0; i < (int)sol.turbines[z].size(); ++i) {
            const Turbine &t = sol.turbines[z][i];
            // index range
            if (t.index < 0 || t.index >= (int)foundations[z].size()) return false;
            // grid mapping
            if (sol.grid[z][t.index] != t.id) return false;
            // coordinates consistent with foundations positions (allow tiny eps)
            double fx = foundations[z][t.index].x;
            double fy = foundations[z][t.index].y;
            if (fabs(t.x - fx) > 1e-6 || fabs(t.y - fy) > 1e-6) return false;
            // unique ids
            if (seen_ids.find(t.id) != seen_ids.end()) return false;
            seen_ids.insert(t.id);
        }
    }

    // No duplicated foundation indices inside each zone
    for (int z = 0; z < num_zones; ++z) {
        unordered_set<int> idxs;
        for (const auto &t : sol.turbines[z]) {
            if (idxs.find(t.index) != idxs.end()) return false;
            idxs.insert(t.index);
        }
    }

    return true;
}

Solution decode_brkga(const vector<double>& chromosome) {
    Solution brkgaSol; // will be returned

    // expected chromosome size
    int expected_size = 0;
    for (int z = 0; z < num_zones; ++z) expected_size += (int)foundations[z].size();

    if ((int)chromosome.size() != expected_size) {
        cerr << "[decode_brkga] ERROR: chromosome size mismatch. expected=" << expected_size
             << " got=" << chromosome.size() << ". Returning penalized solution.\n";

        // build penalized solution (empty but consistent)
        brkgaSol.chromosome = chromosome;
        brkgaSol.grid = vector<vector<int>>(num_zones);
        brkgaSol.turbines = vector<vector<Turbine>>(num_zones);
        for (int z = 0; z < num_zones; ++z) {
            brkgaSol.grid[z] = vector<int>(foundations[z].size(), -1);
        }
        brkgaSol.fitness.first = numeric_limits<double>::infinity();      // worst cost
        brkgaSol.fitness.second = -numeric_limits<double>::infinity();    // worst energy
        return brkgaSol;
    }

    // 1) initialize containers 
    vector<vector<Turbine>> turbines(num_zones);
    for (int z = 0; z < num_zones; ++z) {
        int tz = turbines_per_zone[z];
        if (tz < 0) tz = 0;
        turbines[z].reserve(tz); 
    }

    vector<vector<int>> solution_grid(num_zones);
    for (int z = 0; z < num_zones; ++z) {
        int fp = (int)foundations[z].size();
        solution_grid[z] = vector<int>(fp, -1);
    }

    // build base_turbine using sensible defaults or fixd's first turbine if present
    Turbine base_turbine;
    if (!fixd.empty()) {
        base_turbine.diameter = fixd[0].diameter;
        base_turbine.height = fixd[0].height;
    } else {
        base_turbine.diameter = 240;
        base_turbine.height = 150;
    }
    base_turbine.power = power;               // global power
    base_turbine.thrust_coef = thrust_coef;   // global thrust coefficient

    // For id uniqueness start from current fixed turbines count + a per-call offset
    int next_id = (int)fixd.size();
    // to avoid collisions within a single decode call, we'll increment from next_id

    // read chromosome per position (zone-major), create keys vector per zone
    int pos_chrom = 0;

    for (int z = 0; z < num_zones; ++z) {
        int n_positions = (int)foundations[z].size();
        int n_turbines = turbines_per_zone[z];
        if (n_turbines < 0) n_turbines = 0;
        if (n_positions <= 0) {
            // still advance pos_chrom by zero
            continue;
        }
        if (n_turbines > n_positions) n_turbines = n_positions;

        // read one gene per foundation position (zone-major)
        vector<pair<double,int>> keys; keys.reserve(n_positions);
        for (int i = 0; i < n_positions; ++i) {
            double gene = safe_gene(chromosome[pos_chrom++]); // safe clamp
            // We keep simple behavior: smaller gene => higher priority.
            // BUT we also include stable tie-breaker by index to avoid undefined behavior.
            keys.emplace_back(gene, i);
        }

        // Sort ascending by key (smaller->better). Tie-break by index ensures deterministic order.
        sort(keys.begin(), keys.end(), [](const pair<double,int>& a, const pair<double,int>& b){
            if (a.first != b.first) return a.first < b.first;
            return a.second < b.second;
        });

        // Now select the top-n_turbines, but we fill turbines[z] by push_back to ensure exact size.
        turbines[z].clear();
        turbines[z].reserve(n_turbines);
        for (int pick = 0, used = 0; used < n_turbines && pick < (int)keys.size(); ++pick) {
            int foundation_index = keys[pick].second;
            // create turbine from base, assign fields
            Turbine T = base_turbine;
            T.id = next_id++;
            T.index = foundation_index;
            T.zone = z;
            T.x = foundations[z][foundation_index].x;
            T.y = foundations[z][foundation_index].y;
            // keep power/thrust already set in base_turbine

            // place in grid and turbines vector
            solution_grid[z][foundation_index] = T.id;
            turbines[z].push_back(T);
            used++;
        }

        // Safety: if for some reason we didn't fill enough (shouldn't happen), fill with dummy valid picks
        if ((int)turbines[z].size() < n_turbines) {
            // choose remaining from remaining keys (if any)
            for (int pick = 0; pick < (int)keys.size() && (int)turbines[z].size() < n_turbines; ++pick) {
                int foundation_index = keys[pick].second;
                if (solution_grid[z][foundation_index] != -1) continue; // already used
                Turbine T = base_turbine;
                T.id = next_id++;
                T.index = foundation_index;
                T.zone = z;
                T.x = foundations[z][foundation_index].x;
                T.y = foundations[z][foundation_index].y;
                solution_grid[z][foundation_index] = T.id;
                turbines[z].push_back(T);
            }
        }
    } // end zones

    // assemble solution
    brkgaSol.chromosome = chromosome;
    brkgaSol.grid = move(solution_grid);
    brkgaSol.turbines = move(turbines);

    // Evaluate fitness with defensive try/catch; calculate_cost/calculate_power fill brkgaSol.fitness
    try {
        brkgaSol.fitness.first = calculate_cost(brkgaSol);
    } catch (...) {
        cerr << "[decode_brkga] Exception in calculate_cost; assigning +inf\n";
        brkgaSol.fitness.first = numeric_limits<double>::infinity();
    }
    try {
        brkgaSol.fitness.second = calculate_power(brkgaSol);
    } catch (...) {
        cerr << "[decode_brkga] Exception in calculate_power; assigning -inf\n";
        brkgaSol.fitness.second = -numeric_limits<double>::infinity();
    }

    // Basic validation before returning — if invalid, return strongly penalized solution
    if (!basicValidateSolution(brkgaSol)) {
        cerr << "[decode_brkga] WARNING: basic validation failed. Returning penalized solution.\n";
        Solution bad;
        bad.chromosome = chromosome;
        bad.grid = vector<vector<int>>(num_zones);
        bad.turbines = vector<vector<Turbine>>(num_zones);
        for (int z = 0; z < num_zones; ++z) bad.grid[z] = vector<int>(foundations[z].size(), -1);
        bad.fitness.first = numeric_limits<double>::infinity();
        bad.fitness.second = -numeric_limits<double>::infinity();
        return bad;
    }

    return brkgaSol;
}











#include <vector>
#include <limits>
#include <iostream>
#include <utility>
#include "../../../../headers/metaheuristics/moead/modules/get_best_z_point.h"

using namespace std;

pair<double, double> get_best_z_point(vector<Solution>& population) {
  // CORREÇÃO: z_point ideal = (maior custo negativo = menor custo, maior potência)
  // fitness.first é custo NEGATIVO (mais negativo = pior, menos negativo = melhor)
  // fitness.second é potência POSITIVA (maior = melhor)
  double best_cost = -numeric_limits<double>::infinity();  // Inicializa com -inf (pior possível)
  double best_power = -numeric_limits<double>::infinity(); // Inicializa com -inf (pior possível)

  for (auto& individual : population) {
    // Buscar MAIOR valor de fitness.first (menos negativo = menor custo real)
    if (individual.fitness.first > best_cost) {
      best_cost = individual.fitness.first;
    }
    // Buscar MAIOR valor de fitness.second (maior potência)
    if (individual.fitness.second > best_power) {
      best_power = individual.fitness.second;
    }
  }

  return make_pair(best_cost, best_power);
}

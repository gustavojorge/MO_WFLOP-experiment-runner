#include "../../../../headers/metaheuristics/comolsd/modules/calculate_ipbi.h"
using namespace std;

double calculate_ipbi(const pair<double, double>& fitness,
                      const pair<double, double>& lambda,
                      const pair<double, double>& nadir_point) {
    // 1. Construir o vetor d = f(x) - z^nadir
    double dx = fitness.first - nadir_point.first;
    double dy = fitness.second - nadir_point.second;

    // 2. Normalizar lambda
    double norm_lambda = sqrt(lambda.first * lambda.first + lambda.second * lambda.second);
    double lx = lambda.first / norm_lambda;
    double ly = lambda.second / norm_lambda;

    // 3. Distância projetada d1 (projeção de d em lambda normalizado)
    double d1 = dx * lx + dy * ly;

    // 4. Projeção ortogonal (d2 = ||d - d1 * lambda||)
    double px = d1 * lx;
    double py = d1 * ly;
    double d2 = sqrt((dx - px) * (dx - px) + (dy - py) * (dy - py));

    // 5. Função iPBI (normalmente com θ = 5.0)
    double theta = 5.0;
    return d1 + theta * d2;
}


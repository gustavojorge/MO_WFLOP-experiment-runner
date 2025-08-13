#include "../../../../headers/metaheuristics/comolsd/modules/aggregation_wrapper.h"

using namespace std;

//For functions that take 2 arguments, like calculate_ws
std::function<double(const pair<double, double>&,
                     const pair<double, double>&,
                     const pair<double, double>&)>
make_aggregation_function(
    double (*aggregation)(const pair<double, double>&,
                          const pair<double, double>&)) {
    return [aggregation](const pair<double, double>& fitness,
                         const pair<double, double>& lambda,
                         const pair<double, double>& /*unused*/) -> double {
        return aggregation(fitness, lambda);
    };
}

//For functions that take 3 arguments, like calculate_ipbi
std::function<double(const pair<double, double>&,
                     const pair<double, double>&,
                     const pair<double, double>&)>
make_aggregation_function(
    double (*aggregation)(const pair<double, double>&,
                          const pair<double, double>&,
                          const pair<double, double>&)) {
    return aggregation;
}

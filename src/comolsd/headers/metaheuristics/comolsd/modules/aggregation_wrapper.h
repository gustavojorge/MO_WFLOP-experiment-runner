#ifndef AGGREGATION_WRAPPER_H
#define AGGREGATION_WRAPPER_H

#include <functional>
#include <utility>
#include "calculate_ws.h" 
#include "calculate_ipbi.h"

// Generates the function compatible with the local_search signature
std::function<double(const std::pair<double, double>&,
                     const std::pair<double, double>&,
                     const std::pair<double, double>&)>
make_aggregation_function(
    double (*aggregation)(const std::pair<double, double>&,
                          const std::pair<double, double>&)
);

std::function<double(const std::pair<double, double>&,
                     const std::pair<double, double>&,
                     const std::pair<double, double>&)>
make_aggregation_function(
    double (*aggregation)(const std::pair<double, double>&,
                          const std::pair<double, double>&,
                          const std::pair<double, double>&)
);

#endif

#ifndef LOCAL_SEARCH_TYPE_H
#define LOCAL_SEARCH_TYPE_H

#include <string>
#include <vector>
#include <functional>
#include "../wind_farm_entities/solution.h"

enum class LocalSearchType {
    ParetoLS,
    AnytimePLS,
    None
};

LocalSearchType parseLocalSearchType(const std::string& typeStr);
std::function<std::vector<Solution*>*(std::vector<Solution*>)> getLocalSearchFunction(LocalSearchType type);

#endif
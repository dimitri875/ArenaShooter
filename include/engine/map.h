#pragma once

#include "structs.h"
#include <vector>
#include <string>

std::vector<std::string> tokenize(const std::string& str);

void loadLevelFromFile(const std::string& path, std::vector<Sector>& sectors, std::vector<Wall>& walls);

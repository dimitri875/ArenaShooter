#include "engine/map.h"
#include "structs.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> tokenize(const std::string& str)
{
    std::vector<std::string> tokens;
    std::string buffer;

    for (char c : str)
    {
        if (c == ' ')
        {
            if (buffer.empty())
                continue;

            tokens.push_back(buffer);
            buffer.clear();
        }
        else
        {
            buffer += c;
        }
    }

    if (!buffer.empty())
    {
        tokens.push_back(buffer);
        buffer.clear();
    }

    return tokens;
}

void loadLevelFromFile(const std::string& path, std::vector<Sector>& sectors, std::vector<Wall>& walls)
{
    std::ifstream file;
    file.open(path);

    if (!file.good())
    {
        std::cerr << "Error: Something went wrong while reading the file <" << path << ">\n";
        return;
    }

    std::string buffer;
    std::vector<std::string> sectorsRaw, wallsRaw;
    bool sector = true;

    while (std::getline(file, buffer))
    {
        if (buffer.empty())
        {
            sector = false;
            continue;
        }
        (sector) ? sectorsRaw.push_back(buffer) : wallsRaw.push_back(buffer);
    }

    std::cout << "[DEBUG] Read " << sectorsRaw.size() << " sector(s)" << std::endl;
    std::cout << "[DEBUG] Read " << wallsRaw.size() << " wall(s)" << std::endl;

    size_t sectorIdx = 0, wallIdx = 0;
    while (sectorIdx < sectorsRaw.size())
    {
        std::vector<std::string> tokens = tokenize(sectorsRaw[sectorIdx]);
        int id = std::stoi(tokens[0]), idx = std::stoi(tokens[1]), numWalls = std::stoi(tokens[2]);
        float floor = std::stof(tokens[3]), ceil = std::stof(tokens[4]);
        Sector newSector = {id, idx, numWalls, floor, ceil};

        sectors.push_back(newSector);

        for (int off = 0; off < numWalls; off++)
        {
            tokens = tokenize(wallsRaw[wallIdx++]);
            float x0 = std::stof(tokens[0]), y0 = std::stof(tokens[1]), x1 = std::stof(tokens[2]), y1 = std::stof(tokens[3]);
            int frontSector = sectorIdx+1;  // Current Sector ID
            int backSector = std::stoi(tokens[4]);   // Portal Sector ID
            Wall newWall = {x0, y0, x1, y1, frontSector, backSector};

            walls.push_back(newWall);
        }

        sectorIdx++;
    }
}
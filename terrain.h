#pragma once
#include <glm/glm.hpp>
#include <vector>

void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, TerrainMode mode, const char* heightMapFile = nullptr);

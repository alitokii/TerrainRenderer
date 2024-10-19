#pragma once
#include <glm/glm.hpp>
#include <vector>

enum class TerrainMode {
    PERLIN_NOISE,
    HEIGHTMAP_IMAGE
};

void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, TerrainMode mode, const char* heightMapFile = nullptr);
void loadHeightMap(const char* filename, std::vector<float>& heightMap, int& width, int& height);
glm::vec3 calculateNormal(float hL, float hR, float hD, float hU);

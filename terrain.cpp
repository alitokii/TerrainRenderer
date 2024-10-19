#include "terrain.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "perlin.h"
#include <iostream>
#include <cmath>

#include <cmath>
#include <random>

float octavePerlin(PerlinNoise& pn, float x, float y, int octaves, float persistence) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;
    for(int i = 0; i < octaves; i++) {
        total += pn.noise(x * frequency, y * frequency, 0.5) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }
    return total / maxValue;
}

void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, TerrainMode mode, const char* heightMapFile) {
    int width = 200, height = 200;
    std::vector<float> heightMap;
    PerlinNoise pn;

    if (mode == TerrainMode::HEIGHTMAP_IMAGE) {
        if (heightMapFile == nullptr) {
            std::cerr << "Height map file not provided for HEIGHTMAP_IMAGE mode." << std::endl;
            return;
        }
        loadHeightMap(heightMapFile, heightMap, width, height);
        if (heightMap.empty()) {
            std::cerr << "Failed to load height map. Exiting." << std::endl;
            return;
        }
    }

    float heightScale = 50.0f;  // Increased for more pronounced terrain
    float noiseScale = 0.03f;   // Reduced for smoother terrain

    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            float y;
            if (mode == TerrainMode::HEIGHTMAP_IMAGE) {
                y = heightMap[z * width + x] * heightScale;
            } else {
                y = octavePerlin(pn, x * noiseScale, z * noiseScale, 6, 0.5) * heightScale;
            }

            // Position
            vertices.push_back(static_cast<float>(x));
            vertices.push_back(y);
            vertices.push_back(static_cast<float>(z));

            // Normal calculation
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            if (x > 0 && x < width - 1 && z > 0 && z < height - 1) {
                float hL, hR, hD, hU;
                if (mode == TerrainMode::HEIGHTMAP_IMAGE) {
                    hL = heightMap[z * width + (x - 1)] * heightScale;
                    hR = heightMap[z * width + (x + 1)] * heightScale;
                    hD = heightMap[(z - 1) * width + x] * heightScale;
                    hU = heightMap[(z + 1) * width + x] * heightScale;
                } else {
                    hL = octavePerlin(pn, (x - 1) * noiseScale, z * noiseScale, 6, 0.5) * heightScale;
                    hR = octavePerlin(pn, (x + 1) * noiseScale, z * noiseScale, 6, 0.5) * heightScale;
                    hD = octavePerlin(pn, x * noiseScale, (z - 1) * noiseScale, 6, 0.5) * heightScale;
                    hU = octavePerlin(pn, x * noiseScale, (z + 1) * noiseScale, 6, 0.5) * heightScale;
                }
                normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
            }
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    // Generate indices (this part remains the same)
    for (int z = 0; z < height - 1; z++) {
        for (int x = 0; x < width - 1; x++) {
            unsigned int topLeft = z * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void loadHeightMap(const char* filename, std::vector<float>& heightMap, int& width, int& height) {
    int channels;
    stbi_set_flip_vertically_on_load(true);  // Flip the image vertically if needed
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 1);
    if (!data) {
        std::cerr << "Failed to load height map: " << filename << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return;
    }

    std::cout << "Successfully loaded height map: " << filename << std::endl;
    std::cout << "Image dimensions: " << width << "x" << height << std::endl;
    std::cout << "Channels: " << channels << std::endl;

    heightMap.resize(width * height);
    for (int i = 0; i < width * height; ++i) {
        heightMap[i] = static_cast<float>(data[i]) / 255.0f;
    }

    stbi_image_free(data);
}

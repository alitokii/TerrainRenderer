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

void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, const char* heightMapFile) {
    std::vector<float> heightMap;
    int width, height;
    loadHeightMap(heightMapFile, heightMap, width, height);

    if (heightMap.empty()) {
        std::cerr << "Failed to load height map. Exiting." << std::endl;
        exit(3);
    }

    float heightScale = 20.0f;
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            float y = heightMap[z * width + x] * heightScale;

            // Position
            vertices.push_back(static_cast<float>(x));
            vertices.push_back(y);
            vertices.push_back(static_cast<float>(z));

            // Normal (calculate using central differences)
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            if (x > 0 && x < width - 1 && z > 0 && z < height - 1) {
                float hL = heightMap[z * width + (x - 1)];
                float hR = heightMap[z * width + (x + 1)];
                float hD = heightMap[(z - 1) * width + x];
                float hU = heightMap[(z + 1) * width + x];
                normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
            }
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    // Generate indices
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

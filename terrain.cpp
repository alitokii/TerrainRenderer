#include "terrain.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "perlin.h"
#include <iostream>
#include <cmath>

void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, TerrainMode mode, const char* heightMapFile)
{
    // Sets dimensions for the terrains
    int width = 200, height = 200;
    // Creates vector for height map data
    std::vector<float> heightMap;
    PerlinNoise pn;

    // Error checking for height map file
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

    // Height and Noise scales for the terrain
    float heightScale = 50.0f;  // Increase for more pronounced terrain
    float noiseScale = 0.03f;   // Reduce for smoother terrain

    // Generates terrain vertices and normals
    for (int z = 0; z < height; z++)
    {
        // Calculates the height of the terrain
        for (int x = 0; x < width; x++)
        {
            float y;
            if (mode == TerrainMode::HEIGHTMAP_IMAGE) {
                y = heightMap[z * width + x] * heightScale;
            } else {
                y = octavePerlin(pn, x * noiseScale, z * noiseScale, 6, 0.5) * heightScale;
            }

            // Adds vertex position to vertices vector
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
                normal = calculateNormal(hL, hR, hD, hU);
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
    stbi_set_flip_vertically_on_load(true);  // Flip the image vertically for correct loading

    // Loads height map image
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 1);

    // Error checking for height map image
    if (!data) {
        std::cerr << "Failed to load height map: " << filename << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return;
    }

    // Logging
    std::cout << "Successfully loaded height map: " << filename << std::endl;
    std::cout << "Image dimensions: " << width << "x" << height << std::endl;
    std::cout << "Channels: " << channels << std::endl;

    // Fills height map vector with image data by resizing to fit
    heightMap.resize(width * height);
    for (int i = 0; i < width * height; ++i) {
        heightMap[i] = static_cast<float>(data[i]) / 255.0f;
    }

    stbi_image_free(data);
}

glm::vec3 calculateNormal(float hL, float hR, float hD, float hU) {
    return glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
}

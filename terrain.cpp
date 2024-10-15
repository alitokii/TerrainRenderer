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

void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, int width, int height)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-1.0, 1.0);

    for (int z = 0; z < height; z++)
    {
        for (int x = 0; x < width; x++)
        {
            float y = sin(x * 0.1) * cos(z * 0.1) * 5.0 + distribution(generator);
            vertices.push_back(static_cast<float>(x));
            vertices.push_back(y);
            vertices.push_back(static_cast<float>(z));

            // Add normal vector (simplified, just pointing up)
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
        }
    }

    // Indices remain the same
    for (int z = 0; z < height - 1; z++)
    {
        for (int x = 0; x < width - 1; x++)
        {
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

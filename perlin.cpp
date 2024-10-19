#include "perlin.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>

// Constructor for PerlinNoise class
PerlinNoise::PerlinNoise()
{
    p.resize(256); // Resize the vector to 256 elements
    std::iota(p.begin(), p.end(), 0); // Fill the vector with consecutive integers
    std::random_device rd; // Used for seed
    std::mt19937 g(rd()); // Mersenne Twister engine for random num generation
    std::shuffle(p.begin(), p.end(), g); // Shuffles the elements inside the vector
    p.insert(p.end(), p.begin(), p.end()); // Copies the elements from the vector to the end of the vector
}

double PerlinNoise::noise(double x, double y, double z)
{
    // Gets the integers of the coordinates
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    // Gets the fractional/non-integers of the coordinates
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    // Calculates interpolation factors
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Calculates indices for the permutation table
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Interpolates the gradient values
    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
                                   grad(p[BA], x - 1, y, z)),
                           lerp(u, grad(p[AB], x, y - 1, z),
                                   grad(p[BB], x - 1, y - 1, z))),
                   lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
                                   grad(p[BA + 1], x - 1, y, z - 1)),
                           lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                                   grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

double PerlinNoise::fade(double t)
{
    // Smooth interpolation
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b)
{
    // Linear interpolation
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z)
{
    // Extracts the lower 4 bits of the hash value
    int h = hash & 15;

    // Determines the gradient vector based on the hash value
    double u = h < 8 ? x : y;  // Uses x if its less than 8, otherwise uses y
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;  // Determines the second component of the gradient vector

    // Calculates the dot product of the gradient vector and the input coordinates
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float octavePerlin(PerlinNoise& pn, float x, float y, int octaves, float persistence) {
    float total = 0;  // Accumulator for the final noise value
    float frequency = 1;  // Frequency of the current octave
    float amplitude = 1;  // Amplitude of the current octave
    float maxValue = 0;  // Max possible amplitude

    // Iterates over the octaves
    for(int i = 0; i < octaves; i++) {
        // Calculates noise value for the current octave
        total += pn.noise(x * frequency, y * frequency, 0.5) * amplitude;

        maxValue += amplitude; // Updates the max possible amplitude
        amplitude *= persistence; // Reduces the amplitude of the next octave
        frequency *= 2; // Increases the frequency of the next octave
    }

    // Normalizes the total noise value by the max possible amplitude
    return total / maxValue;
}

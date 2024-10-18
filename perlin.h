#pragma once

#include <vector>

class PerlinNoise {
public:
    PerlinNoise();
    double noise(double x, double y, double z);

private:
    std::vector<int> p;

    static double fade(double t);
    static double lerp(double t, double a, double b);
    static double grad(int hash, double x, double y, double z);
};

// Function declaration for octave Perlin noise
float octavePerlin(PerlinNoise& pn, float x, float y, int octaves, float persistence);

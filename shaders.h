#pragma once
#include <GL/glew.h> // for OpenGL types like GLuint

// Main shader sources
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

// Function declarations
GLuint createShaderProgram();

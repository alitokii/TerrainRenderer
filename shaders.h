#pragma once
#include <GL/glew.h> // for OpenGL types like GLuint

// Normal shader sources
extern const char* normalVertexShaderSource;
extern const char* normalGeometryShaderSource;
extern const char* normalFragmentShaderSource;

// Main shader sources
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

// Optional: Function declarations for shader-related operations
GLuint createShaderProgram();
GLuint createNormalShaderProgram();

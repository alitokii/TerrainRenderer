#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "terrain.h"

// Function declarations
GLFWwindow* initializeWindow();
void processInput(GLFWwindow *window, TerrainMode& mode, std::vector<float>& vertices, std::vector<unsigned int>& indices, GLuint& VAO, GLuint& VBO, GLuint& EBO);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

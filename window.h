#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "terrain.h" // Include this if TerrainMode is defined here

// Function declarations
GLFWwindow* initializeWindow();
void processInput(GLFWwindow *window, TerrainMode& mode, std::vector<float>& vertices, std::vector<unsigned int>& indices, GLuint& VAO, GLuint& VBO, GLuint& EBO);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// External variable declarations
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern bool wireframeMode;
extern bool renderNormals;
extern float yaw;
extern float pitch;
extern float lastX, lastY;
extern bool firstMouse;

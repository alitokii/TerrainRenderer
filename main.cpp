#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// My headers
#include "shaders.h"
#include "perlin.h"
#include "window.h"
#include "terrain.h"

// Globals
double lastTime = 0.0;
int nbFrames = 0;
// For lighting
glm::vec3 lightPos(50.0f, 100.0f, 50.0f);  // Adjust as needed
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);    // White light


int main()
{
    // Spawns window
    GLFWwindow* window = initializeWindow();
    if (!window) return -1;

    // Creates shaders
    GLuint shaderProgram = createShaderProgram();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Sets window background color

    // Shader for normals
    GLuint normalShaderProgram = createNormalShaderProgram();
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);    // White light for light source

    // User prompting
    TerrainMode mode;
    char choice;
    std::cout << "Choose terrain generation mode:" << std::endl;
    std::cout << "1. Random generation (Perlin noise)" << std::endl;
    std::cout << "2. Load from image" << std::endl;
    std::cout << "Enter your choice (1 or 2): ";
    std::cin >> choice;

    // Data for terrain generation
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    if (choice == '1') {
        mode = TerrainMode::PERLIN_NOISE;
        generateTerrain(vertices, indices, mode);
    } else if (choice == '2') {
        mode = TerrainMode::HEIGHTMAP_IMAGE;
        generateTerrain(vertices, indices, mode, "/home/alitoki/Documents/Projects/TerrainRenderer/HeightMapIsland.jpg");
    } else {
        std::cerr << "Invalid choice. Exiting." << std::endl;
        return -1;
    }

    // Error checking
    if (vertices.empty() || indices.empty()) {
        std::cerr << "Failed to generate terrain. Exiting." << std::endl;
        return -1;
    }

    // Logging
    std::cout << "Terrain generated successfully." << std::endl;
    std::cout << "Vertices: " << vertices.size() / 6 << std::endl;
    std::cout << "Indices: " << indices.size() << std::endl;

    // Set up vertex data, buffers, and their pointers
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); // Generates 1 VAO object
    glGenBuffers(1, &VBO); // Generates 1 VBO object
    glGenBuffers(1, &EBO); // Generates 1 EBO object

    glBindVertexArray(VAO); // Binds VAO object

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds VBO object
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // Copies vertices to buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Binds EBO object
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); // Copies indices to buffer

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0); // Enables vertex attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal attribute
    glEnableVertexAttribArray(1); // Enables vertex attribute

    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos"); // Gets lightPos uniform location
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos"); // Gets viewPos uniform location

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        nbFrames++; // Counts frames being rendered per second

        // Print FPS every second
        if (currentTime - lastTime >= 1.0) { // Checks if 1 second has passed
            double ms_per_frame = 1000.0 / double(nbFrames); // Calculates time per frame in milliseconds
            double fps = double(nbFrames) / (currentTime - lastTime); // Calculates frames per second
            printf("%.1f ms/frame (%.1f FPS)\n", ms_per_frame, fps); // Prints FPS
            nbFrames = 0; // Resets frame count
            lastTime += 1.0; // Increments lastTime by 1 second
        }

        // Tracks mouse and keyboard input in the window
        processInput(window, mode, vertices, indices, VAO, VBO, EBO);

        // Clear the screen/buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        glm::vec3 lightPos = glm::vec3(50.0f, 200.0f, 50.0f); // Adjust as needed
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // Camera view matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f); // Camera projection matrix
        glm::mat4 model = glm::mat4(1.0f); // Model matrix

        // Matrix uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model)); // Sets model matrix
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view)); // Sets view matrix
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection)); // Sets projection matrix
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos)); // Sets light position uniform
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor)); // Sets light color uniform
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos)); // Sets view position uniform

        glBindVertexArray(VAO); // Binds VAO object
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Draws terrain

        // After rendering the terrain
        if (renderNormals)
        {
            glUseProgram(normalShaderProgram);

            // Matrix uniforms
            glUniformMatrix4fv(glGetUniformLocation(normalShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(normalShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(normalShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean-up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(normalShaderProgram);

    // End program
    glfwTerminate();
    return 0;
}

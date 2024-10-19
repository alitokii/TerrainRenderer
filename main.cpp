#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

// For camera, globals
glm::vec3 cameraPos = glm::vec3(50.0f, 100.0f, 150.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool wireframeMode = false;

// Function declarations
GLFWwindow* initializeWindow();

// FPS
double lastTime = 0.0;
int nbFrames = 0;

// For camera
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;


int main()
{
    GLFWwindow* window = initializeWindow();
    if (!window) return -1;

    GLuint shaderProgram = createShaderProgram();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set background color

    // Generate terrain data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateTerrain(vertices, indices,"/home/alitoki/Documents/Skyline_NavAI_Challenge/TerrainRenderer/HeightMapIsland.jpg");

    // Terrain error checking
    if (vertices.empty() || indices.empty()) {
            std::cerr << "Failed to generate terrain. Exiting." << std::endl;
            return -1;
        }

        // Logging
        std::cout << "Terrain generated successfully." << std::endl;
        std::cout << "Vertices: " << vertices.size() / 6 << std::endl;
        std::cout << "Indices: " << indices.size() << std::endl;

    // Create and bind VAO and VBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    // Main rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // 1. Render depth of scene to texture (from light's perspective)
        glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        glUseProgram(shadowShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(shadowShaderProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Render scene as normal with shadow mapping
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Set up view and projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Set uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));

        // Bind shadow map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

        // Render the scene
        renderScene(shaderProgram);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Calculate and print FPS
        nbFrames++;
        if (currentFrame - lastTime >= 1.0) {
            printf("%f ms/frame (%.1f FPS)\n", 1000.0 / double(nbFrames), double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void renderScene(GLuint shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Bind your VAO
    glBindVertexArray(VAO);

    // Draw your terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

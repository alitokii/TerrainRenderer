#include "window.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

// Define external variables
glm::vec3 cameraPos(50.0f, 150.0f, 200.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
bool wireframeMode = false;
bool renderNormals = false;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;

GLFWwindow* initializeWindow()
{
    // Error checking
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Set OpenGL version and core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Terrain Renderer", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    return window;
}

void processInput(GLFWwindow *window, TerrainMode& mode, std::vector<float>& vertices, std::vector<unsigned int>& indices, GLuint& VAO, GLuint& VBO, GLuint& EBO)
{
    float cameraSpeed = 0.9f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;

    // Wireframe toggle
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        static double lastToggleTime = 0.0;
        double currentTime = glfwGetTime();
        if (currentTime - lastToggleTime > 0.2) {
            wireframeMode = !wireframeMode;
            if(wireframeMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            lastToggleTime = currentTime;
        }
    }

    // face normals toggle
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        static double lastToggleTime = 0.0;
        double currentTime = glfwGetTime();
        if (currentTime - lastToggleTime > 0.2) {
            renderNormals = !renderNormals;
            std::cout << "Normals rendering " << (renderNormals ? "enabled" : "disabled") << std::endl;
            lastToggleTime = currentTime;
        }
    }

    // Toggle terrain mode
    static bool tKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!tKeyPressed) {
            tKeyPressed = true;
            if (mode == TerrainMode::PERLIN_NOISE) {
                mode = TerrainMode::HEIGHTMAP_IMAGE;
                std::cout << "Switched to height map mode. Enter image path: ";
                std::string filename;
                std::cin >> filename;
                vertices.clear();
                indices.clear();
                generateTerrain(vertices, indices, mode, filename.c_str());
            } else {
                mode = TerrainMode::PERLIN_NOISE;
                vertices.clear();
                indices.clear();
                generateTerrain(vertices, indices, mode);
            }

            // Rebind the vertex and index buffers
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            // Reset vertex attribute pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
    } else {
        tKeyPressed = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    (void)window; // suppresses unused parameter warning
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

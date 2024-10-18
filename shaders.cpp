#include "shaders.h"
#include <GL/glew.h>

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    out vec3 FragPos;
    out vec3 Normal;
    out float Height;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        Height = aPos.y;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragPos;
    in vec3 Normal;
    in float Height;
    out vec4 FragColor;
    uniform vec3 lightPos;
    uniform vec3 viewPos;

    vec3 heatmapColor(float t) {
        vec3 r = vec3(1.0, 0.0, 0.0);
        vec3 y = vec3(1.0, 1.0, 0.0);
        vec3 g = vec3(0.0, 1.0, 0.0);
        vec3 c = vec3(0.0, 1.0, 1.0);
        vec3 b = vec3(0.0, 0.0, 1.0);

        if (t < 0.25) return mix(b, c, t * 4.0);
        else if (t < 0.5) return mix(c, g, (t - 0.25) * 4.0);
        else if (t < 0.75) return mix(g, y, (t - 0.5) * 4.0);
        else return mix(y, r, (t - 0.75) * 4.0);
    }

    void main()
    {
        // Normalize height to [0, 1] range
        float normalizedHeight = (Height + 10.0) / 20.0; // Adjust these values based on your terrain's height range
        vec3 color = heatmapColor(normalizedHeight);

        // Ambient light
        vec3 ambient = 0.3 * color;

        // Diffuse light
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * color;

        // Specular light
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = vec3(0.2) * spec;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
)";

GLuint createShaderProgram()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

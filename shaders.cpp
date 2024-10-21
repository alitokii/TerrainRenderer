#include "shaders.h"
#include <GL/glew.h>

// Shader sources
const char* normalVertexShaderSource = R"(
    #version 330 core

    // Input vertex attributes
    layout (location = 0) in vec3 aPos;  // Position of the vertex
    layout (location = 1) in vec3 aNormal; // Normal vector of the vertex

    // Output vertex attribute
    out vec3 Normal; // Normal vector to be passed to the fragment shader

    // Uniform variables
    uniform mat4 model;       // Model matrix for transforming vertex positions
    uniform mat4 view;        // View matrix for transforming world coordinates to camera coordinates
    uniform mat4 projection;  // Projection matrix for projecting 3D coordinates to 2D screen coordinates

    void main()
    {
        // Calculate the transformed vertex position in clip space
        gl_Position = projection * view * model * vec4(aPos, 1.0);

        // Calculate the transformed normal vector
        Normal = mat3(transpose(inverse(model))) * aNormal;
    }
)";

const char* normalGeometryShaderSource = R"(
    #version 330 core

    // Input type triangles
    layout (triangles) in;

    // Output type line strip
    layout (line_strip, max_vertices = 6) out;

    // Input vertex attribute
    in vec3 Normal[];  // Array of normal vectors for the input triangle

    const float MAGNITUDE = 0.4;  // Length of the normal lines

    // Generates a line segment representing the normal at a vertex
    void GenerateLine(int index)
    {
        // Emit the first vertex of the line
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();

        // Emit the second vertex of the line
        gl_Position = gl_in[index].gl_Position + vec4(Normal[index], 0.0) * MAGNITUDE;
        EmitVertex();

        // End the line primitive
        EndPrimitive();
    }

    void main()
    {
        // Generate lines for each vertex of the input triangle
        GenerateLine(0); // First vertex normal
        GenerateLine(1); // Second vertex normal
        GenerateLine(2); // Third vertex normal
    }
)";

const char* normalFragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Yellow color for normals
    }
)";

const char* vertexShaderSource = R"(
    #version 330 core

    // Input vertex attributes
    layout (location = 0) in vec3 aPos;  // Position of the vertex
    layout (location = 1) in vec3 aNormal; // Normal vector of vertex

    // Output vertex attributes
    out vec3 FragPos;  // Fragmented position for fragment shader
    out vec3 Normal;   // Normal vector for fragment shader
    out float Height;  // Height of the vertex

    // Uniform variables
    uniform mat4 model;       // Model matrix for transforming vertex positions
    uniform mat4 view;        // View matrix for transforming world coordinates to camera coordinates
    uniform mat4 projection;  // Projection matrix for projecting 3D coordinates to 2D screen coordinates

    void main()
    {
        // Calculates transformed vertex position in world space
        FragPos = vec3(model * vec4(aPos, 1.0));

        // Calculates transformed normal vector
        Normal = mat3(transpose(inverse(model))) * aNormal;

        // Extracts the height from the vertex position (y coordinate)
        Height = aPos.y;

        // Calculate the final vertex position in clip space
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core

    // Input attributes
    in vec3 FragPos;  // Fragmented position
    in vec3 Normal;   // Normal vector
    in float Height;  // Height of the fragment

    // Output attribute
    out vec4 FragColor; // Color of fragment

    // Uniform variables
    uniform vec3 lightPos;  // Position of light source
    uniform vec3 lightColor; // Color of light source

    // Calculates the color based on the height
    vec3 heatmapColor(float t) {
        // Color values for different heights
        vec3 r = vec3(1.0, 0.0, 0.0); // Red
        vec3 y = vec3(1.0, 1.0, 0.0); // Yellow
        vec3 g = vec3(0.0, 1.0, 0.0); // Green
        vec3 c = vec3(0.0, 1.0, 1.0); // Cyan
        vec3 b = vec3(0.0, 0.0, 1.0); // Blue

        // Interpolate colors based on the normalized height
        if (t < 0.25) return mix(b, c, t * 4.0);
        else if (t < 0.5) return mix(c, g, (t - 0.25) * 4.0);
        else if (t < 0.75) return mix(g, y, (t - 0.5) * 4.0);
        else return mix(y, r, (t - 0.75) * 4.0);
    }

    void main()
    {
        // Normalizes the height to a [0, 1] range
        float normalizedHeight = (Height + 25.0) / 50.0; // Adjust based on terrain's height range

        // Calculate the base color for the heatmap
        vec3 baseColor = heatmapColor(normalizedHeight);

        // Ambient lighting
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // Diffuse lighting
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // Combines ambient and diffuse lighting with the base color
        vec3 result = (ambient + diffuse) * baseColor;

        // Sets the fragment color
        FragColor = vec4(result, 1.0);
    }
)";

// Shader functions
GLuint createShaderProgram()
{
    // Creates vertex shader object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Sets the source code for the vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // Compiles the vertex shader
    glCompileShader(vertexShader);

    // Creates a fragment shader object
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Sets the source code for the fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // Compiles the fragment shader
    glCompileShader(fragmentShader);

    // Creates a shader program object
    GLuint shaderProgram = glCreateProgram();

    // Attaches the vertex and fragment shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Links the shader program
    glLinkProgram(shaderProgram);

    // Deletes the vertex and fragment shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createNormalShaderProgram()
{
    // Creates a vertex shader object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Sets the source code for the vertex shader
    glShaderSource(vertexShader, 1, &normalVertexShaderSource, NULL);

    // Compiles the vertex shader
    glCompileShader(vertexShader);

    // Creates a geometry shader object
    GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    // Sets the source code for the geometry shader
    glShaderSource(geometryShader, 1, &normalGeometryShaderSource, NULL);

    // Compiles the geometry shader
    glCompileShader(geometryShader);

    // Creates a fragment shader object
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Sets the source code for the fragment shader
    glShaderSource(fragmentShader, 1, &normalFragmentShaderSource, NULL);

    // Compiles the fragment shader
    glCompileShader(fragmentShader);

    // Creates a shader program object
    GLuint shaderProgram = glCreateProgram();

    // Attaches the vertex, geometry, and fragment shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Links the shader program
    glLinkProgram(shaderProgram);

    // Deletes the vertex, geometry, and fragment shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

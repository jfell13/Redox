#include "Renderer.h"
#include "Objects.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

bool Renderer::initialize(
    
) {
    
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "gl_Position =\n"
        "projection *\n"
        "view *\n"
        "model *\n"
        "vec4(aPos, 1.0);\n"
        "}\n";

    const char* fragmentShaderSource = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 objectColor;\n"
        "void main() {\n"
        "FragColor = vec4(objectColor, 1.0);\n"
        "}\n";
    
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        nullptr
    );

    glCompileShader(vertexShader);

    GLint success = GL_FALSE;
    char infoLog[1024];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(
            vertexShader,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cerr << "Vertex shader error:\n" << infoLog << '\n';

        glDeleteShader(vertexShader);
        return false;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        nullptr
    );

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(
            fragmentShader,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cerr << "Fragment shader error:\n" << infoLog << '\n';

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Create and link shader program
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    // Individual shaders are no longer needed after linking.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success) {
        glGetProgramInfoLog(
            shaderProgram,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cerr << "Shader linking error:\n" << infoLog << '\n';

        glDeleteProgram(shaderProgram);
        shaderProgram = 0;

        return false;
    }

    // Retrieve uniform locations
    colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Wireframe cube spanning [0, boxLength] on each axis.
    glm::vec3 vertices[] = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 1.0f}
    };

    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    generateSphere(
        1.0f,
        32,
        16,
        sphereVertices,
        sphereIndices
    );

    sphereIndexCount = static_cast<GLsizei>(
        sphereIndices.size()
    );

    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);

    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sphereVertices.size() * sizeof(float),
        sphereVertices.data(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &sphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sphereIndices.size() * sizeof(unsigned int),
        sphereIndices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr
    );

    glEnableVertexAttribArray(0);
    // Twelve edges of the cube.
    const unsigned int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    glGenVertexArrays(1, &boxVAO);
    glBindVertexArray(boxVAO);

    glGenBuffers(1, &boxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &boxEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        nullptr
    );

    glEnableVertexAttribArray(0);

    // Ground spanning -50 to +50 along X and Y, at Z = 0.
    constexpr float extent = 50.0f;
    const float planeVertices[] = {
        -extent, -extent, 0.0f,
        extent, -extent, 0.0f,
        extent,  extent, 0.0f,

        -extent, -extent, 0.0f,
        extent,  extent, 0.0f,
        -extent,  extent, 0.0f
    };

    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);

    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(planeVertices),
        planeVertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), nullptr
    );
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::cleanup() {
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteBuffers(1, &boxVBO);
    glDeleteBuffers(1, &boxEBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);

    boxVAO = boxVBO = boxEBO = 0;
    sphereVAO = sphereVBO = sphereEBO = 0;
    shaderProgram = 0;
    sphereIndexCount = 0;

    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    planeVAO = planeVBO = 0;
}

void Renderer::drawXYPlane() {
    const glm::mat4 model(1.0f);

    glUniformMatrix4fv(
        modelLoc, 1, GL_FALSE, glm::value_ptr(model)
    );
    glUniform3f(colorLoc, 0.25f, 0.30f, 0.25f);

    glBindVertexArray(planeVAO);

    // Offset the filled surface slightly in depth so the cube's
    // bottom edges at Z = 0 remain visible without flickering.
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Renderer::drawBoxAt(const glm::vec3& position, const glm::vec3& size) {
    // glm::vec3 position(10.0f, 5.0f, 0.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, size);
    glUniformMatrix4fv(
        modelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );
    glUniform3f(
        colorLoc,
        1.0f,
        1.0f,
        1.0f
    );
    glBindVertexArray(boxVAO);
    glDrawElements(
        GL_LINES,
        24,
        GL_UNSIGNED_INT,
        nullptr
    );
}

Renderer::BoxDraw Renderer::drawBox(const glm::vec3& position, const glm::vec3& size) {
    drawBoxAt(position, size);
    return BoxDraw(*this, position, size);

}

Renderer::BoxDraw& Renderer::BoxDraw::drawStairs() {
    constexpr int stepCount = 20;
    const float width = parentSize.x * 0.25f;
    const float run = parentSize.y / stepCount;
    const float rise = parentSize.z / stepCount;

    for (int i = 0; i < stepCount; ++i) {
        const glm::vec3 position = parentPosition + glm::vec3(
            (parentSize.x - width) * 0.5f,
            i * run,
            0.0f
        );

        // Each successive box is taller, producing ascending treads.
        const glm::vec3 size(width, run, (i + 1) * rise);
        renderer.drawBoxAt(position, size);
    }

    return *this;
}

void Renderer::beginFrame(
    const glm::mat4& view,
    const glm::mat4& projection
) {
    glClearColor(
        0.05f,
        0.07f,
        0.12f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(
        viewLoc,
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        projectionLoc,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );
}

void Renderer::drawParticle(
    const glm::vec3& position,
    float radius,
    const glm::vec3& color
) {
    glm::mat4 particleModel(1.0f);

    particleModel = glm::translate(
        particleModel,
        position
    );

    particleModel = glm::scale(
        particleModel,
        glm::vec3(radius)
    );

    glUniformMatrix4fv(
        modelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(particleModel)
    );

    glUniform3f(
        colorLoc,
        color.r,
        color.g,
        color.b
    );

    glBindVertexArray(sphereVAO);

    glDrawElements(
        GL_TRIANGLES,
        sphereIndexCount,
        GL_UNSIGNED_INT,
        nullptr
    );
}

void generateSphere(
    float radius,
    unsigned int sectors,
    unsigned int stacks,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
) {
    for (unsigned int i = 0; i <= stacks; i++) {
        float theta = PI * static_cast<float>(i) / stacks;
        for (unsigned int j = 0; j <= sectors; j++) {
            float phi = 2.0f * PI * static_cast<float>(j) / sectors;
            float x = radius * std::sin(theta) * std::cos(phi);
            float y = radius * std::cos(theta);
            float z = radius * std::sin(theta) * std::sin(phi);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (unsigned int i = 0; i < stacks; i++) {
        for (unsigned int j = 0; j < sectors; j++) {
            unsigned int first = i * (sectors + 1) + j;
            unsigned int second = first + sectors + 1;
            if (i != 0) {
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);
            }
            if (i != stacks - 1) {
                indices.push_back(first + 1);
                indices.push_back(second);
                indices.push_back(second + 1);
            }
        }
    }
}

void Renderer::drawModel(const Model& model, const glm::mat4& transform,
                         const glm::vec3& color) {
    if (!model.vao) return;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
    glBindVertexArray(model.vao);
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
}

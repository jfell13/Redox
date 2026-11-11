#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <glm/glm.hpp>
#include <glad/gl.h>

// Static mesh data uploaded to OpenGL. Load/cleanup require a current context.
class Model {
public:
    Model() = default;
    ~Model() { cleanup(); }
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    bool load(const std::string& path);
    void cleanup();
    const glm::vec3& minimum() const { return boundsMin; }
    const glm::vec3& maximum() const { return boundsMax; }
    const std::string& error() const { return errorMessage; }

private:
    friend class Renderer;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
    glm::vec3 boundsMin{0.0f};
    glm::vec3 boundsMax{0.0f};
    std::string errorMessage;
};
#endif

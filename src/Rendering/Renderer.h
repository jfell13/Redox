#ifndef RENDERER_H
#define RENDERER_H
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

constexpr float PI = 3.14159265358979323846f;

class Model;

class Renderer {
public:
    bool initialize(
    );
    void beginFrame(
        const glm::mat4& view,
        const glm::mat4& projection
    );
    void drawXYPlane();
    void drawModel(const Model& model, const glm::mat4& transform,
                   const glm::vec3& color);
    class BoxDraw{
        public:
            BoxDraw(
                Renderer& renderer, 
                const glm::vec3 position,
                const glm::vec3 size
            )
            : renderer(renderer), 
            parentPosition(position),
            parentSize(size) {}
             BoxDraw& drawStairs(
                // const glm::vec3& offset,
                // float size
            );
        private:
            Renderer& renderer;
            glm::vec3 parentPosition;
            glm::vec3 parentSize;
    };
    BoxDraw drawBox(const glm::vec3& position, const glm::vec3& size);
    void drawParticle(
        const glm::vec3& position,
        float radius,
        const glm::vec3& color
    );
    void cleanup();
private:
    GLuint shaderProgram = 0;
    GLuint boxVAO = 0;
    GLuint boxVBO = 0;
    GLuint boxEBO = 0;
    GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
    GLsizei sphereIndexCount = 0;
    GLint modelLoc = -1;
    GLint viewLoc = -1;
    GLint projectionLoc = -1;
    GLint colorLoc = -1;
    GLuint planeVAO = 0;
    GLuint planeVBO = 0;
    void drawBoxAt(const glm::vec3& position, const glm::vec3& size);
};
void generateSphere(
    float radius,
    unsigned int sectors,
    unsigned int stacks,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
);
#endif // RENDERER_H
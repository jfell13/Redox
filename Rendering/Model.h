#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Model Bounding Box based on shape
struct AABB {
    glm::vec3 min{std::numeric_limits<float>::max()};
    glm::vec3 max{std::numeric_limits<float>::lowest()};

    void ExpandToFit(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
};
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

        glm::vec3 GetRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight, 
                          const glm::mat4& view, const glm::mat4& projection);

    private:
        friend class Renderer;
        struct MaterialRange {
            GLint first;
            GLsizei count;
            glm::vec3 diffuse;
            bool hasDiffuse;
        };
        std::vector<MaterialRange> materialRanges;
        GLuint vao = 0;
        GLuint vbo = 0;
        GLsizei vertexCount = 0;
        glm::vec3 boundsMin{0.0f};
        glm::vec3 boundsMax{0.0f};
        std::string errorMessage;
    };

// Clickable Model wrapper 
class ClickableModel {
    public:
        explicit ClickableModel(const Model* existingModel) : model(existingModel) {
            if (model) {
                bounds.min = model->minimum();
                bounds.max = model->maximum();
            }
        }
        glm::mat4 transform{1.0f};

        bool isSelected = false;

        // void OnClick() {
        //     isSelected = !isSelected;
            // Trigger highlight shader, UI popup, etc.
        // }
        bool IsClicked(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& outDistance) const {
            if (!model) return false;
            const glm::mat4 worldToLocal = glm::inverse(transform);
            const glm::vec3 localOrigin(worldToLocal * glm::vec4(rayOrigin, 1.0f));
            const glm::vec3 localDirection(worldToLocal * glm::vec4(rayDir, 0.0f));

            return IntersectRayAABB(localOrigin, localDirection, bounds, outDistance);
        }
    private:
        const Model* model = nullptr;
        AABB bounds;
        static bool IntersectRayAABB(
            const glm::vec3& rayOrigin, 
            const glm::vec3& rayDir, 
            const AABB& box, 
            float& hitDistance
        ) {
            glm::vec3 invDir = 1.0f / rayDir; // IEEE 754 handles division by zero cleanly as +/-inf

            glm::vec3 t0 = (box.min - rayOrigin) * invDir;
            glm::vec3 t1 = (box.max - rayOrigin) * invDir;

            glm::vec3 tmin = glm::min(t0, t1);
            glm::vec3 tmax = glm::max(t0, t1);

            float tNear = std::max({tmin.x, tmin.y, tmin.z});
            float tFar  = std::min({tmax.x, tmax.y, tmax.z});

            if (tNear > tFar || tFar < 0.0f) {
                return false; // Ray misses the box or box is behind the camera
            }

            hitDistance = (tNear >= 0.0f) ? tNear : tFar;
            return true;
        }
};

#endif

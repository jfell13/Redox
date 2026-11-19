#include <Model.h>
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <limits>
#include <vector>

bool Model::load(const std::string& path) {
    errorMessage.clear();
    Assimp::Importer importer;
    // Bake scene-node transforms into static vertices before the importer dies.
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_PreTransformVertices |
        aiProcess_SortByPType | aiProcess_ValidateDataStructure);
    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
        errorMessage = "Cannot load " + path + ": " + importer.GetErrorString();
        return false;
    }

    std::vector<float> positions;
    std::vector<MaterialRange> loadedRanges;
    glm::vec3 minimum(std::numeric_limits<float>::max());
    glm::vec3 maximum(std::numeric_limits<float>::lowest());
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        const std::size_t firstVertex = positions.size() / 3;
        aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
        bool hasDiffuse = false;

        if (mesh->mMaterialIndex < scene->mNumMaterials) {
            const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            hasDiffuse =
                material->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS;

            if (!hasDiffuse) {
                hasDiffuse =
                    material->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS;
            }
        }
        // aiColor3D diffuse(1.0f, 1.0f, 1.0f);
        // const bool hasDiffuse = mesh->mMaterialIndex < scene->mNumMaterials &&
        //     scene->mMaterials[mesh->mMaterialIndex]->Get(
        //         AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS;
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices != 3)
                continue;
            for (unsigned int j = 0; j < 3; ++j) {
                const aiVector3D& v = mesh->mVertices[face.mIndices[j]];
                positions.insert(positions.end(), {v.x, v.y, v.z});
                minimum = glm::min(minimum, glm::vec3(v.x, v.y, v.z));
                maximum = glm::max(maximum, glm::vec3(v.x, v.y, v.z));
            }
        }
        const std::size_t endVertex = positions.size() / 3;
        if (endVertex > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
            errorMessage = "Model is too large: " + path;
            return false;
        }
        if (endVertex > firstVertex) {
            loadedRanges.push_back({
                static_cast<GLint>(firstVertex),
                static_cast<GLsizei>(endVertex - firstVertex),
                // glm::vec3(diffuse.r, diffuse.g, diffuse.b),
                glm::vec3(baseColor.r, baseColor.g, baseColor.b),
                hasDiffuse
            });
        }
        
    }
    if (positions.empty() || positions.size() / 3 >
        static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
        errorMessage = "Model has no triangles or is too large: " + path;
        return false;
    }

    // aiColor3D emissiveColor(0.0f, 0.0f, 0.0f);
    // material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

    cleanup();
    materialRanges = std::move(loadedRanges);
    boundsMin = minimum;
    boundsMax = maximum;
    vertexCount = static_cast<GLsizei>(positions.size() / 3);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
                 positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return true;
}

void Model::cleanup() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    vao = vbo = 0;
    vertexCount = 0;
    materialRanges.clear();
}

glm::vec3 Model::GetRayFromMouse(
    double mouseX, double mouseY, 
    int screenWidth, int screenHeight, 
    const glm::mat4& view, const glm::mat4& projection
) {
    // 1. Convert screen coordinates to Normalized Device Coordinates (-1 to 1)
    float x = (2.0f * static_cast<float>(mouseX)) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenHeight; // Invert Y for OpenGL

    // 2. Homogeneous Clip Coordinates
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);

    // 3. Eye (Camera) Coordinates
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // 4. World Coordinates
    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    return glm::normalize(rayWorld);
}


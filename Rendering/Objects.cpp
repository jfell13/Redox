#include "Objects.h"
#include <assimp/Importer.hpp>
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
    glm::vec3 minimum(std::numeric_limits<float>::max());
    glm::vec3 maximum(std::numeric_limits<float>::lowest());
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
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
    }
    if (positions.empty() || positions.size() / 3 >
        static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
        errorMessage = "Model has no triangles or is too large: " + path;
        return false;
    }

    cleanup();
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
}

#pragma once

#include "Mesh.hpp"
#include "Program.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <functional>
#include <set>
#include <unordered_map>

class Model {
public:
    Model(std::vector<Mesh> &&meshes);
    Model(const std::string &path);
    void Draw(Program &prog, const glm::mat4 &model);

private:
    void processNode(const aiNode *node, const aiScene *scene);
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
    TextureList loadTextures(const aiMaterial *mat, TextureType texType);

    std::string dir; // the directory where this OBJ file is located
    std::vector<Mesh> meshes;
    TextureList loadedTex;
};
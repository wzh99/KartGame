#include "Model.hpp"

Model::Model(std::vector<Mesh> &&meshes) : meshes(std::move(meshes)) {}

Model::Model(const std::string &path) : dir(path.substr(0, path.find_last_of('/'))) {
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        Error("Assimp error: %s", importer.GetErrorString());
    processNode(scene->mRootNode, scene); // process node recursively, beginning at root
}

void Model::Draw(Program &prog, const glm::mat4 &model) {
    prog.Use();
    prog.SetMat4("model", model);
    auto normMat =  glm::mat3(glm::transpose(glm::inverse(model)));
    prog.SetMat3("normMat", normMat);
    for (auto &mesh : meshes) mesh.Draw(prog);
}

void Model::processNode(const aiNode *node, const aiScene *scene) {
    // Process meshes in current node
    for (auto i = 0u; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // Resursively process each of the children
    for (auto i = 0u; i < node->mNumChildren; i++) 
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
    // Mesh data to fill
    VertexList vertices;
    IndexList indices;
    TextureList textures;

    // Process vertices
    for (auto i = 0u; i < mesh->mNumVertices; i++) {
        auto &aiVert = mesh->mVertices[i];
        glm::vec3 pos{aiVert.x, aiVert.y, aiVert.z};
        auto &aiNorm = mesh->mNormals[i];
        glm::vec3 norm{aiNorm.x, aiNorm.y, aiNorm.z};
        auto aiTexCoordPtr = mesh->mTextureCoords[0];
        glm::vec2 texCoord;
        if (aiTexCoordPtr) 
            texCoord = {aiTexCoordPtr[i].x, aiTexCoordPtr[i].y};
        else
            texCoord = {0, 0};
        vertices.push_back({pos, norm, texCoord});
    }

    // Visit each face and retrieve indices
    for (auto i = 0u; i < mesh->mNumFaces; i++) {
        auto &face = mesh->mFaces[i];
        for (auto j = 0u; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process materials
    auto mat = scene->mMaterials[mesh->mMaterialIndex];
    auto diffMaps = loadTextures(mat, TEXTURE_DIFFUSE); // diffuse maps
    textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
    auto specMaps = loadTextures(mat, TEXTURE_SPECULAR); // specular maps
    textures.insert(textures.end(), specMaps.begin(), specMaps.end());

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

static std::unordered_map<TextureType, aiTextureType> texTypeToAi = {
    {TEXTURE_DIFFUSE, aiTextureType_DIFFUSE},
    {TEXTURE_SPECULAR, aiTextureType_SPECULAR},
};

TextureList Model::loadTextures(const aiMaterial *mat, TextureType texType) {
    TextureList textures;
    auto aiTexType = texTypeToAi.at(texType);
    for (auto i = 0u; i < mat->GetTextureCount(aiTexType); i++) {
        aiString aiPathStr;
        mat->GetTexture(aiTexType, i, &aiPathStr);
        auto path = dir + '/' + aiPathStr.C_Str();
        bool loaded = false;
        for (const auto &tex : loadedTex) {
            if (std::strcmp(tex.Path.data(), path.data()) == 0) { // loaded before
                textures.push_back(tex);
                loaded = true;
                break;
            }
        }
        if (loaded) continue;
        auto tex = Texture::FromFile(path.c_str());
        tex.Type = texType;
        textures.push_back(tex);
        loadedTex.push_back(tex);
    }
    return textures;
}
#pragma once

#include "Program.hpp"
#include "Texture.hpp"

struct Vertex {
    glm::vec3 Pos, Norm;
    glm::vec2 TexCoord;
};

using VertexList = std::vector<Vertex>;
using IndexList = std::vector<GLuint>;
using TextureList = std::vector<Texture>;

class Mesh {
public:
    Mesh() {}
    Mesh(VertexList &&vertices, IndexList &&indices, TextureList &&textures);
    void Draw(Program &prog);

protected:
    size_t nFaces;
    GLuint vao, vbo, ebo;
    std::vector<Texture> textures;
};

// Procedurally generate track mesh
Mesh CreateTrack(float inRadius, float outRadius, float straightLen, int nArcSeg, int nStraightSeg);

// Generate terrain
Mesh CreateTerrain(float width, float texScale);
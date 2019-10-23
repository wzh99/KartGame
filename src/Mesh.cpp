#include "Mesh.hpp"
#include <cstring>

Mesh::Mesh(VertexList &&vertices, IndexList &&indices, TextureList &&textures) 
    : nFaces(indices.size() / 3), textures(std::move(textures))
{
    // Generate vertex array and buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Set vertex attribute pointer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Norm));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
    glBindVertexArray(0);
}

void Mesh::Draw(Program &prog) {
    // Bind textures
    GLuint texCount[NUM_TEXTURE_TYPE];
    std::memset(texCount, 0, sizeof(texCount));
    for (auto i = 0u; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // activate texture before binding
        const auto &tex = textures[i];
        auto num = texCount[tex.Type]++;
        auto name = TexTypeStr[tex.Type] + std::to_string(num); // e.g. texDiffuse0
        prog.SetInt(name.c_str(), i);
        glBindTexture(GL_TEXTURE_2D, tex.Id);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, nFaces * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Track shape:
//        x
//     -------
//    /       \ 
//   |         | -> z
//    \       /
//     -------
Mesh CreateTrack(float inRadius, float outRadius, float straightLen, int nArcSeg, int nStraightSeg) {
    // Basic settings (first inner, then outer)
    auto nOneCycle = 2 * (nArcSeg + nStraightSeg);
    VertexList vertices;
    vertices.reserve(2 * nOneCycle);
    float radius[2]{inRadius, outRadius};
    glm::vec3 rCent(0, 0, straightLen / 2), lCent(0, 0, -straightLen / 2);
    auto degStep = 180.f / nArcSeg;
    auto lenStep = straightLen / nStraightSeg;
    glm::vec3 upStep(0, 0, -lenStep), downStep(0, 0, lenStep);
    constexpr glm::vec3 norm(0, 1, 0);

    // Create inner and outer vertices
    for (auto i = 0u; i < 2; i++) {
        glm::vec2 texCoord{i, 0};
        // right arc path
        for (auto j = 0u; j <= nArcSeg; j++) {
            auto rad = glm::radians(-90.f + degStep * j);
            auto pos = rCent + glm::vec3(std::sin(rad), 0, std::cos(rad)) * radius[i];
            vertices.push_back({pos, norm, texCoord});
        }
        // up straight path
        auto lastPos = vertices.back().Pos; // start from last point of arc
        for (auto j = 1u; j <= nStraightSeg - 1; j++) {
            auto pos = lastPos + float(j) * upStep;
            vertices.push_back({pos, norm, texCoord});
        }
        // left arc path
        for (auto j = 0u; j <= nArcSeg; j++) {
            auto rad = glm::radians(90.f + degStep * j);
            auto pos = lCent + glm::vec3(std::sin(rad), 0, std::cos(rad)) * radius[i];
            vertices.push_back({pos, norm, texCoord});
        }
        // down straight path
        lastPos = vertices.back().Pos; // start from last point of arc
        for (auto j = 1u; j <= nStraightSeg - 1; j++) {
            auto pos = lastPos + float(j) * downStep;
            vertices.push_back({pos, norm, texCoord});
        }
    }

    // Create indices
    IndexList indices;
    indices.reserve(6 * nOneCycle);
    for (auto i = 0u; i < nOneCycle; i++) {
        auto in0 = i, in1 = (i + 1) % nOneCycle;
        auto out0 = in0 + nOneCycle, out1 = in1 + nOneCycle;
        indices.insert(indices.end(), {in0, out0, in1, in1, out0, out1});
    }

    return Mesh(std::move(vertices), std::move(indices), {});
}

Mesh CreateTerrain(float width, float texScale) {
    constexpr glm::vec3 norm(0, 1, 0);
    auto posCoord = width / 2.f;
    auto texCoord = width / texScale;
    return Mesh(
        { // vertices
            {{-posCoord, 0, -posCoord}, norm, {0, 0}},
            {{-posCoord, 0, posCoord}, norm, {texCoord, 0}},
            {{posCoord, 0, -posCoord}, norm, {0, texCoord}},
            {{posCoord, 0, posCoord}, norm, {texCoord, texCoord}}
        },
        {0, 1, 2, 2, 1, 3}, // indices
        {Texture::FromFile("../asset/mud.jpg")} // texture
    );
}
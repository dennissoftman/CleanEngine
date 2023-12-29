#ifndef MESH_PRIMITIVE_HPP
#define MESH_PRIMITIVE_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>


enum class VertexType
{
    eTexture,
    eColor
};

class VertexPrimitive
{
public:
    VertexPrimitive() = default;
    VertexPrimitive(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex);
    VertexPrimitive(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& color);
    ~VertexPrimitive() = default;

    const glm::vec3& position() const;
    const glm::vec3& normal() const;
    const glm::vec2& texCoord() const;

    const void* data() const;
    size_t size() const;
private:
    // struct begin
    glm::vec3 m_position;
    glm::vec3 m_normal;
    union
    {
        glm::vec2 m_texCoord;
        glm::vec3 m_color;
    };
    // struct end
    VertexType m_type;
};

class MeshPrimitive
{
public:
    MeshPrimitive() = default;
    MeshPrimitive(const std::string& name, uint32_t id=0);
    MeshPrimitive(uint32_t id);
    ~MeshPrimitive() = default;

    void setId(uint32_t id);
    uint32_t id() const;

    void setName(const std::string& name);
    const std::string& name() const;

    void addVertex(const VertexPrimitive& vertex);
    void addIndex(unsigned int index);

    size_t vertexCount() const;
    const std::vector<char> vertexData() const;

    size_t indexCount() const;
    const std::vector<char> indexData() const;
private:
    uint32_t m_id; // meshId in model
    std::string m_name;
    std::vector<VertexPrimitive> m_vertices;
    std::vector<uint32_t> m_indices;
};

#endif
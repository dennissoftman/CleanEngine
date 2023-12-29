#include <memory>

#include "common/3d/meshprimitive.hpp"


VertexPrimitive::VertexPrimitive(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoord)
    : m_position(position), m_normal(normal), m_texCoord(texCoord), m_type(VertexType::eTexture)
{}

VertexPrimitive::VertexPrimitive(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& color)
    : m_position(position), m_normal(normal), m_color(color), m_type(VertexType::eColor)
{}

const glm::vec3& VertexPrimitive::position() const
{
    return m_position;
}

const glm::vec3& VertexPrimitive::normal() const
{
    return m_normal;
}

const glm::vec2& VertexPrimitive::texCoord() const
{
    return m_texCoord;
}

const void* VertexPrimitive::data() const
{
    return &m_position;
}

size_t VertexPrimitive::size() const
{
    if (m_type == VertexType::eTexture)
        return sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);
    else if (m_type == VertexType::eColor)
        return sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec3);
    return sizeof(glm::vec3) + sizeof(glm::vec3);
}

MeshPrimitive::MeshPrimitive(const std::string& name, uint32_t id)
    : m_id(id), m_name(name)
{}

MeshPrimitive::MeshPrimitive(uint32_t id)
    : m_id(id)
{}

void MeshPrimitive::setId(uint32_t id)
{
    m_id = id;
}

uint32_t MeshPrimitive::id() const
{
    return m_id;
}

void MeshPrimitive::setName(const std::string& name)
{
    m_name = name;
}

const std::string& MeshPrimitive::name() const
{
    return m_name;
}

void MeshPrimitive::addVertex(const VertexPrimitive& vertex)
{
    m_vertices.push_back(vertex);
}

size_t MeshPrimitive::vertexCount() const
{
    return m_vertices.size();
}

const std::vector<char> MeshPrimitive::vertexData() const
{
    std::vector<char> dataBuffer(m_vertices.size() * (sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(uint32_t)));

    uint32_t meshId = m_id;

    size_t offset = 0;
    for (const auto& vertex : m_vertices)
    {
        memcpy(dataBuffer.data() + offset, vertex.data(), vertex.size());
        memcpy(dataBuffer.data() + offset + vertex.size(), &meshId, sizeof(uint32_t));
        offset += vertex.size() + sizeof(uint32_t);
    }
    return dataBuffer;
}

void MeshPrimitive::addIndex(unsigned int index)
{
    m_indices.push_back(index);
}

size_t MeshPrimitive::indexCount() const
{
    return m_indices.size();
}

const std::vector<char> MeshPrimitive::indexData() const
{
    std::vector<char> data(m_indices.size() * sizeof(uint32_t));
    memcpy(data.data(), m_indices.data(), data.size());
    return data;
}
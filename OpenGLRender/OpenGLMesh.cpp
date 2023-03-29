#include "OpenGLMesh.h"
#include "BoLog.h"
#include "BoUtil.h"

namespace OpenGLRender {

namespace {

bool isVertexArrayCurrent(GLuint vao) {
    GLint current = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
    return current == vao;
}

bool isVertexBufferCurrent(GLuint vbo) {
    GLint current = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current);
    return current == vbo;
}

bool isIndexBufferCurrent(GLuint ibo) {
    GLint current = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current);
    return current == ibo;
}

} // namespace

Mesh::Mesh() { initData(); }

void Mesh::initData() {
    GLCall(glGenVertexArrays(1, &m_vao));
    if (0 == m_vao) {
        BO_INFO("Failed to generate VAO");
    }

    GLCall(glGenBuffers(1, &m_vbo));
    if (0 == m_vbo) {
        BO_INFO("Failed to generate VBO");
    }

    GLCall(glGenBuffers(1, &m_ibo));
    if (0 == m_ibo) {
        BO_INFO("Failed to generate IBO");
    }
}

int Mesh::vertexCount() const { return m_vertexCount; }

int Mesh::indexCount() const { return m_indexCount; }

void Mesh::writeVertexData(size_t byteSize, size_t count,
                           const void *vertexData) {
    GLCall(glBindVertexArray(m_vao));
    if (!isVertexArrayCurrent(m_vao)) {
        BO_ERROR("Failed to bind VAO");
    }

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    if (!isVertexBufferCurrent(m_vbo)) {
        BO_ERROR("Failed to bind VBO");
    }

    assert(count <= UINT32_MAX);

    m_vertexCount = static_cast<uint32_t>(count);
    GLCall(glBufferData(GL_ARRAY_BUFFER, byteSize, vertexData, GL_STATIC_DRAW));

    GLCall(glBindVertexArray(0)); // VAO before VBO
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Mesh::readVertexData(size_t byteCount, void *data) {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    if (!isVertexBufferCurrent(m_vbo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glGetBufferSubData(GL_ARRAY_BUFFER, 0, byteCount, data));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Mesh::writeIndexData(size_t byteSize, size_t count,
                          const void *indexData) {
    GLCall(glBindVertexArray(m_vao));
    if (!isVertexArrayCurrent(m_vao)) {
        BO_ERROR("Failed to bind VAO");
    }

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    if (!isIndexBufferCurrent(m_ibo)) {
        BO_INFO("Failed to bind IBO");
    }

    assert(count <= UINT32_MAX);

    m_indexCount = static_cast<uint32_t>(count);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteSize, indexData, GL_STATIC_DRAW);

    GLCall(glBindVertexArray(0)); // VAO before VBO
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::readIndexData(int byteCount, void *data) {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    if (!isIndexBufferCurrent(m_ibo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, byteCount, data));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::setAttributeDefinition(int index, int tupleSize, int stride,
                                  int64_t offset, GLenum type) {
    GLCall(glBindVertexArray(m_vao));
    if (!isVertexArrayCurrent(m_vao)) {
        BO_ERROR("Failed to bind VAO");
    }

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    if (!isVertexBufferCurrent(m_vbo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    if (!isIndexBufferCurrent(m_ibo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glEnableVertexAttribArray(index));
    GLCall(glVertexAttribPointer(index, tupleSize, type, GL_FALSE, stride,
                                 (const GLvoid *)(offset)));

    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::bind() {
    GLCall(glBindVertexArray(m_vao));
    if (!isVertexArrayCurrent(m_vao)) {
        BO_ERROR("Failed to bind VAO");
    }
}

void Mesh::release() { GLCall(glBindVertexArray(0)); }

void Mesh::render(GLenum drawStyle, GLenum indexDataType) {
    GLCall(glDrawElements(drawStyle, m_indexCount, indexDataType, 0));
}

void Mesh::destroyData() {
    GLCall(glDeleteBuffers(1, &m_ibo));
    GLCall(glDeleteBuffers(1, &m_vbo));
    GLCall(glDeleteVertexArrays(1, &m_vao));
}

Mesh::~Mesh() { destroyData(); }

} // namespace OpenGLRender

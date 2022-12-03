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

struct Mesh::Data {
    Data() { createData(); }

    ~Data() { destoryData(); }

    void createData() {
        GLCall(glGenVertexArrays(1, &vao));
        if (0 == vao) {
            BO_INFO("Failed to generate VAO");
        }

        GLCall(glGenBuffers(1, &vbo));
        if (0 == vbo) {
            BO_INFO("Failed to generate VBO");
        }

        GLCall(glGenBuffers(1, &ibo));
        if (0 == ibo) {
            BO_INFO("Failed to generate IBO");
        }
    }

    void destoryData() {
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint vao = 0;
    GLuint indexCount = 0;
    GLuint vertexCount = 0;
};

Mesh::Mesh() : m_data(std::make_shared<Data>()) {}

int Mesh::vertexCount() const { return m_data->vertexCount; }

int Mesh::indexCount() const { return m_data->indexCount; }

void Mesh::writeVertexData(int byteSize, int count, const void *vertexData) {
    GLCall(glBindVertexArray(m_data->vao));
    if (!isVertexArrayCurrent(m_data->vao)) {
        BO_ERROR("Failed to bind VAO");
    }

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_data->vbo));
    if (!isVertexBufferCurrent(m_data->vbo)) {
        BO_ERROR("Failed to bind VBO");
    }

    m_data->vertexCount = count;
    GLCall(glBufferData(GL_ARRAY_BUFFER, byteSize, vertexData, GL_STATIC_DRAW));

    GLCall(glBindVertexArray(0)); // VAO before VBO
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Mesh::readVertexData(int byteCount, void *data) {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_data->vbo));
    if (!isVertexBufferCurrent(m_data->vbo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glGetBufferSubData(GL_ARRAY_BUFFER, 0, byteCount, data));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Mesh::writeIndexData(int byteSize, int count, const void *indexData) {
    GLCall(glBindVertexArray(m_data->vao));
    if (!isVertexArrayCurrent(m_data->vao)) {
        BO_ERROR("Failed to bind VAO");
    }

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->ibo));
    if (!isIndexBufferCurrent(m_data->ibo)) {
        BO_INFO("Failed to bind IBO");
    }

    m_data->indexCount = count;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteSize, indexData, GL_STATIC_DRAW);

    GLCall(glBindVertexArray(0)); // VAO before VBO
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::readIndexData(int byteCount, void *data) {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->ibo));
    if (!isIndexBufferCurrent(m_data->ibo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, byteCount, data));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::setAttributeDefinition(int index, int tupleSize, int stride,
                                  int offset, GLenum type) {
    GLCall(glBindVertexArray(m_data->vao));
    if (!isVertexArrayCurrent(m_data->vao)) {
        BO_ERROR("Failed to bind VAO");
    }

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_data->vbo));
    if (!isVertexBufferCurrent(m_data->vbo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->ibo));
    if (!isIndexBufferCurrent(m_data->ibo)) {
        BO_ERROR("Failed to bind VBO");
    }

    GLCall(glEnableVertexAttribArray(index));
    GLCall(glVertexAttribPointer(index, tupleSize, type, GL_FALSE, stride,
                                 (const GLvoid *)offset));

    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Mesh::bind() {
    GLCall(glBindVertexArray(m_data->vao));
    if (!isVertexArrayCurrent(m_data->vao)) {
        BO_ERROR("Failed to bind VAO");
    }
}

void Mesh::release() { GLCall(glBindVertexArray(0)); }

void Mesh::render(GLenum drawStyle, GLenum indexDataType) {
    GLCall(glDrawElements(drawStyle, m_data->indexCount, indexDataType, 0));
}

} // namespace OpenGLRender

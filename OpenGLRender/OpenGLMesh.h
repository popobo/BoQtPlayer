#pragma once

#include "glad/glad.h"
#include <memory>
#include <vector>

namespace OpenGLRender {
class Mesh {
  public:
    Mesh();

    ~Mesh();

    void initData();

    void destroyData();

    template <typename T>
    void writeVertexData(const std::vector<T> &vertexData) {
        writeVertexData(vertexData.size() * sizeof(T), vertexData.size(),
                        vertexData.data());
    }

    void writeVertexData(size_t byteSize, size_t count, const void *vertexData);

    template <typename T> void readVertexData(std::vector<T> &vertexData) {
        vertexData.resize(vertexCount());
        readVertexData(vertexData.size() * sizeof(T), vertexData.data());
    }

    void readVertexData(size_t byteCount, void *data);

    int vertexCount() const;

    template <typename T> void writeIndexData(const std::vector<T> &indexData) {
        writeIndexData(indexData.size() * sizeof(T), indexData.size(),
                       indexData.data());
    }

    void writeIndexData(size_t byteSize, size_t count, const void *indexData);

    template <typename T> void readIndexData(std::vector<T> &indexData) {
        indexData.resize(indexCount());
        readIndexData(indexData.size() * sizeof(T), indexData.data());
    }

    void readIndexData(int byteCount, void *data);

    int indexCount() const;

    void setAttributeDefinition(int index, int tupleSize, int stride,
                                int64_t offset, GLenum type = GL_FLOAT);

    void bind();

    void release();

    void render(GLenum drawStyle = GL_TRIANGLES,
                GLenum indexDataType = GL_UNSIGNED_INT);

  private:
    struct Data;
    std::shared_ptr<Data> m_data;

    GLuint m_vbo = 0;
    GLuint m_ibo = 0;
    GLuint m_vao = 0;
    GLuint m_indexCount = 0;
    GLuint m_vertexCount = 0;
};
} // namespace OpenGLRender

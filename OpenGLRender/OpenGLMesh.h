#pragma once

#include "glad/glad.h"
#include <memory>
#include <vector>

namespace OpenGLRender {
class Mesh {
  public:
    Mesh();

    template <typename T>
    void writeVertexData(const std::vector<T> &vertexData) {
        writeVertexData(vertexData.size() * sizeof(T), vertexData.size(),
                        vertexData.data());
    }

    void writeVertexData(int byteSize, int count, const void *vertexData);

    template <typename T> void readVertexData(std::vector<T> &vertexData) {
        vertexData.resize(vertexCount());
        readVertexData(vertexData.size() * sizeof(T), vertexData.data());
    }

    void readVertexData(int byteCount, void *data);

    int vertexCount() const;

    template <typename T> void writeIndexData(const std::vector<T> &indexData) {
        writeIndexData(indexData.size() * sizeof(T), indexData.size(),
                       indexData.data());
    }

    void writeIndexData(int byteSize, int count, const void *indexData);

    template <typename T> void readIndexData(std::vector<T> &indexData) {
        indexData.resize(indexCount());
        readIndexData(indexData.size() * sizeof(T), indexData.data());
    }

    void readIndexData(int byteCount, void *data);

    int indexCount() const;

    void setAttributeDefinition(int index, int tupleSize, int stride,
                                int offset, GLenum type = GL_FLOAT);

    void bind();

    void release();

    void render(GLenum drawStyle = GL_TRIANGLES,
                GLenum indexDataType = GL_UNSIGNED_INT);

  private:
    struct Data;
    std::shared_ptr<Data> m_data;
};
} // namespace OpenGLRender

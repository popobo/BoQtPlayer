#include "OpenGLShader.h"

#include "BoLog.h"
#include "BoUtil.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace OpenGLRender {

std::string shaderInfoLog(GLint id) {
    GLint length = 0;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

    if (0 >= length) {
        return std::string();
    }

    std::string log;
    log.resize(length + 1);
    GLCall(glGetShaderInfoLog(id, length, NULL, (GLchar *)log.c_str()));
    // why
    log.erase(std::remove(log.begin(), log.end(), '\0'), log.end());
    return log;
}

bool isShaderCompiled(GLuint shaderId) {
    GLint status = 0;
    GLCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status));
    return status == GL_TRUE;
}

int uniformLocation(GLuint programId, const std::string &uniform) {
    GLCall(int uniformLocation =
               glGetUniformLocation(programId, uniform.c_str()));

    if (-1 == uniformLocation) {
        BO_ERROR("Failed to find {0} location", uniform);
    }

    return uniformLocation;
}

struct Shader::Data {
    Data() { createData(); }
    ~Data() { destroyData(); }

    void createData() {
        GLCall(vertexShaderId = glCreateShader(GL_VERTEX_SHADER));
        if (0 == vertexShaderId) {
            BO_ERROR("Failed to create vertex shader");
        }

        GLCall(fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER));
        if (0 == fragmentShaderId) {
            BO_ERROR("Failed to create fragment shader");
        }

        GLCall(programId = glad_glCreateProgram());
        if (0 == programId) {
            BO_ERROR("Failed to create program");
        }
    }

    void destroyData() {
        GLCall(glDeleteShader(vertexShaderId));
        GLCall(glDeleteShader(fragmentShaderId));
        GLCall(glDeleteProgram(programId));
    }

    GLuint vertexShaderId = 0;
    GLuint fragmentShaderId = 0;
    GLuint programId = 0;
};

Shader::Shader() : m_data(std::make_shared<Data>()) {}

void Shader::setVertexShader(const std::string &vertexShader) {
    const char *verterShaderPtr = vertexShader.c_str();
    GLCall(glShaderSource(m_data->vertexShaderId, 1, &verterShaderPtr, 0));
    GLCall(glCompileShader(m_data->vertexShaderId));
    if (!isShaderCompiled(m_data->vertexShaderId)) {
        BO_ERROR("Failed to compile vertex shader");
        BO_ERROR(shaderInfoLog(m_data->vertexShaderId));
    }

    GLCall(glAttachShader(m_data->programId, m_data->vertexShaderId));
}

void Shader::setFragmentShader(const std::string &fragmentShader) {
    const char *fragmentShaderPtr = fragmentShader.c_str();
    GLCall(glShaderSource(m_data->fragmentShaderId, 1, &fragmentShaderPtr, 0));
    GLCall(glCompileShader(m_data->fragmentShaderId));
    if (!isShaderCompiled(m_data->fragmentShaderId)) {
        BO_ERROR("Failed to compile vertex shader");
        BO_ERROR(shaderInfoLog(m_data->fragmentShaderId));
    }

    GLCall(glAttachShader(m_data->programId, m_data->fragmentShaderId));
}

void Shader::link() {
    GLCall(glLinkProgram(m_data->programId));

    int status = 0;
    GLCall(glGetProgramiv(m_data->programId, GL_LINK_STATUS, &status));
    if (status != GL_TRUE) {
        BO_ERROR("Failed to link shader program");
    }

    GLCall(glDetachShader(m_data->programId, m_data->vertexShaderId));
    GLCall(glDetachShader(m_data->programId, m_data->fragmentShaderId));
}

void Shader::bind() {
    GLCall(glUseProgram(m_data->programId));

    int status = 0;
    GLCall(glGetProgramiv(m_data->programId, GL_VALIDATE_STATUS, &status));
    if (status != GL_TRUE) {
        BO_ERROR("Shader program is not valid");
    }
}

void Shader::release() { GLCall(glUseProgram(0)); }

void Shader::setUniform(const std::string &name, int i) {
    int location = uniformLocation(m_data->programId, name);
    GLCall(glUniform1i(location, i));
}

void Shader::setUniform(const std::string &name, const glm::mat4 &matrix) {
    int location = uniformLocation(m_data->programId, name);
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

} // namespace OpenGLRender

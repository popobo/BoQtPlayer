#include "BoOpenGLWidget.h"
#include "BoLog.h"
#include <iostream>

#define GLCall(x)                                                              \
    GLClearError();                                                            \
    x;                                                                         \
    GLLogCall(#x, __FILE__, __LINE__)

namespace {
const int INFO_BUFFER_SIZE = 512;

const char *vertexShaderCode = R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition; //顶点信息
        layout (location = 1) in vec2 aTexCoord; //材质坐标
        out vec2 vTexCoord; //输出的材质坐标
        void main(){
            vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);//以左上角为原点
            gl_Position = vec4(aPosition, 1.0); //现实的顶点
        })";

const char *fragmentShaderYUV420PCode = R"(
        #version 330 core

        uniform sampler2D yTexture;  //输入的材质(不透明灰度, 单像素)
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;

        in vec2 vTexCoord; //顶点着色器传递的坐标
        out vec4 FragColor;
        void main(){
            vec3 yuv;//vec3含有三个元素的向量
            vec3 rgb;
            //这变的rgb相当于yuv
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(uTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.5863, 0.0) * yuv;
            //输出像素颜色
            FragColor = vec4(rgb, 1.0);
        }
)";

} // namespace

BoOpenGLWidget::BoOpenGLWidget(QWidget *widget) : QOpenGLWidget(widget) {
    m_widget = widget;
}

void BoOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    //    static const float vertexs[] = {-1.0f, -1.0f, 0.0f,  0.0f,  0.0f,
    //                                    1.0f, -1.0f, 0.0f,  1.0f,  0.0f,
    //                                    1.0f,  0.5f, 0.0f,  1.0f,  1.0f,
    //                                    1.0f,  1.0f, 0.0f,  1.0f,  1.0f,
    //                                   -1.0f,  1.0f, 0.0f,  0.0f,  1.0f,
    //                                   -1.0f, -1.0f, 0.0f,  0.0f,  0.0f,};
    static const float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    };

    GLCall(glGenVertexArrays(1, &m_VAO));
    GLCall(glBindVertexArray(m_VAO));

    GLCall(glGenBuffers(1, &m_VBO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                        GL_STATIC_DRAW));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                 (void *)0));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                 (void *)(3 * sizeof(float))));
    GLCall(glEnableVertexAttribArray(1));

    m_vertexShader = initShader(vertexShaderCode, GL_VERTEX_SHADER);
    if (0 == m_vertexShader) {
        BO_ERROR("failed initShader");
        return;
    }

    m_fragmentShader =
        initShader(fragmentShaderYUV420PCode, GL_FRAGMENT_SHADER);

    if (0 == m_vertexShader) {
        BO_ERROR("failed initShader");
        return;
    }

    BO_INFO("init shaders successfully");
    GLCall(m_programID = glCreateProgram());
    if (0 == m_programID) {
        BO_ERROR("failed glCreateProgram");
        return;
    }

    GLCall(glAttachShader(m_programID, m_vertexShader));
    GLCall(glAttachShader(m_programID, m_fragmentShader));
    GLCall(glLinkProgram(m_programID));
    GLint success;
    GLCall(glGetShaderiv(m_programID, GL_COMPILE_STATUS, &success));
    char info[INFO_BUFFER_SIZE]{0};
    if (0 == success) {
        glGetShaderInfoLog(m_programID, sizeof(info), nullptr, info);
        BO_ERROR(info);
        return;
    }

    GLCall(glUseProgram(m_programID));

    //材质纹理初始化
    //设置纹理层
    GLCall(glUniform1i(glGetUniformLocation(m_programID, "yTexture"), 0));
    GLCall(glUniform1i(glGetUniformLocation(m_programID, "uTexture"), 1));
    GLCall(glUniform1i(glGetUniformLocation(m_programID, "vTexture"), 2));
}

void BoOpenGLWidget::paintGL() {}

void BoOpenGLWidget::resizeGL(int width, int height) {}

GLuint BoOpenGLWidget::initShader(const char *sourceCode, GLint type) {
    // 不需要makeCurrent()和doneCurrent()
    // makeCurrent();
    GLCall(GLuint shader = glCreateShader(type));
    if (0 == shader) {
        BO_ERROR("failed to glCreateShader {0}", type);
        return 0;
    }

    GLCall(glShaderSource(shader, 1, &sourceCode, 0));
    GLCall(glCompileShader(shader));
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    char info[INFO_BUFFER_SIZE]{0};
    if (0 == success) {
        glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
        BO_ERROR(info);
    }
    // doneCurrent();
    return shader;
}

void BoOpenGLWidget::getTexture(unsigned int index, int width, int height,
                                unsigned char *buf, bool isAlpha) {
    unsigned int format = GL_LUMINANCE;

    if (isAlpha) {
        format = GL_LUMINANCE_ALPHA;
    }

    if (0 == m_textures[index]) {
        GLCall(glGenTextures(1, &m_textures[index]));
        GLCall(glActiveTexture(GL_TEXTURE0 + index));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_textures[index]));
        GLCall(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                               GL_LINEAR_MIPMAP_LINEAR));
        GLCall(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                            GL_UNSIGNED_BYTE, buf));
    }
    //    GLCall(glActiveTexture(GL_TEXTURE0 + index));
    //    GLCall(glBindTexture(GL_TEXTURE_2D, m_textures[index]));

    //    //替换纹理内容
    //    GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format,
    //                           GL_UNSIGNED_BYTE, buf));
}

BoOpenGLWidget::~BoOpenGLWidget() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
}

void BoOpenGLWidget::GLClearError() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool BoOpenGLWidget::GLLogCall(const char *function, const char *file,
                               int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error]: " << error << " function: " << function
                  << " file: " << file << " line: " << line << std::endl;
        return false;
    }
    return true;
}

void BoOpenGLWidget::receiveData(BoData boData) {
    makeCurrent();
    getTexture(0, boData.width, boData.height, boData.datas[0]);
    getTexture(1, boData.width / 2, boData.height / 2, boData.datas[1]);
    getTexture(2, boData.width / 2, boData.height / 2, boData.datas[2]);

    if (0 == m_programID) {
        return;
    }
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    m_widget->update();
    doneCurrent();
}

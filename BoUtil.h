#pragma once

#include "glad/glad.h"
#include <iostream>

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

#define BO_ASSERT(x, ...)                                                      \
    {                                                                          \
        if (!(x)) {                                                            \
            BO_ERROR("Assertion Failed: {0}", __VA_ARGS__);                    \
            __debugbreak();                                                    \
        }                                                                      \
    }

#define PRINT_FFMPEG_ERROR(ret)                                                \
    {                                                                          \
        char errBuffer[1024]{0};                                               \
        av_strerror(ret, errBuffer, sizeof(errBuffer));                        \
        BO_ERROR(errBuffer);                                                   \
    }

#define GLCall(x)                                                              \
    GLClearError();                                                            \
    x;                                                                         \
    GLLogCall(#x, __FILE__, __LINE__)

#ifndef BOUTIL_H
#define BOUTIL_H

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

#endif // BOUTIL_H

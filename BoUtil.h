#ifndef BOUTIL_H
#define BOUTIL_H

#define BO_ASSERT(x, ...)                                                      \
    {                                                                          \
        if (!(x)) {                                                            \
            BO_ERROR("Assertion Failed: {0}", __VA_ARGS__);                    \
            __debugbreak();                                                    \
        }                                                                      \
    }

#endif // BOUTIL_H

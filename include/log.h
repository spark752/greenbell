// A namespace for logging
#ifndef GB_LOG_H
#define GB_LOG_H

#include <iostream>
#include <string_view>

namespace Greenbell {

enum LogLevel { LOG_ERROR, LOG_INFO, LOG_TRACE };

namespace Log {

// Return current level
LogLevel Level();

// "Write" is partially thread safe, as calling it from multiple threads
// should not cause a crash but could mix multiple messages together.
inline void Write(LogLevel level, const char* s) {
    if (level <= Level()) {
        if (level == LOG_ERROR) {
            while (s and *s) {
                std::cerr << *s++;
            }
            std::cerr << "\n";
        } else {
            while (s and *s) {
                std::cout << *s++;
            }
            std::cout << "\n";
        }
    }
}

inline void Write(LogLevel level, std::string_view sv) {
    if (level <= Level()) {
        if (level == LOG_ERROR) {
            std::cerr << sv << "\n";
        } else {
            std::cout << sv << "\n";
        }
    }
}

template<typename T, typename... Args>
void Write(LogLevel level, const char* s, T value, Args... args) {
    if (level <= Level()) {
        if (level == LOG_ERROR) {
            while (s and *s) {
                if (*s == '%' and *++s != '%') {
                    std::cerr << value;
                    return Write(level, ++s, args...);
                }
                std::cerr << *s++;
            }
            std::cerr << "\n";
        } else {
            while (s and *s) {
                if (*s == '%' and *++s != '%') {
                    std::cout << value;
                    return Write(level, ++s, args...);
                }
                std::cout << *s++;
            }
            std::cout << "\n";
        }
    }
}

// Write an error message for the results of a glGetError
void WriteGLError(unsigned int error);

// "SetLevel" is NOT thread safe. However it is typically only called once
// at the start of the application and not changed again. Also the level is
// only checked once at the start of "Write" so having it suddenly
// changed by another thread is not much of a problem.
void SetLevel(LogLevel level);

} // namespace Log
} // namespace Greenbell
#endif

#include "log.h"
#include "glad.h"

namespace Greenbell::Log {

static LogLevel level_ = LOG_INFO;

void SetLevel(LogLevel level) {
    level_ = level;
}

LogLevel Level() {
    return level_;
}

void WriteGLError(unsigned int error) {
    std::cout << "OpenGL Error: ";
    switch (error) {
        case GL_INVALID_ENUM:
            std::cout << "GL_INVALID_ENUM\n";
            break;
        case GL_INVALID_VALUE:
            std::cout << "GL_INVALID_VALUE\n";
            break;
        case GL_INVALID_OPERATION:
            std::cout << "GL_INVALID_OPERATION\n";
            break;
        case GL_STACK_OVERFLOW:
            std::cout << "GL_STACK_OVERFLOW\n";
            break;
        case GL_STACK_UNDERFLOW:
            std::cout << "GL_STACK_UNDERFLOW\n";
            break;
        case GL_OUT_OF_MEMORY:
            std::cout << "GL_OUT_OF_MEMORY\n";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
            break;
        case GL_CONTEXT_LOST:
            std::cout << "GL_CONTEXT_LOST\n";
            break;
        default:
            std::cout << "Unknown Error\n";
            break;
    }
}

} // namespace Greenbell::Log

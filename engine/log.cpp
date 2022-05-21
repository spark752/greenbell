#include "log.h"
#include "glad.h"
#include "gb_fmt.h"
#include <exception>

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


// Based on CC0 licensed
//  https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions
void MessageCallback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei /* length */, GLchar const* message,
        void const* /* user_param */) {
    const auto* const src_str = [source]() {
        switch (source) {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION: return "Application";
            default: return "Other";
        }
    }();

    const auto* const type_str = [type]() {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    return "Deprecated Behaviour";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
            case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
            case GL_DEBUG_TYPE_MARKER: return "Marker";
            default: return "Other";
        }
    }();

    const auto* const severity_str = [severity]() {
        switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "Note";
            case GL_DEBUG_SEVERITY_LOW: return "Low";
            case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "UNKNOWN";
        }
    }();
    fmt::print(FMT_STRING("{} {} {} {}: {}\n"), src_str, type_str, 
            severity_str, id, message);
    if (type == GL_DEBUG_TYPE_ERROR) throw std::runtime_error("OpenGL Error");
}

} // namespace Greenbell::Log

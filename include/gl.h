// For wrapping OpenGL functions and a few useful related things
#ifndef GB_GL_WRAPPER_H
#define GB_GL_WRAPPER_H

#if __cplusplus < 201703L
#error "A C++17 compiler is required"
#endif
#include "glad.h"

#include "types.h"
#include <string_view>

#ifdef DEBUG_WRAPPERS
#include <fmt/printf.h>
#endif

namespace Greenbell::GL {

// Template base class for owning OpenGL objects
constexpr auto BUFFER_CLASS_TEMPLATE = 0;
constexpr auto PROGRAM_CLASS_TEMPLATE = 13;
constexpr auto VAO_CLASS_TEMPLATE = 42;
constexpr auto RBO_CLASS_TEMPLATE = 69;
constexpr auto FBO_CLASS_TEMPLATE = 95;
template <int N>
class GenericObject {
  public:
    GenericObject() noexcept {
        if constexpr (N == RBO_CLASS_TEMPLATE) {
            glCreateRenderbuffers(1, &id_);
        } else if constexpr (N == FBO_CLASS_TEMPLATE) {
            glCreateFramebuffers(1, &id_);
        } else if constexpr (N == VAO_CLASS_TEMPLATE) {
            glGenVertexArrays(1, &id_);
        } else if constexpr (N == PROGRAM_CLASS_TEMPLATE) {
            id_ = glCreateProgram();
        } else {
            glCreateBuffers(1, &id_);
        }
        #ifdef DEBUG_WRAPPERS        
        fmt::printf("GenericObject %d ctor %d\n", N, id_);
        #endif          
    }
    virtual ~GenericObject() noexcept {
        #ifdef DEBUG_WRAPPERS        
        fmt::printf("GenericObject %d dtor %d\n", N, id_);
        #endif        
        if constexpr (N == RBO_CLASS_TEMPLATE) {
            glDeleteRenderbuffers(1, &id_);
        } else if constexpr (N == FBO_CLASS_TEMPLATE) {
            glDeleteFramebuffers(1, &id_);
        } else if constexpr (N == VAO_CLASS_TEMPLATE) {
            glDeleteVertexArrays(1, &id_);
        } else if constexpr (N == PROGRAM_CLASS_TEMPLATE) {
            glDeleteProgram(id_);         
        } else {
            glDeleteBuffers(1, &id_);
        }
    }
    
    GenericObject(const GenericObject&) = delete;            // No copy
    GenericObject& operator=(const GenericObject&) = delete; // No copy assign

    GenericObject(GenericObject&& source) noexcept : id_{source.id_} {
        source.id_ = 0;
    } // Move

    GenericObject& operator=(GenericObject&& source) noexcept {
        #ifdef DEBUG_WRAPPERS        
        fmt::printf("GenericObject assign %d replaced by %d\n", id_, 
                source.id_);
        #endif        
        if (&source == this) return *this; // Self assignment
        if (id_) {
            // We are moving into this. If it already has a buffer, delete
            // it so it can be replaced with source buffer.
            if constexpr (N == RBO_CLASS_TEMPLATE) {
                glDeleteRenderbuffers(1, &id_);
            } else if constexpr (N == FBO_CLASS_TEMPLATE) {
                glDeleteFramebuffers(1, &id_);
            } else if constexpr (N == VAO_CLASS_TEMPLATE) {
                glDeleteVertexArrays(1, &id_);
            } else if constexpr (N == PROGRAM_CLASS_TEMPLATE) {
                glDeleteProgram(id_);
            } else {
                glDeleteBuffers(1, &id_);
            }
        }
        id_ = source.id_;
        source.id_ = 0;
        return *this;
    } // Move assign

    // Check for valid ID easily
    explicit operator bool() const noexcept {
        return id_;
    }
    
    // Get the object "name" for use with OpenGL calls
    auto name() const noexcept {
        return id_;
    }
            
  protected:
    uint32_t id_{0};
};
typedef GenericObject<RBO_CLASS_TEMPLATE> RBO;
typedef GenericObject<FBO_CLASS_TEMPLATE> FBO;

// Template class for owning OpenGL buffer objects
template <GLenum TARGET>
class BufferObject : public GenericObject<BUFFER_CLASS_TEMPLATE> {
  public:
    // Do not define ctor/dtor so all base class move/copy things will be used
    void bind() const noexcept {
        glBindBuffer(TARGET, id_);
    }
    void unbind() const noexcept {
        glBindBuffer(TARGET, id_);
    }
};
typedef BufferObject<GL_ARRAY_BUFFER> VBO;
typedef BufferObject<GL_ELEMENT_ARRAY_BUFFER> IBO;
typedef BufferObject<GL_UNIFORM_BUFFER> UBO;

// Class for owning a Vertex Array Object
class VAO : public GenericObject<VAO_CLASS_TEMPLATE> {
  public:
    // Do not define ctor/dtor so all base class move/copy things will be used
    void bind() const noexcept {
        glBindVertexArray(id_);
    }
};

// Class for owning a shader Program Object
class ProgramObject : public GenericObject<PROGRAM_CLASS_TEMPLATE> {
  public:
    // Do not define ctor/dtor so all base class move/copy things will be used
    void use() const noexcept {
         glUseProgram(id_);
    }
};

// Template class for owning OpenGL shader objects.
// Instantations for common types have typedefs below but derived classes are 
// also possible.
template <GLenum TARGET>
class ShaderObject {
  public:
    ShaderObject() noexcept {
        id_ = glCreateShader(TARGET);
#ifdef GL_WRAPPER_DEBUG        
        fmt::printf("ShaderObject %d ctor %d\n", TARGET, id_);   
#endif    
    }
    virtual ~ShaderObject() noexcept {
        #ifdef DEBUG_WRAPPERS        
        fmt::printf("ShaderObject %d dtor %d\n", TARGET, id_);
        #endif        
        glDeleteShader(id_); // Spec says value of 0 will be silently ignored
    }

    ShaderObject(const ShaderObject&) = delete;            // No copy
    ShaderObject& operator=(const ShaderObject&) = delete; // No copy assign

    ShaderObject(ShaderObject&& source) noexcept : id_{source.id_} {
        source.id_ = 0;
    } // Move

    ShaderObject& operator=(ShaderObject&& source) noexcept {
        #ifdef DEBUG_WRAPPERS        
        fmt::printf("ShaderObject assign %d replaced by %d\n", id_, source.id_);
        #endif        
        if (&source == this) return *this; // Self assignment
        if (id_) {
            // We are moving into this. If it already has a shader, delete
            // it so it can be replaced with source shader.
            glDeleteShader(id_);
        }
        id_ = source.id_;
        source.id_ = 0;
        return *this;
    } // Move assign

    // Check for valid ID easily
    explicit operator bool() const noexcept {
        return id_;
    }

    // Get the shader "name" for use with OpenGL calls
    auto name() const noexcept {
        return id_;
    }

    // Compile the shader
    void compile(const char* p_source) const noexcept {
        // OpenGL takes a pointer to an array of null terminated strings
        auto p_array = p_source;
        glShaderSource(id_, 1, static_cast<const GLchar**>(&p_array), nullptr);
        
        // OpenGL will set the shader status and info log
        glCompileShader(id_);
    }
    void compile(std::string_view source) const noexcept {
        // OpenGL takes a pointer to an array of strings + array of lengths
        auto p_array = source.data();
        auto len = static_cast<int32_t>(source.length());
        glShaderSource(id_, 1, static_cast<const GLchar**>(&p_array), &len);
        
        // OpenGL will set the shader status and info log
        glCompileShader(id_);
    }
     
    // Get the compile status via glGetShaderiv
    bool get_compile_status() const noexcept {
        int32_t ret;
        glGetShaderiv(id_, GL_COMPILE_STATUS, &ret);
        return (ret == GL_TRUE);
    }
        
    // Attach the shader to a given program
    void attach(uint32_t pid) const noexcept {
        if (pid && id_) glAttachShader(pid, id_);
    }
        
    // Detach the shader from the given program
    void detach(uint32_t pid) const noexcept {
        if (pid && id_) glDetachShader(pid, id_);
    }
    
  protected:
    uint32_t id_{0};
};
typedef ShaderObject<GL_VERTEX_SHADER> VertexShader;
typedef ShaderObject<GL_FRAGMENT_SHADER> FragmentShader;
typedef ShaderObject<GL_COMPUTE_SHADER> ComputeShader;
typedef ShaderObject<GL_GEOMETRY_SHADER> GeometryShader;
typedef ShaderObject<GL_TESS_CONTROL_SHADER> TessControlShader;
typedef ShaderObject<GL_TESS_EVALUATION_SHADER> TessEvaluationShader;

// Misc wrapped functions
// **********************
inline void DepthTest(bool enable) noexcept {
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}
inline void Blend(bool enable) noexcept {
    if (enable) {
        glEnable(GL_BLEND); 
    } else { 
        glDisable(GL_BLEND);
    }
}
inline void SetWireframe(bool value) {
    glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
}
inline void ClearColour(float red, float green, float blue) {
    glClearColor(red, green, blue, 1.0f);
}
inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    glViewport(x, y, width, height);
}
inline void Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShowSystemInfo(); // In .cpp file
void ShowMemoryInfo(); // In .cpp file

} // namespace Greenbell::GL
#endif

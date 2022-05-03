#include "shader.h"
#include "log.h"
#include "gb_fmt.h"
#include "gl.h"
#include "gl_layout.h"
#include <string>

namespace Greenbell::Shader {

void InitProgram(GLuint pid, std::string_view vertex_source,
        std::string_view fragment_source) {
    static constexpr auto fail_msg = "InitProgram shader";

    // Vertex shader
    Log::Write(LOG_TRACE, "InitProgram VS:");
    Log::Write(LOG_TRACE, vertex_source);
    const GL::VertexShader vertex_shader;
    vertex_shader.compile(vertex_source);
    if (!vertex_shader.get_compile_status()) {
        Log::Write(LOG_ERROR, "Unable to compile vertex shader");
        PrintLog(vertex_shader.name());
        throw std::runtime_error(fail_msg);
    }

    // Fragment shader
    Log::Write(LOG_TRACE, "InitProgram FS:");
    Log::Write(LOG_TRACE, fragment_source);
    const GL::FragmentShader fragment_shader;
    fragment_shader.compile(fragment_source);
    if (!fragment_shader.get_compile_status()) {
        Log::Write(LOG_ERROR, "Unable to compile fragment shader");
        PrintLog(fragment_shader.name());
        throw std::runtime_error(fail_msg);
    }

    // Program
    vertex_shader.attach(pid);
    fragment_shader.attach(pid);
    glLinkProgram(pid);
    int32_t error_check = GL_FALSE;
    glGetProgramiv(pid, GL_LINK_STATUS, &error_check);
    if (error_check != GL_TRUE) {
        Log::Write(LOG_ERROR, "Unable to link shader program");
        PrintLog(pid);
        throw std::runtime_error(fail_msg);
    }

    // Detach shaders (will delete when they go out of scope)
    vertex_shader.detach(pid);
    fragment_shader.detach(pid);

    Log::Write(LOG_TRACE, "CreateProgram Finished for ID = %d", pid);
}

void InitOverlayProgram(GLuint pid) {
    const auto vs = fmt::format(
            "#version 450 core\n"
            "layout(location = {}) in vec3 position\n;"
            "layout(location = {}) in vec2 uv\n;"
            "out vec2 fuv;\n"
            "void main() {{\n"
            "  fuv=uv;\n"
            "  gl_Position=vec4(position.x,position.z,0.0,1.0);\n"
            "}}\n", LOC_POSITION, LOC_TEX_COORD);
    const std::string fs =
            "#version 450 core\n"
            "out vec4 frag;\n"
            "in vec2 fuv;\n"
            "layout(binding = 0) uniform sampler2D tex0;\n"
            "void main() {\n"
            "  frag=texture(tex0,fuv);\n"
            "}\n";
    InitProgram(pid, vs, fs);
}

// OpenGL shader compiler error log, from public domain OpenGL tutorial
void PrintLog(GLuint object) {
    GLint log_length = 0;
    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        Log::Write(LOG_ERROR, "Shader PrintLog: Not a shader or a program");
        return;
    }
    std::vector<char> error_log(static_cast<size_t>(log_length));
    if (glIsShader(object)) {
        glGetShaderInfoLog(object, log_length, nullptr, error_log.data());
    } else if (glIsProgram(object)) {
        glGetProgramInfoLog(object, log_length, nullptr, error_log.data());
    }
    std::cerr << error_log.data();
}

} // namespace Greenbell::Shader

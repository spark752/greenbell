#include "shader.h"
#include "log.h"
#include "gb_fmt.h"
#include "gl.h"
#include "gl_layout.h"
#include <string>
#include <string_view>
#include <cstddef>

namespace Greenbell::Shader {

static constexpr std::string_view GLSL_VERSION = "#version 450 core\n";

void Build(GLuint pid, std::string_view vertex_source,
        std::string_view fragment_source) {
    static constexpr auto fail_msg = "Shader::Build";

    Log::Write(LOG_TRACE, "Build VS:");
    Log::Write(LOG_TRACE, vertex_source);
    const GL::VertexShader vertex_shader{};
    vertex_shader.compile(vertex_source);
    if (!vertex_shader.get_compile_status()) {
        Log::Write(LOG_ERROR, "Unable to compile vertex shader");
        PrintShaderLog(vertex_shader.name());
        throw std::runtime_error(fail_msg);
    }

    Log::Write(LOG_TRACE, "Build FS:");
    Log::Write(LOG_TRACE, fragment_source);
    const GL::FragmentShader fragment_shader{};
    fragment_shader.compile(fragment_source);
    if (!fragment_shader.get_compile_status()) {
        Log::Write(LOG_ERROR, "Unable to compile fragment shader");
        PrintShaderLog(fragment_shader.name());
        throw std::runtime_error(fail_msg);
    }

    vertex_shader.attach(pid);
    fragment_shader.attach(pid);
    glLinkProgram(pid);
    GLint error_check = GL_FALSE;
    glGetProgramiv(pid, GL_LINK_STATUS, &error_check);
    if (error_check != GL_TRUE) {
        Log::Write(LOG_ERROR, "Unable to link shader program");
        PrintProgramLog(pid);
        throw std::runtime_error(fail_msg);
    }

    // Detach shaders (will delete when they go out of scope)
    vertex_shader.detach(pid);
    fragment_shader.detach(pid);

    Log::Write(LOG_TRACE, "Build Finished for ID = %d", pid);
}

void BuildOverlay(GLuint pid) {
    BuildOverlay(pid, "  frag=texture(tex,fuv);\n");
}
    
void BuildOverlay(GLuint pid, std::string_view main) {
    const auto vs = fmt::format(
            "{}"
            "layout(location = {}) in vec2 position;\n"
            "layout(location = {}) in vec2 uv;\n"
            "out vec2 fuv;\n"
            "void main() {{\n"
            "  fuv=uv;\n"
            "  gl_Position=vec4(position.x,position.y,0.0,1.0);\n"
            "}}\n", GLSL_VERSION, LOC_POSITION, LOC_TEX_COORD);
    const auto fs = fmt::format(
            "{}"
            "out vec4 frag;\n"
            "in vec2 fuv;\n"
            "layout(binding = {}) uniform sampler2D tex;\n"
            "void main() {{\n"
            "{}"
            "}}\n", GLSL_VERSION, TEXBIND_OVERLAY, main);
    Build(pid, vs, fs);
}

void BuildCompute(GLuint pid, std::string_view source) {
    static constexpr auto fail_msg = "Shader::BuildCompute";
    Log::Write(LOG_TRACE, "BuildCompute:");
    Log::Write(LOG_TRACE, source);
    const GL::ComputeShader shader{};
    shader.compile(source);
    if (!shader.get_compile_status()) {
        Log::Write(LOG_ERROR, "Unable to compile compute shader");        
        PrintShaderLog(shader.name());
        throw std::runtime_error(fail_msg);
    }
    shader.attach(pid);
    glLinkProgram(pid);
    GLint error_check = GL_FALSE;
    glGetProgramiv(pid, GL_LINK_STATUS, &error_check);
    if (error_check != GL_TRUE) {
        Log::Write(LOG_ERROR, "Unable to link compute program");
        PrintProgramLog(pid);
        throw std::runtime_error(fail_msg);
    }    
    shader.detach(pid); // So it can delete when it goes out of scope
    Log::Write(LOG_TRACE, "Build Finished for ID = %d", pid);
}

// OpenGL shader compiler error logs, from public domain OpenGL tutorial
void PrintShaderLog(GLuint shader_id) {
    GLint log_length = 0;
    if (glIsShader(shader_id)) {
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        Log::Write(LOG_ERROR, "PrintShaderLog: Not a shader");
        return;
    }
    std::vector<char> error_log(static_cast<std::size_t>(log_length));
    glGetShaderInfoLog(shader_id, log_length, nullptr, error_log.data());
    std::cerr << error_log.data();
}
void PrintProgramLog(GLuint pid) {
    GLint log_length = 0;
    if (glIsProgram(pid)) {
        glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        Log::Write(LOG_ERROR, "PrintProgramLog: Not a program");
        return;
    }
    std::vector<char> error_log(static_cast<std::size_t>(log_length));
    glGetProgramInfoLog(pid, log_length, nullptr, error_log.data());
    std::cerr << error_log.data();
}

} // namespace Greenbell::Shader

#ifndef GB_SHADER_H
#define GB_SHADER_H

#include "gl.h"
#include <string_view>

namespace Greenbell::Shader {

// Build a program from two string based shaders
void Build(GLuint pid, std::string_view vertex_source,
        std::string_view fragment_source);

// Build a simple program for a UV mapped quad overlay
// This version will just sample from texture unit TEXBIND_OVERLAY and render 
// the results like "frag=texture(tex,fuv)"
void BuildOverlay(GLuint pid);
// This version will use user provided contents for the body of main() to
// replace the function above 
void BuildOverlay(GLuint pid, std::string_view main);

// Build a compute shader
void BuildCompute(GLuint pid, std::string_view source);

// Print an error log for compiling a shader or linking a program
void PrintShaderLog(GLuint shader_id);
void PrintProgramLog(GLuint pid);

} // namespace Greenbell::Shader

#endif

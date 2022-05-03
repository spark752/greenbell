#ifndef GB_SHADER_H
#define GB_SHADER_H

#include "gl.h"
#include <string_view>

namespace Greenbell::Shader {

// Initialize a program from two string based shaders
void InitProgram(GLuint pid, std::string_view vertex_source,
        std::string_view fragment_source);

// Initialize a program specifically for use as an overlay
void InitOverlayProgram(GLuint pid);

// Print an error log for a shader or program object
void PrintLog(GLuint object);

} // namespace Greenbell::Shader

#endif

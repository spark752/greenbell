// Constants for OpenGL layout specifiers
#ifndef GL_LAYOUT_H
#define GL_LAYOUT_H

namespace Greenbell {

// Vertex shader inputs
// ********************
// GLSL layout locations for vertex attributes
inline constexpr auto LOC_POSITION = 0;
inline constexpr auto LOC_TEX_COORD = 1;
inline constexpr auto LOC_COLOUR = 2;
inline constexpr auto LOC_NORMAL = 3;
inline constexpr auto LOC_TANGENT = 4;
inline constexpr auto LOC_BONE_INDEX = 5;
inline constexpr auto LOC_BONE_WEIGHT = 6;

// Uniforms
// ********
// Must be unique per program, not just per shader

// UBO binding points
// ******************
inline constexpr auto BINDPOINT_MATRICES = 0;
inline constexpr auto BINDPOINT_PBR_COMBO = 1;

} // namespace Greenbell
#endif

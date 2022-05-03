// Greenbell header file to include the right glm settings.
// Named to try and avoid confusion with the glm header itself.
#ifndef GB_GLM_H
#define GB_GLM_H

// Some settings can be defined in cmake
#include "cmake_config.h"

#define GLM_FORCE_CXX17         /* C++17 as required by the rest of Greenbell */
#define GLM_FORCE_RADIANS       /* Always use radians (should be default now) */
#define GLM_FORCE_EXPLICIT_CTOR /* Do not allow implicit conversion of types */
#define GLM_FORCE_SIZE_T_LENGTH /* Make length functions size_t compatible */
#define GLM_ENABLE_EXPERIMENTAL /* Needed for dual quaternions */
#undef GLM_FORCE_QUAT_DATA_WXYZ /* Must not be defined */

// GLM_FORCE_MESSAGES can be set through cmake to see build config messages

// GLM_FORCE_INTRINSICS can be set through cmake to get SIMD optimizations
// automatically based on compiler flags.

// Alternatively several options can be forced via cmake:
// GLM_FORCE_SSE2 = Base level supported by all x86_64 architecture.
//      Compiler support is through the -msse2 flag which should be default.
// GLM_FORCE_SSE3 = Introduced in 2003 and has been required for Google Chrome
//      since early 2021 so should also be widely supported. 
//      Compiler support is through the -msse3 flag.
// GLM_FORCE_AVX = Available since 2011, but several recent AAA games released
//      with AVX and ended up patching it out after customer complaints.
//      Compiler support is through the -mavx flag.

// Unfortunately using SIMD optimizations will prevent most constexpr support
// from glm.

// Using language extensions triggers some warnings which this silences
#define GLM_FORCE_SILENT_WARNINGS

#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/quaternion.hpp"
#include "../glm/glm/gtx/transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include "../glm/glm/gtx/dual_quaternion.hpp"
#include "../glm/glm/gtx/norm.hpp" /* needed to use dual_quaternion.hpp */

#endif

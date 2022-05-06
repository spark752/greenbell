#ifndef GB_OVERLAY_H
#define GB_OVERLAY_H

#include "gl.h"
#include "gl_layout.h"

namespace Greenbell {

// Full window texture quad for overlays. Owns its own VAO & VBO because of the 
// specialized vertex format used, but one instance can be created and ran
// multiple times with different textures and shaders if needed.
// CONSTRUCTOR MAKES OpenGL CALLS
class Overlay {
  public:
    Overlay() noexcept {
        // Normalized signed 16 bit to support partial coverage in possible
        // future version.
        static constexpr GLshort qv[] = {
            INT16_MIN, INT16_MIN, 0,         INT16_MAX,
            INT16_MAX, INT16_MIN, INT16_MAX, INT16_MAX,
            INT16_MIN, INT16_MAX, 0,         0,
            INT16_MAX, INT16_MAX, INT16_MAX, 0,
        };
        static constexpr auto tex_coord_offset = sizeof(GLshort) * 2;
        static constexpr auto stride = tex_coord_offset + sizeof(GLshort) * 2;

        glNamedBufferData(vbo_.name(), static_cast<GLsizeiptr>(sizeof(qv)),
                qv, GL_STATIC_DRAW);

        const auto vao_name = vao_.name();
        glVertexArrayVertexBuffer(vao_name, 0, vbo_.name(), 0, stride);

        glVertexArrayAttribFormat(vao_name, Greenbell::LOC_POSITION,
                2, GL_SHORT, GL_TRUE, 0);
        glVertexArrayAttribBinding(vao_name, Greenbell::LOC_POSITION, 0);
        glEnableVertexArrayAttrib(vao_name, Greenbell::LOC_POSITION);
        glVertexArrayAttribFormat(vao_name, Greenbell::LOC_TEX_COORD, 2,
                GL_SHORT, GL_TRUE, tex_coord_offset);
        glVertexArrayAttribBinding(vao_name, Greenbell::LOC_TEX_COORD, 0);
        glEnableVertexArrayAttrib(vao_name, Greenbell::LOC_TEX_COORD);
    }
    void bind() const noexcept {
        vao_.bind();
    }
    void unbind() const noexcept {
        vao_.unbind();
    }
    void draw() const noexcept { // Not static for API consistency NOLINT
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

  private:
    Greenbell::GL::VBO vbo_{};
    Greenbell::GL::VAO vao_{};
};

} // namespace Greenbell

#endif

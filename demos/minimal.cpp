// A minimal Greenbell demo that doesn't use the demo framework
#include "window.h"
#include "SDL2/SDL.h"
#include "gl.h"
#include "gl_layout.h"
#include "shader.h"
#include "gb_fmt.h"
#include "log.h"

int main() // Let exceptions terminate the program NOLINT
{
    // Turn on trace logging
    Greenbell::Log::SetLevel(Greenbell::LOG_TRACE);
    
    // Fill out WindowInfo with properties, or just use the defaults
    Greenbell::WindowInfo win_info;
    win_info.width = 512;
    win_info.height = 512;
    
    // Create a Window which will initialize SDL and OpenGL. Do not make any
    // OpenGL calls before doing this. That includes creating objects which
    // make OpenGL calls in their constructors.
    Greenbell::Window win{win_info};
    
    // Setup some mesh
    static constexpr GLfloat v[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    static constexpr auto stride = sizeof(GLfloat) * 3;
    Greenbell::GL::VBO vbo{};
    glNamedBufferData(vbo.name(), static_cast<GLsizeiptr>(sizeof(v)),
            v, GL_STATIC_DRAW);
    
    Greenbell::GL::VAO vao{};
    const auto vao_name = vao.name();
    glVertexArrayVertexBuffer(vao_name, 0, vbo.name(), 0, stride);
    
    glVertexArrayAttribFormat(vao_name, Greenbell::LOC_POSITION,
            3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao_name, Greenbell::LOC_POSITION, 0);
    glEnableVertexArrayAttrib(vao_name, Greenbell::LOC_POSITION);
    
    // Setup some shader program
    const auto vs = fmt::format(
            "#version 450 core\n"
            "layout(location = {}) in vec3 position;\n"
            "void main() {{\n"
            "  gl_Position = vec4(position, 1.0);\n"
            "}}\n", Greenbell::LOC_POSITION);
    const auto fs = fmt::format(
            "#version 450 core\n"
            "out vec4 frag;\n"
            "void main() {{\n"
            "  frag = vec4(0.5, 0.0, 0.5, 1.0);\n"
            "}}\n");
    const Greenbell::GL::ProgramObject program{};
    Greenbell::Shader::Build(program.name(), vs, fs);
    
    // Main loop
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        // Initialize timing for the new frame
        win.start_frame();
        
        // Check event queue
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        // Clear the window for the main rendering pass
        Greenbell::GL::Clear();
        
        // Draw. Of course setting the program and VAO each frame is not
        // really necessary when drawing only one object.
        program.use();
        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        
        // Send the output to the window and use the soft frame limiter to 
        // block the thread based on a desired maximum frame rate
        win.end_frame(Greenbell::LIMITER_200_FPS);
    }
    
    return 0;
}

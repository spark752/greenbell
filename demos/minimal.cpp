// A minimal Greenbell demo that doesn't use the demo framework
#include "window.h"
#include "SDL2/SDL.h"

int main()
{
    // Fill out WindowInfo with properties, or just use the defaults
    Greenbell::WindowInfo win_info;
    
    // Create a Window which will initialize SDL and OpenGL. Do not make any
    // OpenGL calls before doing this. That includes creating objects which
    // make OpenGL calls in their constructors.
    Greenbell::Window win{win_info};
    
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
        
        // Add code to draw something here
        
        // Send the output to the window and use the soft frame limiter to 
        // block the thread based on a desired maximum frame rate
        win.end_frame(Greenbell::LIMITER_200_FPS);
    }
    
    return 0;
}

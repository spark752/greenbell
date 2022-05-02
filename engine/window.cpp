#include "window.h"
#include "sdl_exceptions.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "log.h"
#include <thread>

namespace Greenbell {

// Manage init/deinit of SDL window
class SDLWindowWrapper {
  public:
    SDLWindowWrapper(WindowInfo const& win_info) {
        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
            if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) throw SDLException();
        }

        // Attributes must be set before creating window
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, win_info.ogl_major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, win_info.ogl_minor);
        if (win_info.msaa) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, win_info.msaa);
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                SDL_GL_CONTEXT_PROFILE_CORE);

        uint32_t flags =
                SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
        if (win_info.fullscreen) {
            flags = flags | SDL_WINDOW_FULLSCREEN;
        }

        // Create window
        p_sdl_window_ = SDL_CreateWindow(win_info.title.c_str(), win_info.x,
                win_info.y, win_info.width, win_info.height, flags);
        if (!p_sdl_window_) throw SDLException();
        
        // Create context
        ogl_context_ = SDL_GL_CreateContext(p_sdl_window_);
        if (!ogl_context_) throw SDLException();
    }

    ~SDLWindowWrapper() {
        if (ogl_context_) SDL_GL_DeleteContext(ogl_context_);
        if (p_sdl_window_) SDL_DestroyWindow(p_sdl_window_);
        SDL_Quit();
    }

    // No copy or move
    SDLWindowWrapper(const SDLWindowWrapper&) = delete;
    SDLWindowWrapper& operator=(const SDLWindowWrapper&) = delete;
    SDLWindowWrapper(SDLWindowWrapper&&) = delete;
    SDLWindowWrapper& operator=(SDLWindowWrapper&&) = delete;

    [[nodiscard]] std::pair<int, int> drawable_size() const noexcept {
        int w = 0;
        int h = 0;
        if (p_sdl_window_) {
            SDL_GL_GetDrawableSize(p_sdl_window_, &w, &h);
        }
        return std::make_pair(w, h);
    }

    void swap_window() const noexcept {
        if (p_sdl_window_) SDL_GL_SwapWindow(p_sdl_window_);
    }

  private:
    SDL_Window* p_sdl_window_{nullptr};
    SDL_GLContext ogl_context_{nullptr}; // Actually a void* in SDL
};

// Main class
Window::~Window() {
    Log::Write(LOG_INFO, "Greenbell Window Shutdown");
    TTF_Quit();
}

// ctor can throw exceptions
Window::Window(WindowInfo win_info) : win_info_(std::move(win_info)) {
    // Check OpenGL version
    Log::Write(LOG_INFO, "OpenGL version requested: %d.%d Core",
            win_info_.ogl_major, win_info_.ogl_minor);
    if (win_info_.ogl_major < 4) {
        throw std::runtime_error(
                "OpenGL versions less than 4 are not supported");
    }

    // We want an SRGB framebuffer which means doing this before creating the
    // window would be the right thing:
    // SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    // Except it causes context creation to fail with some drivers.
    // So we call "glEnable(GL_FRAMEBUFFER_SRGB)" after context creation
    // which seems to be enough for OpenGL.
    
    // Initialize SDL, create window, create OpenGL context
    ps_win_ = std::make_unique<SDLWindowWrapper>(win_info_);

    // Get screen and drawing size
    const auto display_count = SDL_GetNumVideoDisplays();
    Log::Write(LOG_INFO, "Displays Detected: %d", display_count);
    SDL_DisplayMode display_mode;
    for (auto i = 0; i < display_count; ++i) {
        if (SDL_GetDesktopDisplayMode(i, &display_mode) != 0) {
            Log::Write(LOG_ERROR, "Error getting desktop display mode");
        } else {
            Log::Write(LOG_INFO, "Display #%d Width: %d Height: %d", i,
                    display_mode.w, display_mode.h);
        }
    }
    auto [draw_width, draw_height] = ps_win_->drawable_size();
    Log::Write(LOG_INFO, "Draw Size Width: %d Height: %d", draw_width,
            draw_height);

    // Fullscreen + multi-monitor = don't minimize on focus loss
    if (win_info_.fullscreen && display_count > 1) {
        SDL_SetHint("SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS", "0");
    }

    // Load OpenGL functions
    // Use Glad as an OpenGL loading library
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        Log::Write(LOG_ERROR, "OpenGL interface load failed");
        throw std::runtime_error("Error creating window");
    }

    // Now we can report on the context since glGetString should be loaded
    Log::Write(LOG_INFO, "Context created: %s", glGetString(GL_VERSION));

    // Enable or disable VSYNC
    if (SDL_GL_SetSwapInterval(win_info_.vsync ? 1 : 0) < 0) {
        Log::Write(LOG_ERROR, "SDL VSYNC Error: %s", SDL_GetError());
        // Hardware might not support and there can be other modes, so
        // consider this to be non-fatal
    }

    // Depth, viewport, colour, etc.
    GL::Viewport(0, 0, win_info_.width, win_info_.height);
    GL::ClearColour(0.0f, 0.0f, 0.0f);
    if (win_info_.msaa) glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    // Default colour and blend modes
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Check for errors
#ifdef USE_GET_ERROR
    const auto e = glGetError();
    if (e != GL_NO_ERROR) {
        Log::WriteGLError(e);
        Log::Write(LOG_ERROR, "OpenGL error %X %s:%d", e, __FILE__, __LINE__);
        // Consider this to be non-fatal (?)
    }
#endif

    // Initialize SDL_ttf
    if (!TTF_WasInit() && TTF_Init()) throw TTFException();
}

void Window::start_frame() const noexcept {
    time_point_ = std::chrono::steady_clock::now(); // mutable
}

std::chrono::microseconds Window::end_frame(
        std::chrono::microseconds min_duration) const noexcept {
    // Send the OpenGL buffer to the SDL window
    ps_win_->swap_window();
    
    // Duration since last update to time_point_
    const auto now = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            now - time_point_);  
    
    // Soft frame limiter
    if (min_duration != LIMITER_DISABLE) {
        // Shave a bit off the wait for system overhead
        constexpr auto LIMITER_TWEAK = std::chrono::microseconds(100);
        const auto wait = min_duration - duration - LIMITER_TWEAK;
        // Skip the wait completely for short durations. Note that sleeping
        // a negative amount is allowed and should return immediately, but
        // any small value will probably take at least 50us on Linux.
        if (wait > LIMITER_TWEAK) {
            std::this_thread::sleep_for(wait);
        }
    }        
    return duration;
}

float Window::aspect_ratio() const noexcept {
    if (!win_info_.height) return 0.0f;
    return static_cast<float>(win_info_.width) /
            static_cast<float>(win_info_.height);
}

} // namespace Greenbell

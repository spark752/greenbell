#ifndef GB_WINDOW_H
#define GB_WINDOW_H

#include "SDL2/SDL_video.h"
#include "gl.h"
#include <string>
#include <chrono>
#include <memory>

namespace Greenbell {

enum class MSAA { OFF, X2, X4, X8, X16 };

// Use whichever glad header was included to try and figure out a good default
// OpenGL version
#if defined(GL_VERSION_4_6)
inline constexpr auto OGL_MAJOR_DEFAULT = 4;
inline constexpr auto OGL_MINOR_DEFAULT = 6;
#elif defined(GL_VERSION_4_5)
inline constexpr auto OGL_MAJOR_DEFAULT = 4;
inline constexpr auto OGL_MINOR_DEFAULT = 5;
#elif defined(GL_VERSION_4_4)
inline constexpr auto OGL_MAJOR_DEFAULT = 4;
inline constexpr auto OGL_MINOR_DEFAULT = 4;
#elif defined(GL_VERSION_4_3)
inline constexpr auto OGL_MAJOR_DEFAULT = 4;
inline constexpr auto OGL_MINOR_DEFAULT = 3;
#else
inline constexpr auto OGL_MAJOR_DEFAULT = 3;
inline constexpr auto OGL_MINOR_DEFAULT = 3;
#endif

inline constexpr auto LIMITER_DISABLE = std::chrono::microseconds(0);
inline constexpr auto LIMITER_100_FPS = std::chrono::microseconds(10000);
inline constexpr auto LIMITER_200_FPS = std::chrono::microseconds(5000);
inline constexpr auto LIMITER_300_FPS = std::chrono::microseconds(3333);
inline constexpr auto LIMITER_400_FPS = std::chrono::microseconds(2500);
inline constexpr auto LIMITER_500_FPS = std::chrono::microseconds(2000);

struct WindowInfo {
    std::string title{"Greenbell"};
    int32_t width{1280};
    int32_t height{720};
    int32_t x{SDL_WINDOWPOS_UNDEFINED};
    int32_t y{SDL_WINDOWPOS_UNDEFINED};
    bool fullscreen{false};
    bool vsync{false};
    int32_t msaa{0};
    int32_t ogl_major{OGL_MAJOR_DEFAULT};
    int32_t ogl_minor{OGL_MINOR_DEFAULT};
};

class SDLWindowWrapper; // Forward reference

class Window {
  public:
    // Constructing a window will initialize SDL and OpenGL and SDL_ttf and
    // create the main application window. You should only create one of these.
    // Fill out a win_info struct and pass it to the constructor.
    Window(WindowInfo win_info);

    virtual ~Window();
    Window(const Window&) = delete;            // No copy
    Window& operator=(const Window&) = delete; // No copy assign
    Window(Window&&) = delete;                 // No move
    Window& operator=(Window&&) = delete;      // No move assign

    // Call at start of frame before rendering. Required so that frame time
    // can be measured for statistics and the soft frame limiter.
    void start_frame() const noexcept;

    // Call when finished rendering to swap the frame to output, invoke the
    // soft frame limiter (if min_duration > 0), and return frame duration.
    // The return duration is the time since the previous call to start_frame
    // not including any delay by the soft frame limiter. However this will
    // include the delay by vsync if it is enabled.
    std::chrono::microseconds end_frame(
            std::chrono::microseconds min_duration) const noexcept;

    // Get information about the window
    float aspect_ratio() const noexcept;
    int32_t width() const noexcept {return win_info_.width;}
    int32_t height() const noexcept {return win_info_.height;}
    int32_t x() const noexcept {return win_info_.x;}
    int32_t y() const noexcept {return win_info_.y;}

  protected:
    mutable std::chrono::time_point<std::chrono::steady_clock> time_point_;
    WindowInfo win_info_;
    std::unique_ptr<SDLWindowWrapper> ps_win_;
};

} // namespace Greenbell
#endif

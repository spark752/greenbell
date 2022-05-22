#ifndef GB_SDL_WRAPPERS_H
#define GB_SDL_WRAPPERS_H

#include "SDL2/SDL_ttf.h"
#include "log.h"
#include "sdl_exceptions.h"
#include <string>
#include <cstddef>

namespace Greenbell::SDL {

// Ownership of a TTF_Font pointer
class Font {
  public:
    // Default ctor can't do anything, but it is allowed for use in other
    // classes. No SDL or GL initialization is required.
    Font() noexcept = default;

    // Construct from an existing pointer. Takes ownership.
    Font(TTF_Font* p_font) noexcept : p_font_{p_font} {}

    // Construct by trying to open a font file and size. Attempts to initialize
    // the TTF engine if needed. If font loading fails the internal pointer will
    // be nullptr and an exception will be thrown.
    Font(const char* filename, int ptsize) {
        if (!TTF_WasInit()) {
            if (TTF_Init() != 0) throw Greenbell::TTFException();
        }
        p_font_ = TTF_OpenFont(filename, ptsize);
        if (p_font_ == nullptr) throw Greenbell::TTFException();
    }
    Font(const std::string& filename, int ptsize) noexcept :
            Font(filename.c_str(), ptsize) {}

    ~Font() noexcept {
        if (p_font_) TTF_CloseFont(p_font_);
    }

    Font(const Font&) = delete;              // No copy
    Font& operator=(const Font&) = delete;   // No copy assign
    Font(Font&& source) noexcept : p_font_{source.p_font_} {
        source.p_font_ = nullptr;
    } // Move
    Font& operator=(Font&& source) noexcept {
        if (&source == this) return *this; // Self assignment
        if (p_font_) TTF_CloseFont(p_font_);
        p_font_ = source.p_font_;
        source.p_font_ = nullptr;
        return *this;
    } // Move assign

    explicit operator bool() const {
        return p_font_;
    }
    TTF_Font* get() const noexcept {
        return p_font_;
    }

  private:
    TTF_Font* p_font_{nullptr}; // TTF code writes state so can't be const
};

// Ownership of an SDL Surface
class Surface {
  public:
    // Default ctor can't do anything, but it is allowed for use in other
    // classes. No SDL or GL initialization is required.
    Surface() noexcept = default;

    // Construct from an existing pointer. Takes ownership.
    Surface(SDL_Surface* p_surface) noexcept : p_surface_{p_surface} {}

    ~Surface() noexcept {
        if (p_surface_) SDL_FreeSurface(p_surface_);
    }

    Surface(const Surface&) = delete;              // No copy
    Surface& operator=(const Surface&) = delete;   // No copy assign
    Surface(Surface&& source) noexcept : p_surface_{source.p_surface_} {
        source.p_surface_ = nullptr;
    } // Move
    Surface& operator=(Surface&& source) noexcept {
        if (&source == this) return *this; // Self assignment
        if (p_surface_) SDL_FreeSurface(p_surface_);
        p_surface_ = source.p_surface_;
        source.p_surface_ = nullptr;
        return *this;
    } // Move assign

    explicit operator bool() const {
        return p_surface_;
    }

    SDL_Surface* get() const noexcept {
        return p_surface_;
    }

    int w() const noexcept {
        if (p_surface_) return p_surface_->w;
        return 0;
    }

    int h() const noexcept {
        if (p_surface_) return p_surface_->h;
        return 0;
    }

    int pitch() const noexcept {
        if (p_surface_) return p_surface_->pitch;
        return 0;
    }

    void* pixels() const noexcept {
        // SDL specifies a void pointer
        if (p_surface_) return p_surface_->pixels;
        return nullptr;
    }

  private:
    SDL_Surface* p_surface_{nullptr};
};

} // namespace Greenbell::SDL
#endif

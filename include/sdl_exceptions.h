#ifndef GB_SDL_EXCEPTIONS_H
#define GB_SDL_EXCEPTIONS_H

#include "SDL2/SDL_error.h"
#include "SDL2/SDL_ttf.h"
#include <stdexcept>

namespace Greenbell {

class SDLException : public std::runtime_error {
  public:
    SDLException() : std::runtime_error(SDL_GetError()){};
};

class TTFException : public std::runtime_error {
  public:
    TTFException() : std::runtime_error(TTF_GetError()){};
};

} // namespace Greenbell
#endif

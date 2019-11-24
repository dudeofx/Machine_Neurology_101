#ifndef __COUNTERMODULE_H__
#define __COUNTERMODULE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "tModules.h"

typedef struct _tCounter tCounter;
struct _tCounter {
   tModuleInstance _;
   int             type;
   int             window_id;
   int             count;
   SDL_Window     *window;
   SDL_Surface    *window_surface;
   TTF_Font       *font;
   SDL_Color       textcolor;
};

tModule *InitializeModule();
tModuleInstance *CreateInstance(tModule *mod);

#endif


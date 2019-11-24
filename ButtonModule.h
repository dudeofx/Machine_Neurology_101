#ifndef __BUTTONMODULE_H__
#define __BUTTONMODULE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "tModules.h"

typedef struct _tButton tButton;
struct _tButton {
   tModuleInstance  _;
   int              type;
   int              window_id;
   int              state;
   int              dirty;
   int              which;
   SDL_Window      *window;
   SDL_Surface     *window_surface;
   char             label[64];

   void           (*SetLabel)(void *obj, char *st);
};

tModule *InitializeModule();
tModuleInstance *CreateInstance(tModule *mod);

#endif


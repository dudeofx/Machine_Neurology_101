#include "CounterModule.h"

void _DefaultCounterHandler(tModuleInstance *self);
void CounterModule_Startup();

#define NUM_OF_RX_ENDP 1

tEndpDescriptor default_rx_endp_handlers[NUM_OF_RX_ENDP] = {
   {"RX-counter-trigger", {0, -1, NULL, _DefaultCounterHandler, NULL}},
};

//--------------------------------------------------------------------------
tModule *InitializeModule() {
   tModule *mod = NULL;

   mod = (tModule *) malloc(sizeof(tModule));
   mod->uid_counter = 1000;
   mod->resources = NULL;
   mod->instances = NULL;

   return mod;
}
//--------------------------------------------------------------------------
tModuleInstance *CreateInstance(tModule *mod) {
   tCounter *instance;

   instance = (tCounter *) malloc(sizeof(tCounter));
   tModuleInstance_Init(&(instance->_), mod, CounterModule_Startup, NUM_OF_RX_ENDP, default_rx_endp_handlers, 0, NULL);

   instance->type = o_tCounter;
   instance->window = NULL;
   instance->window_surface = NULL;
   instance->count = 0;
   
   return (tModuleInstance *) instance;
}
//--------------------------------------------------------------------------
void CounterModule_Startup(tModuleInstance *self) {
   tCounter *obj = (tCounter *) self;

   obj->window = SDL_CreateWindow("Counter Module", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 80, 40, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY | SDL_WINDOW_SKIP_TASKBAR);
   obj->window_surface = SDL_GetWindowSurface(obj->window);
   obj->textcolor.r = 0;
   obj->textcolor.g = 0;
   obj->textcolor.b = 0;
   obj->textcolor.a = 255;
   obj->font = TTF_OpenFont( "LiberationSans-Regular.ttf", 32);

   SDL_FillRect(obj->window_surface, NULL, SDL_MapRGB(obj->window_surface->format, 0x40, 0xFF, 0xFF ) );
   obj->window_id = SDL_GetWindowID(obj->window);
   SDL_UpdateWindowSurface(obj->window);
}
//--------------------------------------------------------------------------
void _DefaultCounterHandler(tModuleInstance *self) {
   char st[16];
   tCounter *obj = (tCounter *) self;
printf("counter up one\n");
   obj->count++;
   SDL_FillRect(obj->window_surface, NULL, SDL_MapRGB(obj->window_surface->format, 0xD0, 0xD0, 0xD0 ) );

   sprintf(st, "%i", obj->count);
   SDL_Surface *textSurface = TTF_RenderText_Blended(obj->font, st, obj->textcolor);
   SDL_BlitSurface(textSurface, NULL, obj->window_surface, NULL);
   SDL_FreeSurface(textSurface);

   SDL_UpdateWindowSurface(obj->window);
}
//--------------------------------------------------------------------------


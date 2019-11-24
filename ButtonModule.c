#include "ButtonModule.h"

void ButtonModule_Startup();

void _DefaultOnClick(tModuleInstance *);
void _DefaultEventHandler(tModuleInstance *, void *);
void _DefaultUpdate(tModuleInstance *);

#define NUM_OF_RX_ENDP 2

tEndpDescriptor default_rx_endp_handlers[NUM_OF_RX_ENDP] = {
   {"RX-event-handler", {0x6, -1, NULL, _DefaultEventHandler, NULL}},
   {"RX-update", {0, -1, NULL, _DefaultUpdate, NULL}},
};

#define NUM_OF_TX_ENDP 1

tEndpDescriptor default_tx_endp_handlers[NUM_OF_TX_ENDP] = {
   {"TX-onclick", {0, -1, NULL, _DefaultOnClick, NULL}},
};

typedef struct _tButtonResources tButtonResources;
struct _tButtonResources {
   SDL_Surface *btn_on;
   SDL_Surface *btn_off;
   SDL_Color    on_color;
   SDL_Color    off_color;
   TTF_Font    *font;
};

//--------------------------------------------------------------------------
static void SetLabel(void *obj, char *st) {
	tButton *button = (tButton *) obj;
	
	if (button == NULL) return;
	if (button->type != o_tButton) return;
    strcpy(button->label, st);
}
//--------------------------------------------------------------------------
tModule *InitializeModule() {
   tModule *mod;
   tButtonResources *res;
   
   res = (tButtonResources *) malloc(sizeof(tButtonResources));

   res->off_color.r = 20;
   res->off_color.g = 20;
   res->off_color.b = 20;
   res->off_color.a = 255;

   res->on_color.r = 200;
   res->on_color.g = 200;
   res->on_color.b = 200;
   res->on_color.a = 255;

   res->font = TTF_OpenFont( "LiberationSans-Regular.ttf", 16 );

   res->btn_on = SDL_LoadBMP("button_on.bmp");
   res->btn_off = SDL_LoadBMP("button_off.bmp");
   
   mod = (tModule *) malloc(sizeof(tModule));
   mod->uid_counter = 1000;
   mod->resources = (void *) res;
   mod->instances = NULL;

   return mod;
}
//--------------------------------------------------------------------------
tModuleInstance *CreateInstance(tModule *mod) {
   tButton *instance;
   int i;

   instance = (tButton *) malloc(sizeof(tButton));
   tModuleInstance_Init(&(instance->_), mod, ButtonModule_Startup, NUM_OF_RX_ENDP, default_rx_endp_handlers, NUM_OF_TX_ENDP, default_tx_endp_handlers);

   instance->type = o_tButton;
   instance->window = NULL;
   instance->window_surface = NULL;
   instance->SetLabel = SetLabel;
   strcpy(instance->label, "Button");
 
   return (tModuleInstance *) instance;
}
//--------------------------------------------------------------------------
void ButtonModule_Startup(tModuleInstance *self) {
   tButton *obj = (tButton *) self;
   tButtonResources *res = self->mod->resources;

   obj->window = SDL_CreateWindow("Button Module", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, res->btn_off->w, res->btn_on->h, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY | SDL_WINDOW_SKIP_TASKBAR);
   obj->window_surface = SDL_GetWindowSurface(obj->window);
   SDL_FillRect(obj->window_surface, NULL, SDL_MapRGB(obj->window_surface->format, 0xFF, 0x40, 0xFF ) );
   obj->window_id = SDL_GetWindowID(obj->window);
   obj->state = 0;
   obj->dirty = 1;
   obj->which = -1;


   SDL_BlitSurface(res->btn_off, NULL, obj->window_surface, NULL );
   SDL_UpdateWindowSurface(obj->window);
}
//--------------------------------------------------------------------------
void _DefaultOnClick(tModuleInstance *self) {
   tEndp *chain;
   tPulseWireHandler TriggerHandler;
   
   tButton *obj = (tButton *) self;
   if (obj->state == 1) printf("TX-onClick signal\n");

   chain = self->tx_endp_handlers[0].endp.next;
   while (chain != NULL) {
      TriggerHandler = (tPulseWireHandler) chain->func_handler;
      TriggerHandler(chain->owner);
      chain = chain->next;
   }
}
//--------------------------------------------------------------------------
void _DefaultEventHandler(tModuleInstance *self, void *data) {
   SDL_Event *e = (SDL_Event *) data;
   tButton *obj = (tButton *) self;
   int mouse_x, mouse_y, window_x, window_y, window_w, window_h;
   int outside_test;

   if (e->type == SDL_MOUSEBUTTONDOWN) {
      if (obj->window_id != e->button.windowID) return;
      if (e->button.button != SDL_BUTTON_LEFT) return;
      if (obj->which != -1) return;
      obj->which = e->button.which;
      obj->state = 1;
      obj->dirty = 1;
      return;
   }

   if (e->type == SDL_MOUSEBUTTONUP) {
      if (e->button.button != SDL_BUTTON_LEFT) return;
      if (obj->which != e->button.which) return;
      _DefaultOnClick(self);
      obj->which = -1;
      obj->state = 0;
      obj->dirty = 1;
      return;
   }

   if (e->type == SDL_MOUSEMOTION) {
      if (obj->which == -1) return; // mouse pointer hasn't been captured 
      if (obj->window_id != e->window.windowID) return;

      SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
      SDL_GetWindowPosition(obj->window, &window_x, &window_y);
      SDL_GetWindowSize(obj->window, &window_w, &window_h);

      outside_test = 0;
      if (mouse_x < window_x) outside_test = 1;
      if (mouse_y < window_y) outside_test = 1;
      if (mouse_x - window_x >= window_w) outside_test = 1;
      if (mouse_y - window_y >= window_h) outside_test = 1;

      if (outside_test == 0) { // inside
         if (obj->state != 1) {
            obj->dirty = 1;
            obj->state = 1;
         }
      } else {                 // outside
         if (obj->state != 0) {
            obj->dirty = 1;
            obj->state = 0;
         }
      }
      return;
   }
}
//--------------------------------------------------------------------------
void _DefaultUpdate(tModuleInstance *self) {
   tButton *obj = (tButton *) self;
   tButtonResources *res = self->mod->resources;
   SDL_Surface* textSurface;
   SDL_Rect txt_pos;

   if (obj->dirty == 0) return;

   if (obj->state == 0) {
      textSurface = TTF_RenderText_Blended(res->font, obj->label, res->off_color);
   } else {
      textSurface = TTF_RenderText_Blended(res->font, obj->label, res->on_color);
   }
   
   if (obj->state == 0) {
      SDL_BlitSurface(res->btn_off, NULL, obj->window_surface, NULL );
   } else {
      SDL_BlitSurface(res->btn_on, NULL, obj->window_surface, NULL );
   }

   txt_pos.w = textSurface->w;
   txt_pos.h = textSurface->h;
   txt_pos.x = (res->btn_on->w - textSurface->w) / 2;
   txt_pos.y = (res->btn_on->h - textSurface->h) / 2;

   SDL_BlitSurface(textSurface, NULL, obj->window_surface, &txt_pos);
   SDL_FreeSurface(textSurface);

   SDL_UpdateWindowSurface(obj->window);
   obj->dirty = 0;
}
//--------------------------------------------------------------------------


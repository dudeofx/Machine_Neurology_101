#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "tModules.h"
#include "ButtonModule.h"
#include "CounterModule.h"

//--------------------------------------------------------------
int Fail(int line) {
   printf("error! line#=%d\n", line);
   return -1;
}
//--------------------------------------------------------------
int main() {
   char *error;

   
   //---[Init SDL stuff]-------------------------------------
   if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
      printf("e: %s\n", SDL_GetError());
      return Fail(__LINE__);
   }
   SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
   if (TTF_Init() < 0) return Fail(__LINE__);

   //---[load modules]-------------------------------------
   void *btn_dl = dlopen ("./btnmod.so", RTLD_NOW);
   if (btn_dl == NULL) return Fail(__LINE__);
   void *cnt_dl = dlopen("./cntmod.so", RTLD_NOW);
   if (cnt_dl == NULL) return Fail(__LINE__);


   //---[initialize modules]-------------------------------
   tModule *(*InitializeModule)();

   InitializeModule = dlsym(btn_dl, "InitializeModule");
   error = dlerror();
   if (error != NULL) return Fail(__LINE__);
   tModule *button_module = InitializeModule();
   if (button_module == NULL) return Fail(__LINE__);

   InitializeModule = dlsym(cnt_dl, "InitializeModule");
   error = dlerror();
   if (error != NULL) return Fail(__LINE__);
   tModule *counter_module = InitializeModule();
   if (counter_module == NULL) return Fail(__LINE__);

   //---[create instance objects]--------------------------
   tModuleInstance *(*CreateInstance)(tModule *mod);

   CreateInstance = dlsym(btn_dl, "CreateInstance");
   error = dlerror();
   if (error != NULL) return Fail(__LINE__);
   tModuleInstance *btn_instance = CreateInstance(button_module);
   if (btn_instance == NULL) return Fail(__LINE__);
   ((tButton *) btn_instance)->SetLabel(btn_instance, "Click Here");

   CreateInstance = dlsym(cnt_dl, "CreateInstance");
   error = dlerror();
   if (error != NULL) return Fail(__LINE__);
   tModuleInstance *cnt_instance = CreateInstance(counter_module);
   if (cnt_instance == NULL) return Fail(__LINE__);

   //---[bind endpoints]-----------------------------------
   tEndpJunctionBox *jbox = CreateEndpJunctionBox();   
   BindWire(jbox, btn_instance, "TX-onclick", cnt_instance, "RX-counter-trigger");

   tStaticDataWireHandler ButtonEventHandler;
   ButtonEventHandler = (tStaticDataWireHandler) GetWireHandler(btn_instance, "RX-event-handler");

   tPulseWireHandler ButtonUpdate;
   ButtonUpdate = (tPulseWireHandler) GetWireHandler(btn_instance, "RX-update");

   //---[Startup instance]---------------------------------
   cnt_instance->Startup(cnt_instance);
   btn_instance->Startup(btn_instance);

   //---[work is performed]--------------------------------

   int quit = 0;
   SDL_Event e;
   while(quit == 0 ) {
      while( SDL_PollEvent( &e ) != 0 ) {
         if( e.type == SDL_QUIT ) quit = 1;
         ButtonEventHandler(btn_instance, &e);
      }
      ButtonUpdate(btn_instance);
   }

   //---[Shutdown modules]---------------------------------

   //---[Release endpoints]--------------------------------

   //---[destroy instance objects]-------------------------

   //---[shutdown modules]---------------------------------

   //---[unload modules]-----------------------------------

   return 0;
}


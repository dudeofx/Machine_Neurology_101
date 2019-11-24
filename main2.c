// proof of concept of many senders to one receiver

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "tModules.h"
#include "ButtonModule.h"

#define NUM_BUTTONS 5

//--------------------------------------------------------------
int Fail(int line) {
   printf("error! line#=%d\n", line);
   return -1;
}
//--------------------------------------------------------------
int main() {
   char *error;
   int i;
   char st[64];

   
   //---[Init SDL stuff]-------------------------------------
   if (SDL_Init(SDL_INIT_VIDEO) < 0) return Fail(__LINE__);
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

   tModuleInstance *btns[NUM_BUTTONS];

   for (i = 0; i < NUM_BUTTONS; i++) {
      btns[i] = CreateInstance(button_module);
      if (btns[i] == NULL) return Fail(__LINE__);
      sprintf(st, "Button %i", i);
      ((tButton *) btns[i])->SetLabel(btns[i], st);   
   }

   CreateInstance = dlsym(cnt_dl, "CreateInstance");
   error = dlerror();
   if (error != NULL) return Fail(__LINE__);
   tModuleInstance *cnt_instance = CreateInstance(counter_module);
   if (cnt_instance == NULL) return Fail(__LINE__);

   //---[bind endpoints]-----------------------------------
   tEndpJunctionBox *jbox = CreateEndpJunctionBox();   
   for (i = 0; i < NUM_BUTTONS; i++) {
      BindWire(jbox, btns[i], "TX-onclick", cnt_instance, "RX-counter-trigger");
   }

   tStaticDataWireHandler BtnEventMules[NUM_BUTTONS];
   tPulseWireHandler BtnUpdateMules[NUM_BUTTONS];

   for (i = 0; i < NUM_BUTTONS; i++) {
      BtnEventMules[i] = (tStaticDataWireHandler) GetWireHandler(btns[i], "RX-event-handler");
      BtnUpdateMules[i] = (tPulseWireHandler) GetWireHandler(btns[i], "RX-update");
   }


   //---[Startup instance]---------------------------------
   cnt_instance->Startup(cnt_instance);
   for (i = 0; i < NUM_BUTTONS; i++) {
      btns[i]->Startup(btns[i]);
   }

   int quit = 0;
   SDL_Event e;
   while(quit == 0 ) {
      while( SDL_PollEvent( &e ) != 0 ) {
         if( e.type == SDL_QUIT ) quit = 1;
         for (i = 0; i < NUM_BUTTONS; i++) BtnEventMules[i](btns[i], &e);
      }
      for (i = 0; i < NUM_BUTTONS; i++) BtnUpdateMules[i](btns[i]);
   }

   //---[work is performed]--------------------------------

   //---[Shutdown modules]---------------------------------

   //---[Release endpoints]--------------------------------

   //---[destroy instance objects]-------------------------

   //---[shutdown modules]---------------------------------

   //---[unload modules]-----------------------------------

   return 0;
}


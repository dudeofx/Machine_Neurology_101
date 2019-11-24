#include <stdio.h>
#include <string.h>

#include "tModules.h"

//--------------------------------------------------------------------------
tEndpJunctionBox *CreateEndpJunctionBox() {
   tEndpJunctionBox *tmp;
   tmp = (tEndpJunctionBox *) malloc(sizeof(tEndpJunctionBox));
   tmp->uid = 1000;
   tmp->root = NULL;

   return tmp;
}
//--------------------------------------------------------------------------
int BindWire(tEndpJunctionBox *jbox, tModuleInstance *tx_instance, char *tx_signal, tModuleInstance *rx_instance, char *rx_signal) {
   tEndp *endp;
   int wire;

   endp = GetEndPoint(rx_instance, rx_signal);
   if (endp == NULL) return -1;
   wire = jbox->uid++;
   AddEndPoint(tx_instance, tx_signal, endp, wire);
   return wire;
}
//--------------------------------------------------------------------------
void *GetWireHandler(tModuleInstance *mod, char *signal) {
   int i;
   for (i = 0; i < mod->num_of_rx_endp; i++) {
      if (strcmp(signal, mod->rx_endp_handlers[i].signal) == 0) {
         return mod->rx_endp_handlers[i].endp.func_handler;
      }
   }
   return NULL;
}
//--------------------------------------------------------------------------
tEndp *GetEndPoint(tModuleInstance *mod, char *signal) {
   int i;
   for (i = 0; i < mod->num_of_rx_endp; i++) {
      if (strcmp(signal, mod->rx_endp_handlers[i].signal) == 0) {
         return &(mod->rx_endp_handlers[i].endp);
      }
   }
   return NULL;
}
//--------------------------------------------------------------------------
int AddEndPoint(tModuleInstance *mod, char *signal, tEndp *endp, int wire) {
   int i;
   tEndp *tmp = NULL;
   tEndp *tail;

   for (i = 0; i < mod->num_of_tx_endp; i++) {
      if (strcmp(signal, mod->tx_endp_handlers[i].signal) == 0) {
         tail = &(mod->tx_endp_handlers[i].endp);
         while (tail->next != NULL) tail = tail->next;

         tmp = (tEndp *) malloc(sizeof(tEndp));
         tmp->owner = endp->owner;
         tmp->type = endp->type;
         tmp->wire = wire;
         tmp->func_handler = endp->func_handler;
         tmp->next = NULL; 

         tail->next = tmp;
         return wire;
      }
   }
   return -1;
}
//--------------------------------------------------------------------------
void DelEndPoint(tModuleInstance *mod, char *signal, int endp_id) {
   if (strcmp(signal, "TX-onclick") == 0) {
      // onclick handler is removed
   }
}
//--------------------------------------------------------------------------
void ClrEndPoint(tModuleInstance *mod, char *signal) {
   if (strcmp(signal, "TX-onclick") == 0) {
      // all onclick handlers are removed
   }
}
//--------------------------------------------------------------------------
void EndpInvoke(tModuleInstance *mod, char *signal) {
}
//--------------------------------------------------------------------------
void AddInstanceToModule(tModule *mod, tModuleInstance *instance) {
   tModuleInstance *tail;

   tail = mod->instances;
   if (tail == NULL) {
      mod->instances = instance;
   } else {
      while (tail->next != NULL) tail = tail->next;
      tail->next = instance;
   }
   
}
//--------------------------------------------------------------------------
void tModuleInstance_Init(tModuleInstance *obj, tModule *mod, void (*startup_func)(tModuleInstance *self), int num_rx, tEndpDescriptor *rx_endp, int num_tx, tEndpDescriptor *tx_endp) {
   int i;
   
   obj->instance_id = mod->uid_counter++;
   obj->type = o_tModuleInstance;
   obj->mod = mod;
   obj->next = NULL;
   obj->Startup = startup_func;
   AddInstanceToModule(mod, obj);

   // initilize receive endpoints
   obj->num_of_rx_endp = num_rx;
   obj->rx_endp_handlers = NULL;
   if (num_rx > 0) {
      obj->rx_endp_handlers = malloc(sizeof(tEndpDescriptor)*num_rx);
      memcpy(obj->rx_endp_handlers, rx_endp, sizeof(tEndpDescriptor)*num_rx);
      for (i = 0; i < num_rx; i++) {
         obj->rx_endp_handlers[i].endp.owner = obj;
      }
   }

   // initilize transmit endpoints
   obj->num_of_tx_endp = num_tx;
   obj->tx_endp_handlers = NULL;
   if (num_tx > 0) {
      obj->tx_endp_handlers = malloc(sizeof(tEndpDescriptor)*num_tx);
      memcpy(obj->tx_endp_handlers, tx_endp, sizeof(tEndpDescriptor)*num_tx);
      for (i = 0; i < num_tx; i++) {
         obj->tx_endp_handlers[i].endp.owner = obj;
      }
   }
}
//--------------------------------------------------------------------------

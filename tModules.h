#ifndef __TMODULES_H__
#define __TMODULES_H__

#include <stdlib.h>
#include "oTypes.h"

typedef struct _tEndp tEndp;
typedef struct _tEndpDescriptor tEndpDescriptor;
typedef struct _tModuleInstance tModuleInstance;
typedef struct _tModule tModule;
typedef struct _tEndpJunction tEndpJunction;
typedef struct _tEndpJunctionBox tEndpJunctionBox;

struct _tEndp {
   int    type;
   int    wire;
   void  *owner;
   void  *func_handler;
   tEndp *next;
};

struct _tEndpDescriptor {
   char *signal;
   tEndp endp;
};

struct _tModuleInstance {
   int              instance_id;
   int              type;
   tModule         *mod;
   int              num_of_rx_endp;
   tEndpDescriptor *rx_endp_handlers;
   int              num_of_tx_endp;
   tEndpDescriptor *tx_endp_handlers;
   tModuleInstance *next;

   void (*Startup)(tModuleInstance *self);
};

struct _tModule {
   int uid_counter;
   void *resources;
   tModuleInstance *instances;
};

struct _tEndpJunction {
   tModuleInstance *tx_instance;
   char            *tx_signal;
   tModuleInstance *rx_instance;
   char            *rx_signal;
   int              wire;
   void            *func_handler;
   tEndpJunction   *next;
};

struct _tEndpJunctionBox {
   int uid;
   tEndpJunction *root;
};


//----[standard types for endpoint function handlers]-----------------
typedef void (*tPulseWireHandler)(tModuleInstance *);
typedef void (*tBooleanWireHandler)(tModuleInstance *, int);
typedef void (*tIntegerWireHandler)(tModuleInstance *, int);
typedef void (*tLongWireHandler)(tModuleInstance *, long);
typedef void (*tFloatWireHandler)(tModuleInstance *, float);
typedef void (*tDoubleWireHandler)(tModuleInstance *, double);
typedef void (*tStaticStringWireHandler)(tModuleInstance *, char *);
typedef void (*tHeapStringWireHandler)(tModuleInstance *, char *);
typedef void (*tStaticDataWireHandler)(tModuleInstance *, void *);
typedef void (*tHeapDataWireHandler)(tModuleInstance *, void *);
//--------------------------------------------------------------------

tEndpJunctionBox *CreateEndpJunctionBox();
int    BindWire(tEndpJunctionBox *jbox, tModuleInstance *tx_instance, char *tx_signal, tModuleInstance *rx_instance, char *rx_signal);
void  *GetWireHandler(tModuleInstance *mod, char *signal);
void   AddInstanceToModule(tModule *mod, tModuleInstance *instance);

tEndp *GetEndPoint(tModuleInstance *mod, char *signal);
int    AddEndPoint(tModuleInstance *mod, char *signal, tEndp *endp, int wire);
void   DelEndPoint(tModuleInstance *mod, char *signal, int wire);
void   ClrEndPoint(tModuleInstance *mod, char *signal);

void   tModuleInstance_Init(tModuleInstance *obj, tModule *mod, void (*startup_func)(tModuleInstance *self), int num_rx, tEndpDescriptor *rx_endp, int num_tx, tEndpDescriptor *tx_endp);

#endif


// standard endpoint types:
//   0x00000000 - Pulse (void)
//   0x00000001 - Boolean (0 and 1, int)
//   0x00000002 - Integer (int)
//   0x00000003 - Long (long int)
//   0x00000004 - Float (float)
//   0x00000005 - Double (double)
//   0x00000006 - StaticString (char *)
//   0x80000006 - HeapString (char *)
//   0x00000007 - StaticData (int, char *)
//   0x80000007 - HeapData (int, char *)


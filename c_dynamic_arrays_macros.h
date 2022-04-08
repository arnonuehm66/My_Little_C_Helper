/*******************************************************************************
 ** Name: c_dynamic_arrays_macros.h
 ** Purpose:  Provides dynamic arrays as macros.
 ** Author: (JE) Jens Elstner
 ** Version: v0.1.2
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 11.04.2021  JE    Created lib.
 ** 19.04.2021  JE    Renamed 'ptArray' to 'tArray'.
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_DYNAMIC_ARRAYS_MACROS_H
#define C_DYNAMIC_ARRAYS_MACROS_H


//******************************************************************************
//* includes

#include <stdlib.h>


//******************************************************************************
//* defines and macros

#define C_DYNAMIC_ARRAYS_INITIAL_CAPACITY 256

//******************************************************************************
//* How To use:
//*-------------
//* Do not use spaces in type like 'unsigned int'!
//* Use either a 'typedef unsigned int ui;' and then use 'ui' as type,
//* or use one of the definitions from 'stdint.h' like 'uint32_t'.
//*
//* Create the struct like this in your main.c file, once per type.
//*
//*   s_array(uint32_t);
//*   s_array(float);
//*
//* After that, you can use 't_array(uint32_t)' for varaible declarations and
//* as arguments in functions like:
//*
//*   int myFunction(t_array(uint32_t) myDa) { ... }
//*
//*   t_array(uint32_t) myDa;
//*
//* Then your're up and running with the usage of all macros creating, adding
//* and freeing the dynamic arrays;
//*
//*   daInit(uint32_t, myDa);
//*
//*   daAdd(uint32_t, myDa, 1);
//*   daAdd(uint32_t, myDa, 10);
//*   daAdd(uint32_t, myDa, 100);
//*
//*   uint32_t Variable = myDa.pVal[2];
//*
//*   int rv = myFunction(myDa);
//*
//*   daClear(uint32_t, myDa);
//*
//*   daFree(myDa);
//*
//* If there is a compund variable including a pointer like:
//*
//*   s_array(cstr);
//*
//* Use it as:
//*
//*   t_array(cstr) myDa;
//*
//*   daInit(cstr, myDa);
//*
//*   daAdd(cstr, myDa, csNew("Eins"));
//*   daAdd(cstr, myDa, csNew("Zwei"));
//*   daAdd(cstr, myDa, csNew("Drei"));
//*
//* Last argument is the name of the internal pointer (one level beyond)
//* to be freed prior dynamic array pointer.
//*
//*   daFreeEx(myDa, cStr);
//*
//* If a pointer is needed use it like this:
//*
//*   int myFunction(t_array(uint32_t)* myDa) {
//*     daAdd(uint32_t, (*myDa), 1);
//*     return 1;
//*   }
//*
//******************************************************************************



//******************************************************************************
//* struct_type definition

#define s_array(type) struct _s_array_ ## type { \
  type* pVal; \
  size_t sCount; \
  size_t sCapacity; \
}

#define t_array(type) struct _s_array_ ## type


//******************************************************************************
//* int

/*******************************************************************************
 * Name:  daInit
 * Purpose: Initialze dynamic array of type.
 *******************************************************************************/
#define daInit(type, tArray) { \
  tArray.sCount    = 0; \
  tArray.sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY; \
  tArray.pVal      = (type*) malloc(sizeof(type) * tArray.sCapacity); \
}

/*******************************************************************************
 * Name:  daAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
#define daAdd(type, tArray, value) { \
  if (tArray.sCount + 1 > tArray.sCapacity) { \
    tArray.sCapacity *= 2; \
    tArray.pVal       = (type*) realloc(tArray.pVal, sizeof(type) * tArray.sCapacity); \
  } \
  tArray.pVal[tArray.sCount++] = value; \
}

/*******************************************************************************
 * Name:  daFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
#define daFree(tArray) { \
  if (tArray.pVal != NULL) free(tArray.pVal); \
}

/*******************************************************************************
 * Name:  daClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
#define daClear(type, tArray) { \
  daFree(tArray); \
  daInit(type, tArray); \
}

/*******************************************************************************
 * Name:  daFreeEx
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
#define daFreeEx(tArray, pointer) { \
  for (int i = 0; i < tArray.sCount; ++i) \
    if (tArray.pVal[i].pointer != NULL) free(tArray.pVal[i].pointer); \
  if (tArray.pVal != NULL) free(tArray.pVal); \
}

/*******************************************************************************
 * Name:  daClearEx
 * Purpose: Reset dynamic array.
 *******************************************************************************/
#define daClearEx(type, tArray, pointer) { \
  daFreeEx(tArray, pointer); \
  daInit(type, tArray); \
}


#endif // C_DYNAMIC_ARRAYS_MACROS_H

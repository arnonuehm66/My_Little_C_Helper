/*******************************************************************************
 ** Name: c_dynamic_array_macros.h
 ** Purpose:  Provides dynamic arrays as macros.
 ** Author: (JE) Jens Elstner
 ** Version: v0.1.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 11.04.2021  JE    Created lib.
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
//* Use either a 'typedef ui unsigned int;' and then use 'ui' as type,
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
//* Then your're up and runnug with the usage of all macros creating, adding
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
//*   daInit(myType, myDa);
//*
//*     daAdd(cstr, my_dacs, csNew("Eins"));
//*     daAdd(cstr, my_dacs, csNew("Zwei"));
//*     daAdd(cstr, my_dacs, csNew("Drei"));
//*
//* Last argument is the name of the internal pointer (one level beyond)
//* to be freed prior dynamic array pointer.
//*
//*   daFreeEx(myDa, cStr);
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
 * Name:  daiInit
 * Purpose: Initialze dynamic array of type.
 *******************************************************************************/
#define daInit(type, ptArray) { \
  ptArray.sCount    = 0; \
  ptArray.sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY; \
  ptArray.pVal      = (type*) malloc(sizeof(type) * ptArray.sCapacity); \
}

/*******************************************************************************
 * Name:  daAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
#define daAdd(type, ptArray, value) { \
  if (ptArray.sCount + 1 > ptArray.sCapacity) { \
    ptArray.sCapacity *= 2; \
    ptArray.pVal       = (type*) realloc(ptArray.pVal, sizeof(type) * ptArray.sCapacity); \
  } \
  ptArray.pVal[ptArray.sCount++] = value; \
}

/*******************************************************************************
 * Name:  daFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
#define daFree(ptArray) { \
  if (ptArray.pVal != NULL) free(ptArray.pVal); \
}

/*******************************************************************************
 * Name:  daClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
#define daClear(type, ptArray) { \
  daFree(ptArray); \
  daInit(type, ptArray); \
}

/*******************************************************************************
 * Name:  daFreeEx
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
#define daFreeEx(ptArray, pointer) { \
  for (int i = 0; i < ptArray.sCount; ++i) \
    if (ptArray.pVal[i].pointer != NULL) free(ptArray.pVal[i].pointer); \
  if (ptArray.pVal != NULL) free(ptArray.pVal); \
}

/*******************************************************************************
 * Name:  daClearEx
 * Purpose: Reset dynamic array.
 *******************************************************************************/
#define daClearEx(type, ptArray, pointer) { \
  daFreeEx(ptArray, pointer); \
  daInit(type, ptArray); \
}


#endif // C_DYNAMIC_ARRAYS_H

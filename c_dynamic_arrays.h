/*******************************************************************************
 ** Name: c_dynamic_array.h
 ** Purpose:  Provides differend types of dynamic arrays.
 ** Author: (JE) Jens Elstner
 ** Version: v0.2.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 09.02.2018  JE    Created lib.
 ** 21.02.2018  JE    Changed 'cstr' to 'const char' in 'dacsAdd()'.
 ** 22.02.2018  JE    Added 'Capacity' to all structs.
 ** 22.02.2018  JE    Now reallocate just when capacity is full in 'daXAdd()'.
 ** 22.02.2018  JE    Added daXFree() functions.
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_DYNAMIC_ARRAYS_H
#define C_DYNAMIC_ARRAYS_H


//******************************************************************************
//* includes

#include <stdlib.h>


//******************************************************************************
//* defines and macros

#define C_DYNAMIC_ARRAYS_INITIAL_CAPACITY 256


//******************************************************************************
//* type definition

typedef struct {
  cstr* pcsData;
  int   iCount;
  int   iCapacity;
} t_array_cstr;

typedef struct {
  int* piData;
  int  iCount;
  int  iCapacity;
} t_array_int;

typedef struct {
  unsigned int* puiData;
  unsigned int  uiCount;
  unsigned int  uiCapacity;
} t_array_uint;


//******************************************************************************
//* function forward declarations
//* Makes for a better function's arrangement.

// cstr
void dacsInit(t_array_cstr* ptArray);
void dacsAdd(t_array_cstr* ptArray, const char* pcValue);
void dacsClear(t_array_cstr* ptArray);
void dacsFree(t_array_cstr* ptArray);

// int
void daiInit(t_array_int* ptArray);
void daiAdd(t_array_int* ptArray, int iValue);
void daiClear(t_array_int* ptArray);
void daiFree(t_array_int* ptArray);

// unsigned int
void dauiInit(t_array_uint* ptArray);
void dauiAdd(t_array_uint* ptArray, unsigned int uiValue);
void dauiClear(t_array_uint* ptArray);
void dauiFree(t_array_uint* ptArray);


//******************************************************************************
//* cstr

/*******************************************************************************
 * Name:  dacsInit
 * Purpose: Initialze dynamic array of cstr.
 *******************************************************************************/
void dacsInit(t_array_cstr* ptArray) {
  ptArray->iCount    = 0;
  ptArray->iCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->pcsData   = (cstr*) malloc(sizeof(cstr) * ptArray->iCapacity);
}

/*******************************************************************************
 * Name:  dacsAdd
 * Purpose: Adds a cstr value to a dynamic array.
 *******************************************************************************/
void dacsAdd(t_array_cstr* ptArray, const char *pcValue) {
  // Check and double cap, if necessary.
  if (ptArray->iCount + 1 > ptArray->iCapacity) {
    ptArray->iCapacity *= 2;
    ptArray->pcsData    = (cstr*) realloc(ptArray->pcsData, sizeof(cstr) * ptArray->iCapacity);
  }

  // Set value in next slot and increment counter.
  csSet(&ptArray->pcsData[ptArray->iCount++], pcValue);
}


/*******************************************************************************
 * Name:  dacsClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
void dacsClear(t_array_cstr* ptArray) {
  dacsFree(ptArray);
  dacsInit(ptArray);
}

/*******************************************************************************
 * Name:  dacsFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
void dacsFree(t_array_cstr* ptArray) {
  if (ptArray->pcsData != NULL)
    free(ptArray->pcsData);
}


//******************************************************************************
//* int

/*******************************************************************************
 * Name:  daiInit
 * Purpose: Initialze dynamic array of int.
 *******************************************************************************/
void daiInit(t_array_int* ptArray) {
  ptArray->iCount    = 0;
  ptArray->iCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->piData    = (int*) malloc(sizeof(int) * ptArray->iCapacity);
}

/*******************************************************************************
 * Name:  daiAdd
 * Purpose: Adds a cstr value to a dynamic array.
 *******************************************************************************/
void daiAdd(t_array_int* ptArray, int iValue) {
  // Check and double cap, if necessary.
  if (ptArray->iCount + 1 > ptArray->iCapacity) {
    ptArray->iCapacity *= 2;
    ptArray->piData     = (int*) realloc(ptArray->piData, sizeof(int) * ptArray->iCapacity);
  }

  // Set value in next slot and increment counter.
  ptArray->piData[ptArray->iCount++] = iValue;
}

/*******************************************************************************
 * Name:  daiClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
void daiClear(t_array_int* ptArray) {
  daiFree(ptArray);
  daiInit(ptArray);
}

/*******************************************************************************
 * Name:  daiFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
void daiFree(t_array_int* ptArray) {
  if (ptArray->piData != NULL)
    free(ptArray->piData);
}


//******************************************************************************
//* unsigned int

/*******************************************************************************
 * Name:  dauiInit
 * Purpose: Initialze dynamic array of int.
 *******************************************************************************/
void dauiInit(t_array_uint* ptArray) {
  ptArray->uiCount    = 0;
  ptArray->uiCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->puiData    = (unsigned int*) malloc(sizeof(unsigned int) * ptArray->uiCapacity);
}

/*******************************************************************************
 * Name:  dauiAdd
 * Purpose: Adds a cstr value to a dynamic array.
 *******************************************************************************/
void dauiAdd(t_array_uint* ptArray, unsigned int uiValue) {
  // Check and double cap, if necessary.
  if (ptArray->uiCount + 1 > ptArray->uiCapacity) {
    ptArray->uiCapacity *= 2;
    ptArray->puiData     = (unsigned int*) realloc(ptArray->puiData, sizeof(unsigned int) * ptArray->uiCapacity);
  }

  // Set value in next slot and increment counter.
  ptArray->puiData[ptArray->uiCount++] = uiValue;
}

/*******************************************************************************
 * Name:  dauiClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
void dauiClear(t_array_uint* ptArray) {
  dauiFree(ptArray);
  dauiInit(ptArray);
}

/*******************************************************************************
 * Name:  dauiFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
void dauiFree(t_array_uint* ptArray) {
  if (ptArray->puiData != NULL)
    free(ptArray->puiData);
}


#endif // C_DYNAMIC_ARRAYS_H


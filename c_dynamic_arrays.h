/*******************************************************************************
 ** Name: c_dynamic_array.h
 ** Purpose:  Provides differend types of dynamic arrays.
 ** Author: (JE) Jens Elstner
 ** Version: v0.4.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 09.02.2018  JE    Created lib.
 ** 21.02.2018  JE    Changed 'cstr' to 'const char' in 'dacsAdd()'.
 ** 22.02.2018  JE    Added 'Capacity' to all structs.
 ** 22.02.2018  JE    Now reallocate just when capacity is full in 'daXAdd()'.
 ** 22.02.2018  JE    Added daXFree() functions.
 ** 07.03.2019  JE    Now structs are all named.
 ** 18.04.2019  JE    Added dabXXX() functions for byte handling.
 ** 23.04.2019  JE    Changed xCount and xCapacity vars form int to size_t.
 ** 23.04.2019  JE    Changed xData vars to appropriate names.
 ** 27.03.2020  JE    Added dasXXX() functions for size_t handling.
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_DYNAMIC_ARRAYS_H
#define C_DYNAMIC_ARRAYS_H


//******************************************************************************
//* includes

#include <stdlib.h>
#include "c_string.h"


//******************************************************************************
//* defines and macros

#define C_DYNAMIC_ARRAYS_INITIAL_CAPACITY 256


//******************************************************************************
//* type definition

typedef struct s_array_cstr {
  cstr*  pStr;
  size_t sCount;
  size_t sCapacity;
} t_array_cstr;

typedef struct s_array_int {
  int*   pInt;
  size_t sCount;
  size_t sCapacity;
} t_array_int;

typedef struct s_array_uint {
  unsigned int* pUInt;
  size_t        sCount;
  size_t        sCapacity;
} t_array_uint;

typedef struct s_array_byte {
  unsigned char* pBytes;
  size_t         sCount;
  size_t         sCapacity;
} t_array_byte;

typedef struct s_array_size {
  size_t* pSize;
  size_t  sCount;
  size_t  sCapacity;
} t_array_size;


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

// byte aka unsigned char
void dabInit(t_array_byte* ptArray);
void dabAdd(t_array_byte* ptArray, unsigned char bValue);
void dabClear(t_array_byte* ptArray);
void dabFree(t_array_byte* ptArray);

// size_t
void dasInit(t_array_size* ptArray);
void dasAdd(t_array_size* ptArray, size_t bValue);
void dasClear(t_array_size* ptArray);
void dasFree(t_array_size* ptArray);


//******************************************************************************
//* cstr

/*******************************************************************************
 * Name:  dacsInit
 * Purpose: Initialze dynamic array of cstr.
 *******************************************************************************/
void dacsInit(t_array_cstr* ptArray) {
  ptArray->sCount    = 0;
  ptArray->sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->pStr      = (cstr*) malloc(sizeof(cstr) * ptArray->sCapacity);
}

/*******************************************************************************
 * Name:  dacsAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
void dacsAdd(t_array_cstr* ptArray, const char *pcValue) {
  // Check and double cap, if necessary.
  if (ptArray->sCount + 1 > ptArray->sCapacity) {
    ptArray->sCapacity *= 2;
    ptArray->pStr       = (cstr*) realloc(ptArray->pStr, sizeof(cstr) * ptArray->sCapacity);
  }

  // Set value in next slot and increment counter.
  csSet(&ptArray->pStr[ptArray->sCount++], pcValue);
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
  if (ptArray->pStr != NULL)
    free(ptArray->pStr);
}


//******************************************************************************
//* int

/*******************************************************************************
 * Name:  daiInit
 * Purpose: Initialze dynamic array of int.
 *******************************************************************************/
void daiInit(t_array_int* ptArray) {
  ptArray->sCount    = 0;
  ptArray->sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->pInt      = (int*) malloc(sizeof(int) * ptArray->sCapacity);
}

/*******************************************************************************
 * Name:  daiAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
void daiAdd(t_array_int* ptArray, int iValue) {
  // Check and double cap, if necessary.
  if (ptArray->sCount + 1 > ptArray->sCapacity) {
    ptArray->sCapacity *= 2;
    ptArray->pInt       = (int*) realloc(ptArray->pInt, sizeof(int) * ptArray->sCapacity);
  }

  // Set value in next slot and increment counter.
  ptArray->pInt[ptArray->sCount++] = iValue;
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
  if (ptArray->pInt != NULL)
    free(ptArray->pInt);
}


//******************************************************************************
//* unsigned int

/*******************************************************************************
 * Name:  dauiInit
 * Purpose: Initialze dynamic array of ubsigned int.
 *******************************************************************************/
void dauiInit(t_array_uint* ptArray) {
  ptArray->sCount    = 0;
  ptArray->sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->pUInt     = (unsigned int*) malloc(sizeof(unsigned int) * ptArray->sCapacity);
}

/*******************************************************************************
 * Name:  dauiAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
void dauiAdd(t_array_uint* ptArray, unsigned int uiValue) {
  // Check and double cap, if necessary.
  if (ptArray->sCount + 1 > ptArray->sCapacity) {
    ptArray->sCapacity *= 2;
    ptArray->pUInt      = (unsigned int*) realloc(ptArray->pUInt, sizeof(unsigned int) * ptArray->sCapacity);
  }

  // Set value in next slot and increment counter.
  ptArray->pUInt[ptArray->sCount++] = uiValue;
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
  if (ptArray->pUInt != NULL)
    free(ptArray->pUInt);
}

//******************************************************************************
//* byte aka unsigned char

/*******************************************************************************
 * Name:  dabInit
 * Purpose: Initialze dynamic array of bytes.
 *******************************************************************************/
void dabInit(t_array_byte *ptArray) {
  ptArray->sCount    = 0;
  ptArray->sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->pBytes    = (unsigned char*) malloc(sizeof(unsigned char) * ptArray->sCapacity);
}

/*******************************************************************************
 * Name:  dabAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
void dabAdd(t_array_byte *ptArray, unsigned char bValue) {
  // Check and double cap, if necessary.
  if (ptArray->sCount + 1 > ptArray->sCapacity) {
    ptArray->sCapacity *= 2;
    ptArray->pBytes     = (unsigned char*) realloc(ptArray->pBytes, sizeof(unsigned char) * ptArray->sCapacity);
  }

  // Set value in next slot and increment counter.
  ptArray->pBytes[ptArray->sCount++] = bValue;
}

/*******************************************************************************
 * Name:  dabClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
void dabClear(t_array_byte *ptArray) {
  dabFree(ptArray);
  dabInit(ptArray);
}

/*******************************************************************************
 * Name:  dabFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
void dabFree(t_array_byte *ptArray) {
  if (ptArray->pBytes != NULL)
    free(ptArray->pBytes);
}

//******************************************************************************
//* size_t

/*******************************************************************************
 * Name:  dasInit
 * Purpose: Initialze dynamic array of bytes.
 *******************************************************************************/
void dasInit(t_array_size *ptArray) {
  ptArray->sCount    = 0;
  ptArray->sCapacity = C_DYNAMIC_ARRAYS_INITIAL_CAPACITY;
  ptArray->pSize     = (size_t*) malloc(sizeof(size_t) * ptArray->sCapacity);
}

/*******************************************************************************
 * Name:  dasAdd
 * Purpose: Adds a value to a dynamic array.
 *******************************************************************************/
void dasAdd(t_array_size *ptArray, size_t bValue) {
  // Check and double cap, if necessary.
  if (ptArray->sCount + 1 > ptArray->sCapacity) {
    ptArray->sCapacity *= 2;
    ptArray->pSize      = (size_t*) realloc(ptArray->pSize, sizeof(size_t) * ptArray->sCapacity);
  }

  // Set value in next slot and increment counter.
  ptArray->pSize[ptArray->sCount++] = bValue;
}

/*******************************************************************************
 * Name:  dasClear
 * Purpose: Reset dynamic array.
 *******************************************************************************/
void dasClear(t_array_size *ptArray) {
  dasFree(ptArray);
  dasInit(ptArray);
}

/*******************************************************************************
 * Name:  dasFree
 * Purpose: Free memory of dynamic array.
 *******************************************************************************/
void dasFree(t_array_size *ptArray) {
  if (ptArray->pSize != NULL)
    free(ptArray->pSize);
}


#endif // C_DYNAMIC_ARRAYS_H


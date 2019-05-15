/*******************************************************************************
 ** Name: c_string.h
 ** Purpose:  Provides a self contained kind of string.
 ** Author: (JE) Jens Elstner
 ** Version: v0.10.3
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 03.11.2017  JE    Created version 0.1.2
 ** 06.11.2017  JE    Changed 'cat' and 'mid' interfaces from 'void' to 'cstr'.
 ** 01.02.2018  JE    Changed all interfaces back to 'void' and deleted
 **                   csSetChar() and csGetChar().
 ** 07.02.2018  JE    Added csInStr().
 ** 08.02.2018  JE    Added cstr2ll(), ll2cstr(), ld2cstr() and cstr2ld().
 ** 15.02.2018  JE    Added csSplit().
 ** 15.02.2018  JE    Changed interface of csInStr() and csCat().
 ** 15.02.2018  JE    Added a few csClear() to removed memory leaks.
 ** 22.02.2018  JE    Added csFree() for freeing memory.
 ** 22.02.2018  JE    Changed csClear() to reset string to "".
 ** 29.04.2018  JE    Added csInput() for a convieneient string input 'box'.
 ** 29.05.2018  JE    Added csTrim(), strips leading and trailing whitespaces.
 ** 29.05.2018  JE    Added cstr_check_if_whitespace() as helper for csTrim().
 ** 28.08.2018  JE    Added csHhex2ll() and ll2csHhex().
 ** 11.09.2018  JE    Added csSetf() to mimik a secure sprinf().
 ** 21.10.2018  JE    Now cstr do make no unecessary reallocations.
 ** 31.01.2019  JE    Added 'csTmp' in 'csSet()', because 'pcString' could be
 **                   a copy of 'pcsString.cStr', and therefore been cleared
 **                   prior usage!
 ** 07.03.2019  JE    Now structs are all named.
 ** 23.04.2019  JE    Minor corrections and optimisations.
 ** 14.05.2019  JE    Changed interface of csTrim().
 ** 14.05.2019  JE    Fixed two offset-by-one bugs in csTrim().
 ** 14.05.2019  JE    Added 'csTmp' in 'csTrim()', because 'pcString' could be
 **                   a copy of 'pcsOut.cStr', and therefore been cleared
 **                   prior usage!
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_STRING_H
#define C_STRING_H


//******************************************************************************
//* includes

#include <stdlib.h>
#include <stdarg.h>


//******************************************************************************
//* defines and macros

#define C_STRING_INITIAL_CAPACITY 256

// To give the cstr var a clean initialisation use
// cstr str = csNew("");


//******************************************************************************
//* type definition

// Central struct, which defines a cstr 'object'.
typedef struct s_cstr {
  int   len;      // length of cstr
  int   size;     // size of array
  int   capacity; // total available slots
  char* cStr;     // array of chars we're storing
} cstr;


//******************************************************************************
//* function forward declarations
//* Makes for a better function's arrangement.

// Internal functions.
void cstr_init(cstr* pcString);
void cstr_check(cstr* pcString);
void cstr_double_capacity_if_full(cstr* pcString, int iSize);
int  cstr_len(const char* pcString);
int  cstr_check_if_whitespace(const char cChar, int bWithNewLines);

// External functions.

// Init & destroy.
cstr csNew(const char* pcString);
void csClear(cstr* pcsString);
void csFree(cstr* pcsString);

// String manipulation functions.
void        csSet(cstr* pcsString, const char* pcString);
void        csSetf(cstr* pcsString, const char* pcFormat, ...);
void        csCat(cstr* pcsDest, const char* pcSource, const char* pcAdd);
int         csInStr(const char *pcString, const char* pcFind);
void        csMid(cstr* pcsDest, const char *pcSource, int iOffset, int iLength);
int         csSplit(cstr* pcsLeft, cstr* pcsRight, const char *pcString, const char *pcSplitAt);
void        csTrim(cstr* pcsOut, const char* pcString, int bWithNewLines);
int         csInput(const char* pcMsg, cstr* pcsDest);
cstr        ll2cstr(long long llValue);
long long   cstr2ll(cstr csValue);
cstr        ld2cstr(long double ldValue);
long double cstr2ld(cstr csValue);
cstr        ll2csHex(long long llValue);
long long   csHex2ll(cstr csValue);


//******************************************************************************
//* private functions

/*******************************************************************************
 * Name: cstr_init
 *******************************************************************************/
void cstr_init(cstr* pcString) {
  if (pcString->cStr != NULL) free(pcString->cStr);
  pcString->len      = 0;
  pcString->size     = 1;
  pcString->capacity = C_STRING_INITIAL_CAPACITY;
  pcString->cStr     = malloc(sizeof(char) * pcString->capacity);
  pcString->cStr[0]  = '\0';
}

/*******************************************************************************
 * Name: cstr_check
 *******************************************************************************/
void cstr_check(cstr* pcString) {
  if (pcString->cStr == NULL) {
    cstr_init(pcString);
    return;
  }
  if (pcString->size      > pcString->capacity        ||
      pcString->capacity  < C_STRING_INITIAL_CAPACITY ||
      pcString->size     != pcString->len + 1         ||
      pcString->len      != cstr_len(pcString->cStr)) {
    pcString->cStr = NULL;
    cstr_init(pcString);
  }
}

/*******************************************************************************
 * Name: cstr_double_capacity_if_full
 *******************************************************************************/
void cstr_double_capacity_if_full(cstr* pcString, int iSize) {
  // Avoid unnecessary reallocations.
  if (pcString->size + iSize <= pcString->capacity) return;

  // Increase capacity until new size fits.
  while (pcString->size + iSize > pcString->capacity) pcString->capacity *= 2;

  // Reallocate new memory.
  pcString->cStr = realloc(pcString->cStr, sizeof(char) * pcString->capacity);
}

/*******************************************************************************
 * Name: cstr_len
 *******************************************************************************/
int cstr_len(const char* pcString) {
  int i = 0;
  while (pcString[i] != '\0') ++i;
  return i;
}

/*******************************************************************************
 * Name: cstr_check_if_whitespace
 *******************************************************************************/
int cstr_check_if_whitespace(const char cChar, int bWithNewLines) {
  if (bWithNewLines) {
    if (cChar == ' '  || cChar == '\t' ||
        cChar == '\n' || cChar == '\r') return 1;
  }
  else {
    if (cChar == ' '  || cChar == '\t') return 1;
  }

  return 0;
}


//******************************************************************************
//* public string functions


//******************************************************************************
//* Init & destroy functions.

/*******************************************************************************
 * Name: csNew
 *******************************************************************************/
cstr csNew(const char* pcString) {
  cstr csOut  = {0};
  int  iCSize = cstr_len(pcString) + 1;

  cstr_init(&csOut);
  cstr_double_capacity_if_full(&csOut, iCSize);

  // Copy char array to cstr.
  for (int i = 0; i < iCSize; ++i)
    csOut.cStr[i] = pcString[i];

  // Adjust parameter.
  csOut.len  = iCSize - 1;
  csOut.size = iCSize;

  // Do not csFree(&csOut);!
  return csOut;
}

/*******************************************************************************
 * Name: csClear
 *******************************************************************************/
void csClear(cstr* pcsString) {
  cstr_init(pcsString);
}

/*******************************************************************************
 * Name: csFree
 *******************************************************************************/
void csFree(cstr* pcsString) {
  if (pcsString->cStr != NULL) free(pcsString->cStr);
  pcsString->len      = 0;
  pcsString->size     = 0;
  pcsString->capacity = 0;
  pcsString->cStr     = NULL;
}


//******************************************************************************
//* String manipulation functions.

/*******************************************************************************
 * Name: csSet
 *******************************************************************************/
void csSet(cstr* pcsString, const char* pcString) {
  // Watch out, 'pcString' could be a pointer from 'pcsString.cStr'!
  cstr csTmp = csNew(pcString);
  csFree(pcsString);
  *pcsString = csNew(csTmp.cStr);
  csFree(&csTmp);
}

/*******************************************************************************
 * Name: csSetf
 *******************************************************************************/
void csSetf(cstr* pcsString, const char* pcFormat, ...) {
  va_list args1;    // Needs two dynamic args pointer because after first use
  va_list args2;    // pointer will have unkown behaviour!

  va_start(args1, pcFormat);
  va_start(args2, pcFormat);

  char* pcBuff = malloc(sizeof(char) * vsnprintf(NULL, 0, pcFormat, args1) + 1);
  vsprintf(pcBuff, pcFormat, args2);

  va_end(args1);
  va_end(args2);

  csSet(pcsString, pcBuff);

  free(pcBuff);
}

/*******************************************************************************
 * Name: csCat
 *******************************************************************************/
void csCat(cstr* pcsDest, const char* pcSource, const char* pcAdd) {
  cstr csOut = csNew(pcSource);
  cstr csAdd = csNew(pcAdd);

  // Make room for the second string.
  cstr_double_capacity_if_full(&csOut, csAdd.size);

  // Now append pChars over csChr's '\0' including pChars's '\0'.
  for (int i = 0; i < csAdd.size; ++i)
    csOut.cStr[csOut.len + i] = pcAdd[i];

  csOut.len  = csOut.len  + csAdd.len;
  csOut.size = csOut.size + csAdd.size - 1;

  csSet(pcsDest, csOut.cStr);

  csClear(&csOut);
  csClear(&csAdd);
}

/*******************************************************************************
 * Name: csFind
 *******************************************************************************/
int csInStr(const char* pcString, const char* pcFind) {
  cstr csString = csNew(pcString);
  cstr csFind   = csNew(pcFind);
  int iSearch   = 0;
  int iFind     = 0;
  int iPos      = -1;

  if (csFind.len == 0)
    return -1;

  // Find last occurence of search-string.
  for (iSearch = 0; iSearch < csString.len; ++iSearch) {
    for (iFind = 0; iFind < csFind.len; ++iFind) {
      if (csString.cStr[iSearch + iFind] != csFind.cStr[iFind])
        break;
    }
    if (csFind.cStr[iFind] != '\0')
      continue;
    iPos = iSearch;
  }

  csClear(&csString);
  csClear(&csFind);

  return iPos;
}

/*******************************************************************************
 * Name: csMid
 *******************************************************************************/
void csMid(cstr* pcsDest, const char* pcSource, int iOffset, int iLength) {
  cstr csSource = csNew(pcSource);
  int  i        = 0;

  // Negative offset stands for offset from the right side.
  // Negative length stands for maxlength from given offset (aka string rest).
  // " a  b  c  d  e  f  g  h  \0 "
  //   0  1  2  3  4  5  6  7       offset (real)
  //  -8 -7 -6 -5 -4 -3 -2 -1       offset (virtual)
  //   8  7  6  5  4  3  2  1       maxlen = len - offset (real)
  // len = 8; size = 9

  // Clear out string prior use.
  csSet(pcsDest, "");

  // Set negative offset to corresponding positive.
  if (iOffset < 0)
    iOffset = csSource.len + iOffset;

  // Return empty string object if offset doesn't fit (negativ or positive).
  // Or wanted length is 0.
  if (iOffset > csSource.len || iLength == 0)
    return;

  // Adjust length to max if it exceeds string's length or is negative.
  if (iLength > csSource.len - iOffset || iLength < 0)
    iLength = csSource.len - iOffset;

  cstr_double_capacity_if_full(pcsDest, iLength + 1);

  // Copy length chars from offset.
  for (i = 0; i < iLength; ++i)
    pcsDest->cStr[i] = csSource.cStr[iOffset + i];

  // Mind the '\0'!
  pcsDest->cStr[iLength] = '\0';
  pcsDest->len           = iLength;
  pcsDest->size          = iLength + 1;

  csClear(&csSource);
}

/*******************************************************************************
 * Name:  csSplit
 * Purpose: Splits a cstr string at first occurence of 'pcSplitAt'.
 *******************************************************************************/
int csSplit(cstr* pcsLeft, cstr* pcsRight, const char* pcString, const char* pcSplitAt) {
  int iPos   = csInStr(pcString, pcSplitAt);
  int iWidth = cstr_len(pcSplitAt);

  // Split, if found.
  if (iPos > -1) {
    csMid(pcsLeft,  pcString,             0, iPos);
    csMid(pcsRight, pcString, iPos + iWidth,   -1);
  }

  // Return, where the split occured.
  return iPos;
}

/*******************************************************************************
 * Name:  csTrim
 * Purpose: Strips leading and trailing whitespaces from string.
 *******************************************************************************/
void csTrim(cstr* pcsOut, const char* pcString, int bWithNewLines) {
  // Watch out, 'pcString' could be a pointer from 'pcsOut.cStr'!
  cstr csTmp = csNew(pcString);
  int iOffMin = 0;
  int iOffMax = csTmp.len - 1;
  int iLen    = 0;

  // Get offset of first non whitespace char from left.
  while (cstr_check_if_whitespace(csTmp.cStr[iOffMin], bWithNewLines))
    ++iOffMin;

  // Get offset of first non whitespace char from right.
  while (cstr_check_if_whitespace(csTmp.cStr[iOffMax], bWithNewLines))
    --iOffMax;

  // Length of trimmed string.
  iLen = iOffMax - iOffMin + 1;

  // Initialize pcsOut.
  csSet(pcsOut, "");

  // Check if length plus '0' byte fits into csOut.
  cstr_double_capacity_if_full(pcsOut, iLen + 1);

  // Copy
  for (int i = 0; i < iLen; ++i)
    pcsOut->cStr[i] = csTmp.cStr[iOffMin + i];

  // Complete csOut's information and don't forget the '0' byte!
  pcsOut->cStr[iLen] = 0;
  pcsOut->len        = iLen;
  pcsOut->size       = iLen + 1;

  csFree(&csTmp);
}

/*******************************************************************************
 * Name:  csInput
 * Purpose: Kind of a getline() from stdin to a cstr var.
 *******************************************************************************/
int csInput(const char* pcMsg, cstr* pcsDest) {
  int  iChar     = 0;
  char acChar[2] = {0};

  // Print message and try to get input line.
  printf("%s", pcMsg);

  // Get all chars excluding the nasty '\n'.
  while (1) {
    iChar = getchar();

    // Error condition of getchar().
    if (iChar == EOF) {
      csSet(pcsDest, "");
      return 1;
    }

    // Take care of the '\n'.
    if ((char) iChar == '\n') return 0;

    // Create a minute string of one char.
    acChar[0] = (char) iChar;
    csCat(pcsDest, pcsDest->cStr, acChar);
  }
}

/*******************************************************************************
 * Name:  ll2cstr
 * Purpose: Converts long long to cstr.
 *******************************************************************************/
cstr ll2cstr(long long llValue) {
  cstr csValue     = csNew("");
  char cBuffer[99] = {0};

  sprintf(cBuffer, "%lld", llValue);
  csSet(&csValue, cBuffer);

  return csValue;
}

/*******************************************************************************
 * Name:  cstr2ll
 * Purpose: Converts cstr to long long.
 *******************************************************************************/
long long cstr2ll(cstr csValue) {
  char* pcEnd;
  return strtoll(csValue.cStr, &pcEnd, 10);
}

/*******************************************************************************
 * Name:  ld2cstr
 * Purpose: Converts long double to cstr.
 *******************************************************************************/
cstr ld2cstr(long double ldValue) {
  cstr csValue     = csNew("");
  char cBuffer[99] = {0};

  sprintf(cBuffer, "%Lf", ldValue);
  csSet(&csValue, cBuffer);

  return csValue;
}

/*******************************************************************************
 * Name:  cstr2ld
 * Purpose: Converts cstr to long double.
 *******************************************************************************/
long double cstr2ld(cstr csValue) {
  return strtold(csValue.cStr, NULL);
}

/*******************************************************************************
 * Name:  ll2csHex
 * Purpose: Converts long long to hex cstr.
 *******************************************************************************/
cstr ll2csHex(long long llValue) {
  cstr csValue     = csNew("");
  char cBuffer[99] = {0};

  sprintf(cBuffer, "0x%llx", llValue);
  csSet(&csValue, cBuffer);

  return csValue;
}

/*******************************************************************************
 * Name:  csHex2ll
 * Purpose: Converts hex cstr to long long.
 *******************************************************************************/
long long csHex2ll(cstr csValue) {
  cstr      csPre = csNew("");
  cstr      csHex = csNew(csValue.cStr);
  long long llVal = 0;

  // Delete possible '0x' prior conversion.
  csMid(&csPre, csHex.cStr, 0, 2);
  if (!strcmp(csPre.cStr, "0x")) csMid(&csHex, csHex.cStr, 2, -1);

  llVal = strtoll(csHex.cStr, NULL, 16);

  csFree(&csPre);
  csFree(&csHex);

  return llVal;
}


#endif // C_STRING_H

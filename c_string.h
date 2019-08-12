/*******************************************************************************
 ** Name: c_string.h
 ** Purpose:  Provides a self contained kind of string.
 ** Author: (JE) Jens Elstner
 ** Version: v0.11.3
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
 ** 06.06.2019  JE    Added lenUtf8 in struct, cstr_utf8_conts(),
 **                   cstr_utf8_bytes() and cstr_lenUtf8().
 ** 06.06.2019  JE    Added csIsUtf8(), csAt() and csAtUtf8().
 ** 11.06.2019  JE    Changed all positions and length ints into size_t.
 ** 07.08.2019  JE    Changed all pos and off from size_t to long long in csMid.
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
  size_t len;      // number of characters in cstr
  size_t lenUtf8;  // number of UTF-8 characters in cstr
  size_t size;     // size of array
  size_t capacity; // total available slots
  char*  cStr;     // array of chars we're storing
} cstr;


//******************************************************************************
//* function forward declarations
//* Makes for a better function's arrangement.

// Internal functions.
void   cstr_init(cstr* pcString);
void   cstr_check(cstr* pcString);
void   cstr_double_capacity_if_full(cstr* pcString, size_t tSize);
int    cstr_utf8_cont(const char c);
size_t cstr_utf8_bytes(const char *c);
size_t cstr_lenUtf8(const char* pcString, size_t *pLen);
size_t cstr_len(const char* pcString);
int    cstr_check_if_whitespace(const char cChar, int bWithNewLines);

// External functions.

// Init & destroy.
cstr csNew(const char* pcString);
void csClear(cstr* pcsString);
void csFree(cstr* pcsString);

// String manipulation functions.
void        csSet(cstr* pcsString, const char* pcString);
void        csSetf(cstr* pcsString, const char* pcFormat, ...);
void        csCat(cstr* pcsDest, const char* pcSource, const char* pcAdd);
size_t      csInStr(const char *pcString, const char* pcFind);
void        csMid(cstr* pcsDest, const char *pcSource, long long llOffset, long long llLength);
size_t      csSplit(cstr* pcsLeft, cstr* pcsRight, const char *pcString, const char *pcSplitAt);
void        csTrim(cstr* pcsOut, const char* pcString, int bWithNewLines);
int         csInput(const char* pcMsg, cstr* pcsDest);
int         csIsUtf8(const char* pcString);
int         csAt(char* pcChar, const char* pcString, size_t tPos);
size_t      csAtUtf8(char* pcChar, const char* pcString, size_t tPos);
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
  pcString->lenUtf8  = 0;
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
void cstr_double_capacity_if_full(cstr* pcString, size_t tSize) {
  // Avoid unnecessary reallocations.
  if (pcString->size + tSize <= pcString->capacity) return;

  // Increase capacity until new size fits.
  while (pcString->size + tSize > pcString->capacity) pcString->capacity *= 2;

  // Reallocate new memory.
  pcString->cStr = realloc(pcString->cStr, sizeof(char) * pcString->capacity);
}

/*******************************************************************************
 * Name: cstr_utf8_cont
 *******************************************************************************/
int cstr_utf8_cont(const char c) {
    return (c & 0xc0) == 0x80;
}

/*******************************************************************************
 * Name: cstr_utf8_bytes
 *******************************************************************************/
size_t cstr_utf8_bytes(const char* c) {
  if ((c[0] & 0x80) == 0x00) return 1;

  if ((c[0] & 0xe0) == 0xc0 &&
       cstr_utf8_cont(c[1])) return 2;

  if ((c[0] & 0xf0) == 0xe0 &&
       cstr_utf8_cont(c[1]) &&
       cstr_utf8_cont(c[2])) return 3;

  if ((c[0] & 0xf8) == 0xf0 &&
       cstr_utf8_cont(c[1]) &&
       cstr_utf8_cont(c[2]) &&
       cstr_utf8_cont(c[3])) return 4;

  return 0;
}

/*******************************************************************************
 * Name: cstr_lenUtf8
 *******************************************************************************/
size_t cstr_lenUtf8(const char* pcString, size_t* pLen) {
  size_t lenUtf8 = 0;
        *pLen    = 0;

  // UTF char is counted if it not continues.
  while (pcString[*pLen] != '\0') {
    if (!cstr_utf8_cont(pcString[*pLen])) ++(lenUtf8);
    ++(*pLen);
  }
  return lenUtf8;
}

/*******************************************************************************
 * Name: cstr_len
 *******************************************************************************/
size_t cstr_len(const char* pcString) {
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
  else
    if (cChar == ' '  || cChar == '\t') return 1;

  return 0;
}


//******************************************************************************
//* public string functions


//******************************************************************************
//* Init & destroy functions.

/*******************************************************************************
 * Name: csNew
 * Purpose: .
 *******************************************************************************/
cstr csNew(const char* pcString) {
  cstr   csOut  = {0};
  size_t tClen  = 0;
  size_t tUlen  = cstr_lenUtf8(pcString, &tClen);
  size_t tCsize = 0;

  // Includes '\0'.
  tCsize = tClen + 1;

  cstr_init(&csOut);
  cstr_double_capacity_if_full(&csOut, tCsize);

  // Copy char array to cstr.
  for(size_t i = 0; i < tCsize; ++i)
    csOut.cStr[i] = pcString[i];

  // Adjust parameter.
  csOut.len     = tClen;
  csOut.lenUtf8 = tUlen;
  csOut.size    = tCsize;

  // Do not csFree(&csOut);!
  return csOut;
}

/*******************************************************************************
 * Name: csClear
 * Purpose: .
 *******************************************************************************/
void csClear(cstr* pcsString) {
  cstr_init(pcsString);
}

/*******************************************************************************
 * Name: csFree
 * Purpose: .
 *******************************************************************************/
void csFree(cstr* pcsString) {
  if (pcsString->cStr != NULL) free(pcsString->cStr);
  pcsString->len      = 0;
  pcsString->lenUtf8  = 0;
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
 * Purpose: .
 *******************************************************************************/
void csCat(cstr* pcsDest, const char* pcSource, const char* pcAdd) {
  cstr csOut = csNew(pcSource);
  cstr csAdd = csNew(pcAdd);

  // Make room for the second string.
  cstr_double_capacity_if_full(&csOut, csAdd.size);

  // Now append pChars over csChr's '\0' including pChars's '\0'.
  for(size_t i = 0; i < csAdd.size; ++i)
    csOut.cStr[csOut.len + i] = pcAdd[i];

  csOut.len  = csOut.len  + csAdd.len;
  csOut.size = csOut.size + csAdd.size - 1;

  csSet(pcsDest, csOut.cStr);

  csClear(&csOut);
  csClear(&csAdd);
}

/*******************************************************************************
 * Name: csFind
 * Purpose: .
 *******************************************************************************/
size_t csInStr(const char* pcString, const char* pcFind) {
  cstr   csString = csNew(pcString);
  cstr   csFind   = csNew(pcFind);
  size_t tSearch   = 0;
  size_t tFind     = 0;
  size_t tPos      = -1;

  if (csFind.len == 0)
    return -1;

  // Find last occurence of search-string.
  for (tSearch = 0; tSearch < csString.len; ++tSearch) {
    for (tFind = 0; tFind < csFind.len; ++tFind) {
      if (csString.cStr[tSearch + tFind] != csFind.cStr[tFind])
        break;
    }
    if (csFind.cStr[tFind] != '\0')
      continue;
    tPos = tSearch;
  }

  csClear(&csString);
  csClear(&csFind);

  return tPos;
}

/*******************************************************************************
 * Name: csMid
 * Purpose: .
 *******************************************************************************/
void csMid(cstr* pcsDest, const char* pcSource, long long llOffset, long long llLength) {
  cstr csSource = csNew(pcSource);

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
  if (llOffset < 0)
    llOffset = csSource.len + llOffset;

  // Return empty string object if offset doesn't fit (negativ or positive).
  // Or wanted length is 0.
  if (llOffset > csSource.len || llLength == 0)
    return;

  // Adjust length to max if it exceeds string's length or is negative.
  if (llLength > csSource.len - llOffset || llLength < 0)
    llLength = csSource.len - llOffset;

  cstr_double_capacity_if_full(pcsDest, llLength + 1);

  // Copy length chars from offset.
  for (size_t i = 0; i < llLength; ++i)
    pcsDest->cStr[i] = csSource.cStr[llOffset + i];

  // Mind the '\0'!
  pcsDest->cStr[llLength] = '\0';
  pcsDest->len            = llLength;
  pcsDest->size           = llLength + 1;

  csClear(&csSource);
}

/*******************************************************************************
 * Name:  csSplit
 * Purpose: Splits a cstr string at first occurence of 'pcSplitAt'.
 *******************************************************************************/
size_t csSplit(cstr* pcsLeft, cstr* pcsRight, const char* pcString, const char* pcSplitAt) {
  size_t tPos   = csInStr(pcString, pcSplitAt);
  size_t tWidth = cstr_len(pcSplitAt);

  // Split, if found.
  if (tPos > -1) {
    csMid(pcsLeft,  pcString,             0, tPos);
    csMid(pcsRight, pcString, tPos + tWidth,   -1);
  }

  // Return, where the split occured.
  return tPos;
}

/*******************************************************************************
 * Name:  csTrim
 * Purpose: Strips leading and trailing whitespaces from string.
 *******************************************************************************/
void csTrim(cstr* pcsOut, const char* pcString, int bWithNewLines) {
  // Watch out, 'pcString' could be a pointer from 'pcsOut.cStr'!
  cstr   csTmp = csNew(pcString);
  size_t tOffMin = 0;
  size_t tOffMax = csTmp.len - 1;
  size_t tLen    = 0;

  // Get offset of first non whitespace char from left.
  while (cstr_check_if_whitespace(csTmp.cStr[tOffMin], bWithNewLines))
    ++tOffMin;

  // Get offset of first non whitespace char from right.
  while (cstr_check_if_whitespace(csTmp.cStr[tOffMax], bWithNewLines))
    --tOffMax;

  // Length of trimmed string.
  tLen = tOffMax - tOffMin + 1;

  // Initialize pcsOut.
  csSet(pcsOut, "");

  // Check if length plus '0' byte fits into csOut.
  cstr_double_capacity_if_full(pcsOut, tLen + 1);

  // Copy
  for(size_t i = 0; i < tLen; ++i)
    pcsOut->cStr[i] = csTmp.cStr[tOffMin + i];

  // Complete csOut's information and don't forget the '0' byte!
  pcsOut->cStr[tLen] = 0;
  pcsOut->len        = tLen;
  pcsOut->size       = tLen + 1;

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
 * Name:  csIsUtf8
 * Purpose: Checks if string is ASCII or UTF-8.
 *******************************************************************************/
int csIsUtf8(const char* pcString) {
  size_t len     = 0;
  size_t lenUtf8 = cstr_lenUtf8(pcString, &len);

  if (len != lenUtf8) return 1;
  return 0;
}

/*******************************************************************************
 * Name:  csAt
 * Purpose: Returns byte at given offset, else 0 byte.
 *******************************************************************************/
int csAt(char* pcChar, const char* pcString, size_t tPos) {
  size_t len = cstr_len(pcString);

  if (tPos > len || tPos < 0) {
    pcChar[0] = 0;
    return 0;
  }
  // else
  pcChar[0] = pcString[tPos];
  return 1;
}

/*******************************************************************************
 * Name:  csAtUtf8
 * Purpose: Returns UTF-8 codepoint/size at given position, else empty string.
 *******************************************************************************/
size_t csAtUtf8(char* pcStr, const char* pcString, size_t tPos) {
  size_t pos     = 0;
  size_t posUtf8 = cstr_lenUtf8(pcString, &pos);
  size_t tBytes  = 0;

  // Must be a 5 byte char array for a 4 byte UTF-8 char at max. Clear it.
  pcStr[0] = pcStr[1] = pcStr[2] = pcStr[3] = pcStr[4] = 0;

  // Calc count of UTF-8 chars for boundary check.
  if (tPos > posUtf8 || tPos < 0) {
    pcStr[0] = 0;
    return 0;
  }

  // Rest vars for their actual purpose.
  pos     = 0;
  posUtf8 = 0;

  // Get offset of UTF-8 position.
  while (posUtf8 < tPos) {
    // Stop at any malformed UTF-8 char.
    if ((tBytes = cstr_utf8_bytes(&pcString[pos])) == 0) {
      pcStr[0] = 0;
      return 0;
    }
    pos     += tBytes;
    posUtf8 += 1;
  }

  tBytes = cstr_utf8_bytes(&pcString[pos]);
  for(size_t i = 0; i < tBytes; ++i)
    pcStr[i] = pcString[pos + i];

  return tBytes;
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

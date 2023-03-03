/*******************************************************************************
 ** Name: c_string.h
 ** Purpose:  Provides a self contained kind of string.
 ** Author: (JE) Jens Elstner
 ** Version: v0.21.2
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
 ** 15.02.2018  JE    Added a few csClear() to remove memory leaks.
 ** 22.02.2018  JE    Added csFree() for freeing memory.
 ** 22.02.2018  JE    Changed csClear() to reset string to "".
 ** 29.04.2018  JE    Added csInput() for a convienient string input 'box'.
 ** 29.05.2018  JE    Added csTrim(), strips leading and trailing whitespaces.
 ** 29.05.2018  JE    Added cstr_check_if_whitespace() as helper for csTrim().
 ** 28.08.2018  JE    Added csHhex2ll() and ll2csHhex().
 ** 11.09.2018  JE    Added csSetf() to mimic a secure sprinf().
 ** 21.10.2018  JE    Now cstr do make no unecessary reallocations.
 ** 31.01.2019  JE    Added 'csTmp' in 'csSet()', because 'pcString' could be
 **                   a copy of 'pcsString.cStr', and therefore been cleared
 **                   prior usage!
 ** 07.03.2019  JE    Now structs are all named.
 ** 23.04.2019  JE    Minor corrections and optimisations.
 ** 14.05.2019  JE    Changed interface of csTrim().
 ** 14.05.2019  JE    Fixed two off-by-one bugs in csTrim().
 ** 14.05.2019  JE    Added 'csTmp' in 'csTrim()', because 'pcString' could be
 **                   a copy of 'pcsOut.cStr', and therefore been cleared
 **                   prior usage!
 ** 06.06.2019  JE    Added lenUtf8 in struct, cstr_utf8_conts(),
 **                   cstr_utf8_bytes() and cstr_len_utf8_char().
 ** 06.06.2019  JE    Added csIsUtf8(), csAt() and csAtUtf8().
 ** 11.06.2019  JE    Changed all positions and length ints into size_t.
 ** 07.08.2019  JE    Changed all pos and off from size_t to long long in csMid.
 ** 30.08.2019  JE    Changed all size_t to long long due to unsigned int bugs.
 ** 06.10.2019  JE    Changed rv of csAtUtf8() and cstr_utf8_bytes to int.
 ** 20.03.2020  JE    Added csIconv() wrapping codepage converter library.
 ** 21.03.2020  JE    Added csSanitize().
 ** 04.04.2020  JE    Changed internals of csInStr() to use strstr().
 ** 29.04.2020  JE    Fixed comments. Fixed csSanitize() '\0' bug.
 ** 04.06.2020  JE    Added llPos in csInStr() to set start offset prior search.
 **                   Adjusted csSplit() accordingly.
 ** 04.06.2020  JE    Added csSplitPos() to split at given offset.
 ** 04.06.2020  JE    Simplified csInStr();
 ** 01.07.2020  JE    Added '#include <string.h>' for strcmp().
 ** 02.01.2021  JE    Changed all csClear() to csFree() in csCat() and csMid().
 ** 16.02.2021  JE    Added (char*) to all malloc()s and realloc()s.
 ** 16.02.2021  JE    Added #include <stdio.h>.
 ** 01.04.2021  JE    Added csInStrRev().
 ** 01.04.2021  JE    Added consts for csMid(), csInStr() and csInStrRev().
 ** 02.04.2021  JE    Now use new consts in own functions.
 ** 05.04.2021  JE    Now all internal cstr_*() functions are static.
 ** 05.04.2021  JE    Commented out unused function cstr_check().
 ** 05.04.2021  JE    Added const CS_START for external use with csInStr() and
 **                   csInStrRev().
 ** 06.04.2021  JE    Deleted cstr_check().
 ** 27.05.2021  JE    Adjusted var names in csIconv().
 ** 20.09.2021  JE    Now set UTF-8 length in csMid(), too.
 ** 11.11.2021  JE    Now csSplitPos() returns 1 on success, else 0.
 ** 14.12.2021  JE    Added csReadLine().
 ** 04.01.2022  JE    Adjusted error checking in csIconv().
 ** 04.01.2022  JE    Now converter is closed when iconv() returnes an error.
 ** 13.04.2022  JE    Added error handling in csReadLine().
 ** 19.04.2022  JE    Removed hacky int to char* conversion in csReadLine().
 ** 25.11.2022  JE    Now free char pointer without check for NULL.
 ** 25.11.2022  JE    Simplify checks in cstr_check_if_whitespace().
 ** 25.12.2022  JE    Fixed csInStrRev() logic error where pos will end.
 ** 19.01.2023  JE    Switched to from/to logic consistently in csIconv().
 ** 21.01.2023  JE    Added pfAgain to csIconv() to signal out-buffer too small.
 ** 21.01.2023  JE    Changed logic from pfAgain to iFactorGuess in csIconv(),
 **                   now realloc out-buffer automatically while too small.
 ** 29.01.2023  JE    Added free() to csIconv(), preventing memory leak.
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_STRING_H
#define C_STRING_H


//******************************************************************************
//* includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <iconv.h>
#include <errno.h>


//******************************************************************************
//* defines and macros

#define C_STRING_INITIAL_CAPACITY 256

// To give the cstr var a clean initialisation use
// cstr str = csNew("");

// csMids()
#define CS_MID_REST (-1)

// csInStr(), csInStrRev()
#define CS_START      (0)
#define CS_NOT_FOUND (-1)

// csIvonv()
#define CS_ICONV_NO_GUESS (0)


//******************************************************************************
//* type definition

// Central struct, which defines a cstr 'object'.
typedef struct s_cstr {
  long long len;      // number of characters in cstr
  long long lenUtf8;  // number of UTF-8 characters in cstr
  long long size;     // size of array
  long long capacity; // total available slots
  char*     cStr;     // array of chars we're storing
} cstr;


//******************************************************************************
//* function forward declarations
//* For a better function's arrangement.

// Internal functions.
static void      cstr_init(cstr* pcString);
static void      cstr_double_capacity_if_full(cstr* pcString, long long llSize);
static int       cstr_utf8_cont(const char c);
static int       cstr_utf8_bytes(const char* c);
static long long cstr_len_utf8_char(const char* pcString, long long* pLen);
static long long cstr_len(const char* pcString);
static int       cstr_check_if_whitespace(const char cChar, int bWithNewLines);
static int       cstr_init_iconv_buffer(cstr* pcsFromStr,
                                        char** pacBufFrom, char** ppcBufFrom, size_t sLenFrom,
                                        char** pacBufTo,   char** ppcBufTo,   size_t sLenTo);

// External functions.

// Init & destroy.
cstr csNew(const char* pcString);
void csClear(cstr* pcsString);
void csFree(cstr* pcsString);

// String manipulation functions.
void        csSet(cstr* pcsString, const char* pcString);
void        csSetf(cstr* pcsString, const char* pcFormat, ...);
void        csCat(cstr* pcsDest, const char* pcSource, const char* pcAdd);
long long   csInStr(long long llPos, const char* pcString, const char* pcFind);
long long   csInStrRev(long long llPosMax, const char* pcString, const char* pcFind);
void        csMid(cstr* pcsDest, const char* pcSource, long long llOffset, long long llLength);
long long   csSplit(cstr* pcsLeft, cstr* pcsRight, const char* pcString, const char* pcSplitAt);
int         csSplitPos(long long llPos, cstr* pcsLeft, cstr* pcsRight, const char* pcString, long long llWidth);
void        csTrim(cstr* pcsOut, const char* pcString, int bWithNewLines);
int         csInput(const char* pcMsg, cstr* pcsDest);
int         csReadLine(cstr* pcsLine, FILE* hFile);
void        csSanitize(cstr* pcsLbl);
int         csIconv(cstr* pcsFromStr, cstr* pcsToStr, const char* pcFrom, const char* pcTo, int iFactorGuess);
int         csIsUtf8(const char* pcString);
int         csAt(char* pcChar, const char* pcString, long long llPos);
int         csAtUtf8(char* pcChar, const char* pcString, long long llPos);
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
static void cstr_init(cstr* pcString) {
  free(pcString->cStr);
  pcString->len      = 0;
  pcString->lenUtf8  = 0;
  pcString->size     = 1;
  pcString->capacity = C_STRING_INITIAL_CAPACITY;
  pcString->cStr     = (char*) malloc(sizeof(char) * pcString->capacity);
  pcString->cStr[0]  = '\0';
}

/*******************************************************************************
 * Name: cstr_double_capacity_if_full
 *******************************************************************************/
static void cstr_double_capacity_if_full(cstr* pcString, long long llSize) {
  // Avoid unnecessary reallocations.
  if (pcString->size + llSize <= pcString->capacity)
    return;

  // Increase capacity until new size fits.
  while (pcString->size + llSize > pcString->capacity)
    pcString->capacity *= 2;

  // Reallocate new memory.
  pcString->cStr = (char*) realloc(pcString->cStr, sizeof(char) * pcString->capacity);
}

/*******************************************************************************
 * Name: cstr_utf8_cont
 *******************************************************************************/
static int cstr_utf8_cont(const char c) {
  return (c & 0xc0) == 0x80;
}

/*******************************************************************************
 * Name: cstr_utf8_bytes
 *******************************************************************************/
static int cstr_utf8_bytes(const char* c) {
  if ((c[0] & 0x80) == 0x00)
    return 1;

  if ((c[0] & 0xe0) == 0xc0 &&
       cstr_utf8_cont(c[1]))
    return 2;

  if ((c[0] & 0xf0) == 0xe0 &&
       cstr_utf8_cont(c[1]) &&
       cstr_utf8_cont(c[2]))
    return 3;

  if ((c[0] & 0xf8) == 0xf0 &&
       cstr_utf8_cont(c[1]) &&
       cstr_utf8_cont(c[2]) &&
       cstr_utf8_cont(c[3]))
    return 4;

  return 0;
}

/*******************************************************************************
 * Name: cstr_len_utf8_char
 *******************************************************************************/
static long long cstr_len_utf8_char(const char* pcString, long long* pLen) {
  long long lenUtf8 = 0;
           *pLen    = 0;

  // UTF char is counted if it not continues.
  while (pcString[*pLen] != '\0') {
    if (!cstr_utf8_cont(pcString[*pLen]))
      ++(lenUtf8);
    ++(*pLen);
  }
  return lenUtf8;
}

/*******************************************************************************
 * Name: cstr_len
 *******************************************************************************/
static long long cstr_len(const char* pcString) {
  int i = 0;
  while (pcString[i] != '\0')
    ++i;
  return i;
}

/*******************************************************************************
 * Name: cstr_check_if_whitespace
 *******************************************************************************/
static int cstr_check_if_whitespace(const char cChar, int bWithNewLines) {
  if                   (cChar == ' '  || cChar == '\t')  return 1;
  if (bWithNewLines && (cChar == '\n' || cChar == '\r')) return 1;
  return 0;
}

/*******************************************************************************
 * Name:  cstr_init_iconv_buffer
 *******************************************************************************/
static int cstr_init_iconv_buffer(cstr* pcsFromStr,
                                  char** pacBufFrom, char** ppcBufFrom, size_t sLenFrom,
                                  char** pacBufTo,   char** ppcBufTo,   size_t sLenTo) {
  // (Re-)allocate vars and copy their pointers for iconv().
  *pacBufFrom = (char*) realloc(*pacBufFrom, sLenFrom * sizeof(char));
  if (*pacBufFrom == NULL)
    return 0;
  *ppcBufFrom = *pacBufFrom;
  *pacBufTo   = (char*) realloc(*pacBufTo,   sLenTo   * sizeof(char));
  if (*pacBufTo   == NULL)
    return 0;
  *ppcBufTo   = *pacBufTo;

  // Copy string to one buffer ...
  for (size_t i = 0; i < sLenFrom; ++i)
    (*pacBufFrom)[i] = pcsFromStr->cStr[i];

  // ... and clear the other.
  for (size_t i = 0; i < sLenTo; ++i)
    (*pacBufTo)[i] = 0;

  return 1;
}


//******************************************************************************
//* public string functions


//******************************************************************************
//* Init & destroy functions.

/*******************************************************************************
 * Name: csNew
 * Purpose: Creates a new cstr object with default parameters + adds a string.
 *******************************************************************************/
cstr csNew(const char* pcString) {
  cstr      csOut   = {0};
  long long llClen  = 0;
  long long llUlen  = cstr_len_utf8_char(pcString, &llClen);
  long long llCsize = 0;

  // Include '\0'.
  llCsize = llClen + 1;

  cstr_init(&csOut);
  cstr_double_capacity_if_full(&csOut, llCsize);

  // Copy char array to cstr.
  for(long long i = 0; i < llCsize; ++i)
    csOut.cStr[i] = pcString[i];

  // Adjust parameter.
  csOut.len     = llClen;
  csOut.lenUtf8 = llUlen;
  csOut.size    = llCsize;

  // Do not csFree(&csOut);!
  return csOut;
}

/*******************************************************************************
 * Name: csClear
 * Purpose: Clears old cstr object and initializes it to an empty one.
 *******************************************************************************/
void csClear(cstr* pcsString) {
  cstr_init(pcsString);
}

/*******************************************************************************
 * Name: csFree
 * Purpose: Deletes cstr object and frees memory used.
 *******************************************************************************/
void csFree(cstr* pcsString) {
  free(pcsString->cStr);
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
 * Purpose: Inserts a new string in cstr object, deletes old one.
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
 * Purpose: Sets new string in cstr object like sprintf().
 *******************************************************************************/
void csSetf(cstr* pcsString, const char* pcFormat, ...) {
  va_list args1;    // Needs two dynamic args pointer because after first use
  va_list args2;    // pointer will have unkown behaviour!

  va_start(args1, pcFormat);
  va_start(args2, pcFormat);

  char* pcBuff = (char*) malloc(sizeof(char) * vsnprintf(NULL, 0, pcFormat, args1) + 1);
  vsprintf(pcBuff, pcFormat, args2);

  va_end(args1);
  va_end(args2);

  csSet(pcsString, pcBuff);

  free(pcBuff);
}

/*******************************************************************************
 * Name: csCat
 * Purpose: Concatenates two strings to one cstr object.
 *******************************************************************************/
void csCat(cstr* pcsDest, const char* pcSource, const char* pcAdd) {
  cstr csOut = csNew(pcSource);
  cstr csAdd = csNew(pcAdd);

  // Make room for the second string.
  cstr_double_capacity_if_full(&csOut, csAdd.size);

  // Now append psAdd over csOut's '\0' including psAdd's '\0'.
  for(long long i = 0; i < csAdd.size; ++i)
    csOut.cStr[csOut.len + i] = pcAdd[i];

  csOut.len  = csOut.len  + csAdd.len;
  csOut.size = csOut.size + csAdd.size - 1;

  csSet(pcsDest, csOut.cStr);

  csFree(&csOut);
  csFree(&csAdd);
}

/*******************************************************************************
 * Name: csInStr
 * Purpose: Finds offset of the first occurence of pcFind in pcString.
 *******************************************************************************/
long long csInStr(long long llPos, const char* pcString, const char* pcFind) {
  long long llStrLen  = cstr_len(pcString);
  long long llFindLen = cstr_len(pcFind);
  long long i         = 0;   // Offset in String.
  long long c         = 0;   // Offset in Find.

  // Sanity checks.
  if (llPos < 0 || llPos > llStrLen || llStrLen == 0 || llFindLen == 0)
    return CS_NOT_FOUND;

  for (i = llPos; i < llStrLen; ++i)
    if (pcFind[c++] == pcString[i]) {
      if (c == llFindLen)
        return i - c + 1;
    }
    else
      c = 0;

  return CS_NOT_FOUND;
}

/*******************************************************************************
 * Name: csInStrRev
 * Purpose: Finds offset of the last occurence of pcFind in pcString, not
 *          greater than llPosMax.
 *******************************************************************************/
long long csInStrRev(long long llPosMax, const char* pcString, const char* pcFind) {
  long long llPos  = 0;
  long long llLast = CS_NOT_FOUND;

  while ((llPos = csInStr(llPos, pcString, pcFind)) != CS_NOT_FOUND) {
    if (llPos > llPosMax) break;
    llLast = llPos;
    ++llPos;
  }

  return llLast;
}

/*******************************************************************************
 * Name: csMid
 * Purpose: Mimics BASIC's MID$(). Added negative offsets and rest of string.
 *          Negative offsets counts from right, negative length, gives rest.
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

  // Adjust length to max if it exceeds string's length or is -1.
  if (llLength > csSource.len - llOffset || llLength == CS_MID_REST)
    llLength = csSource.len - llOffset;

  cstr_double_capacity_if_full(pcsDest, llLength + 1);

  // Copy length chars from offset.
  for (long long i = 0; i < llLength; ++i)
    pcsDest->cStr[i] = csSource.cStr[llOffset + i];

  // Set string object's values and last '\0'!
  pcsDest->cStr[llLength] = '\0';
  pcsDest->lenUtf8        = cstr_len_utf8_char(pcsDest->cStr, &pcsDest->len);
  pcsDest->size           = llLength + 1;

  csFree(&csSource);
}

/*******************************************************************************
 * Name:  csSplit
 * Purpose: Splits a cstr string at first occurence of 'pcSplitAt'.
 *******************************************************************************/
long long csSplit(cstr* pcsLeft, cstr* pcsRight, const char* pcString, const char* pcSplitAt) {
  long long llPos   = csInStr(0, pcString, pcSplitAt);
  long long llWidth = cstr_len(pcSplitAt);

  // Split, if found.
  if (llPos != CS_NOT_FOUND) {
    csMid(pcsLeft,  pcString,               0,       llPos);
    csMid(pcsRight, pcString, llPos + llWidth, CS_MID_REST);
  }

  // Return, where the split occured.
  return llPos;
}

/*******************************************************************************
 * Name:  csSplitPos
 * Purpose: Splits a cstr string at given offset and given width.
 *******************************************************************************/
int csSplitPos(long long llPos, cstr* pcsLeft, cstr* pcsRight, const char* pcString, long long llWidth) {
  long long llStringLen = cstr_len(pcString);

  if (llPos >= 0 && llPos <= llStringLen && llWidth >= 0 && llWidth <= llStringLen) {
    csMid(pcsLeft,  pcString,               0,       llPos);
    csMid(pcsRight, pcString, llPos + llWidth, CS_MID_REST);
    return 1;
  }
  return 0;
}

/*******************************************************************************
 * Name:  csTrim
 * Purpose: Strips leading and trailing whitespaces from string.
 *******************************************************************************/
void csTrim(cstr* pcsOut, const char* pcString, int bWithNewLines) {
  // Watch out, 'pcString' could be a pointer from 'pcsOut.cStr'!
  cstr      csTmp    = csNew(pcString);
  long long llOffMin = 0;
  long long llOffMax = csTmp.len - 1;
  long long llLen    = 0;

  // Get offset of first non whitespace char from left.
  while (cstr_check_if_whitespace(csTmp.cStr[llOffMin], bWithNewLines))
    ++llOffMin;

  // Get offset of first non whitespace char from right.
  while (cstr_check_if_whitespace(csTmp.cStr[llOffMax], bWithNewLines))
    --llOffMax;

  // Length of trimmed string.
  llLen = llOffMax - llOffMin + 1;

  // Initialize pcsOut.
  csSet(pcsOut, "");

  // Check if length plus '0' byte fits into csOut.
  cstr_double_capacity_if_full(pcsOut, llLen + 1);

  // Copy
  for(long long i = 0; i < llLen; ++i)
    pcsOut->cStr[i] = csTmp.cStr[llOffMin + i];

  // Complete csOut's information and don't forget the '0' byte!
  pcsOut->cStr[llLen] = 0;
  pcsOut->lenUtf8     = cstr_len_utf8_char(pcsOut->cStr, &pcsOut->len);
  pcsOut->size        = llLen + 1;

  csFree(&csTmp);
}

/*******************************************************************************
 * Name:  csInput
 * Purpose: Kind of a getline() from stdin into a cstr object.
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
      return 0;
    }

    // Take care of the '\n'.
    if ((char) iChar == '\n')
      return 1;

    // Create a minute string of one char.
    acChar[0] = (char) iChar;
    csCat(pcsDest, pcsDest->cStr, acChar);
  }
}

//*******************************************************************************
//* Name:  csReadLine
//* Purpose: Reads a text line from file into a cstr object.
//*******************************************************************************
int csReadLine(cstr* pcsLine, FILE* hFile) {
  int  iChar     = 0;
  char acChar[2] = {0};

  csSet(pcsLine, "");

  while (1) {
    iChar = fgetc(hFile);

    if (ferror(hFile)) {
      clearerr(hFile);
      return 0;
    }
    if (iChar == '\n')
      return 1;
    if (iChar ==  EOF)
      return 1;

    // Create a minute string of one char.
    acChar[0] = (char) iChar;
    csCat(pcsLine, pcsLine->cStr, acChar);
  }

  return 0;
}

//*******************************************************************************
//* Name:  csSanitize
//* Purpose: Deletes all non printable chars lower than 0x20.
//*******************************************************************************
void csSanitize(cstr* pcsLbl) {
  cstr csTmp = csNew(pcsLbl->cStr);
  int  iTmp  = 0;

  // Save only sane chars in new string.
  for (int i = 0; i < pcsLbl->len; ++i)
    if ((unsigned char) pcsLbl->cStr[i] > 0x1f)
      csTmp.cStr[iTmp++] = pcsLbl->cStr[i];

  // Set to '\0' after last char, to end string.
  csTmp.cStr[iTmp] = 0x00;

  csSet(pcsLbl, csTmp.cStr);

  csFree(&csTmp);
}

/*******************************************************************************
 * Name:  csIconv
 * Purpose: Runs lib version of `echo 'str' | iconv -f from -t to`.
 *          iFactorGuess gives a first factor to multiply in-buffer size with.
 *******************************************************************************/
int csIconv(cstr* pcsFromStr, cstr* pcsToStr, const char* pcFrom, const char* pcTo, int iFactorGuess) {
  int     iFactor    = (iFactorGuess == CS_ICONV_NO_GUESS) ? 1 : iFactorGuess;
  size_t  sLenFrom   = pcsFromStr->size;
  size_t  sLenTo     = pcsFromStr->size * iFactor;
  iconv_t tConverter = iconv_open(pcTo, pcFrom);
  int     iRetVal    = 1;

  char* acBufFrom = NULL;
  char* pcBufFrom = NULL;
  char* acBufTo   = NULL;
  char* pcBufTo   = NULL;

  // Check if something is to do.
  if (tConverter == (iconv_t) -1)
    return 0;
  if (sLenFrom   ==            0)
    return 1;

  while (1) {
    // Create dynamically allocated vars and copy their pointers for iconv().
    if (! cstr_init_iconv_buffer(pcsFromStr,&acBufFrom, &pcBufFrom, sLenFrom, &acBufTo, &pcBufTo, sLenTo)) {
      iRetVal = 0;
      goto close_and_exit;
    }

    if (iconv(tConverter, &pcBufFrom, &sLenFrom, &pcBufTo, &sLenTo) == (size_t) -1) {
      // If out-buffer was too small try a bigger one and reset lengths.
      if (errno == E2BIG) {
        ++iFactor;
        sLenFrom = pcsFromStr->size;
        sLenTo   = pcsFromStr->size * iFactor;
        continue;
      }
      // Else a non-recoverable error occurred.
      iRetVal = 0;
      goto close_and_exit;
    }
    else
      // Everything was OK.
      break;
  }

  csSet(pcsToStr, acBufTo);

close_and_exit:
  iconv_close(tConverter);
  free(acBufFrom);
  free(acBufTo);

  return iRetVal;
}

/*******************************************************************************
 * Name:  csIsUtf8
 * Purpose: Checks if string is ASCII or UTF-8.
 *******************************************************************************/
int csIsUtf8(const char* pcString) {
  long long len     = 0;
  long long lenUtf8 = cstr_len_utf8_char(pcString, &len);

  if (len != lenUtf8)
    return 1;
  return 0;
}

/*******************************************************************************
 * Name:  csAt
 * Purpose: Returns byte at given offset and length of found char (0 or 1).
 *******************************************************************************/
int csAt(char* pcChar, const char* pcString, long long llPos) {
  long long len = cstr_len(pcString);

  if (llPos > len || llPos < 0) {
    pcChar[0] = 0;
    return 0;
  }
  // else
  pcChar[0] = pcString[llPos];
  return 1;
}

/*******************************************************************************
 * Name:  csAtUtf8
 * Purpose: Returns UTF-8 codepoint and length of codepoint (0 to 4).
 *******************************************************************************/
int csAtUtf8(char* pcStr, const char* pcString, long long llPos) {
  long long llPosChar = 0;
  long long llPosUtf8 = cstr_len_utf8_char(pcString, &llPosChar);
  int       iBytes    = 0;

  // Must be a 5 byte char array for a 4 byte UTF-8 char at max. Clear it.
  pcStr[0] = pcStr[1] = pcStr[2] = pcStr[3] = pcStr[4] = 0;

  // Calc count of UTF-8 chars for boundary check.
  if (llPos > llPosUtf8 || llPos < 0) {
    pcStr[0] = 0;
    return 0;
  }

  // Reset vars for their actual purpose.
  llPosChar = 0;
  llPosUtf8 = 0;

  // Get offset of UTF-8 position.
  while (llPosUtf8 < llPos) {
    // Stop at any malformed UTF-8 char.
    if ((iBytes = cstr_utf8_bytes(&pcString[llPosChar])) == 0) {
      pcStr[0] = 0;
      return 0;
    }
    llPosChar += iBytes;
    llPosUtf8 += 1;
  }

  iBytes = cstr_utf8_bytes(&pcString[llPosChar]);
  for(long long i = 0; i < iBytes; ++i)
    pcStr[i] = pcString[llPosChar + i];

  return iBytes;
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
  if (!strcmp(csPre.cStr, "0x"))
    csMid(&csHex, csHex.cStr, 2, CS_MID_REST);

  llVal = strtoll(csHex.cStr, NULL, 16);

  csFree(&csPre);
  csFree(&csHex);

  return llVal;
}


#endif // C_STRING_H

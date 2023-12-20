/*******************************************************************************
 ** Name: c_my_regex.h
 ** Purpose:  Provides an easy interface for pcre.h.
 ** Author: (JE) Jens Elstner
 ** Version: v0.11.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 19.09.2018  JE    Created version 0.0.1
 ** 30.11.2018  JE    Changed processing of flag 'x'.
 ** 07.03.2019  JE    Now structs are all named.
 ** 01.07.2019  JE    Now create and free matching array internally.
 ** 18.02.2020  JE    Added daiOffStart and daiOffEnd like Perl's @- and @+.
 ** 21.02.2020  JE    Now rxMatch() use iStartPos and pcSearchStr.
 ** 21.02.2020  JE    Added RX_KEEP_POS for ease of use of rxMatch() in loops.
 ** 21.02.2020  JE    Added stCount to rxMatch() for use of non string char arrays.
 ** 21.02.2020  JE    Added RX_NO_COUNT handling string length or byte count.
 ** 21.02.2020  JE    Renamed rxDelMatcher() to rxFreeMatcher().
 ** 21.02.2020  JE    Fixed: Added daiClear() for start and end arrays.
 ** 24.03.2020  JE    Changed 'StartPos' from 'int' to 'size_t ' in 'rxMatch()'.
 ** 27.03.2020  JE    Changed daiXXX() functions to new dasXXX().
 ** 30.03.2020  JE    Now pMatchData is freed befor every new match.
 ** 27.05.2021  JE    Now use 'c_dynamic_arrays_macros.h'.
 ** 27.05.2021  JE    Renamed csOptions to csFlags in rxInitMatcher().
 ** 01.01.2023  JE    Refactored RX_NO_COUNT to RX_LEN_MAX and sCount to
 **                   sSearchLenMax.
 ** 12.02.2023  JE    Now rxInitMatcher() throws a wrong option error.
 ** 12.02.2023  JE    Now rxMatch() sets pos = 0 if finished without error.
 ** 19.02.2023  JE    Added convienience macros for ovector start and end.
 ** 20.12.2023  JE    Now in 'rxMatch()' and 'rxInitMatcher()' 'pcsErr' and
 **                   'piErr' can be NULL.
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_MY_REGEX_H
#define C_MY_REGEX_H


//******************************************************************************
//* includes

// Add '-lpcre2-8' to the CMAKE_EXE_LINKER_FLAGS!
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <stdio.h>
#include <stdlib.h>

#include "c_string.h"
#include "c_dynamic_arrays_macros.h"


//******************************************************************************
//* defines and macros

#define RX_RV_END  0x00
#define RX_RV_CONT 0x01

#define RX_NO_ERROR  0x00
#define RX_NO_MATCH  0x01
#define RX_NO_VECTOR 0x02
#define RX_ERROR     0x03

#define RX_KEEP_POS (~0L) // Get -1 or largest number.
#define RX_LEN_MAX  (~0L) // Get -1 or largest number.

#define O_START(var) (2 * var)      // Even index.
#define O_END(var)   (2 * var + 1)  // Odd index.


//******************************************************************************
//* type definition

s_array(cstr);
s_array(size_t);

// Control struct for global matching.
typedef struct s_rx_matcher {
  size_t            sPos;
  pcre2_match_data* pMatchData;
  pcre2_code*       pRegex;
  uint32_t          ui32Opts;
  t_array(cstr)     dacsMatch;
  t_array(size_t)   dasStart;
  t_array(size_t)   dasEnd;
} t_rx_matcher;


//******************************************************************************
//* function forward declarations
//* Makes for a better function's arrangement.


//******************************************************************************
//* public functions

int  rxInitMatcher(t_rx_matcher* prxMatcher, const char* pcRegex, const char* pcFlags, cstr* pcsErr);
void rxFreeMatcher(t_rx_matcher* prxMatcher);
int        rxMatch(t_rx_matcher* prxMatcher, size_t sStartPos, const char* pcSearchStr, size_t sSearchLenMax, int* piErr, cstr* pcsErr);


//******************************************************************************
//* public functions

/*******************************************************************************
 * Name: rxInitMatcher
 *******************************************************************************/
int rxInitMatcher(t_rx_matcher* prxMatcher, const char* pcRegex, const char* pcFlags, cstr* pcsErr) {
  cstr       csFlags = csNew(pcFlags);
  cstr       csRegex = csNew(pcRegex);
  int        iErr    = RX_NO_ERROR;
  int        iErrNo  = 0;
  PCRE2_SIZE iErrOff = 0;

  prxMatcher->sPos       = 0;
  prxMatcher->pMatchData = NULL;
  prxMatcher->pRegex     = NULL;
  prxMatcher->ui32Opts   = 0;

  // Init cstr and int arrays, which holds all matches and offsets.
  daInit(cstr, prxMatcher->dacsMatch);
  daInit(size_t, prxMatcher->dasStart);
  daInit(size_t, prxMatcher->dasEnd);

  // Convert option string into options and init everything to work global.
  // Because PCRE2_EXTENDED don't work, I use the implicit form '(?x:...)'.
  for (int i = 0; i < csFlags.len; ++i) {
    if (csFlags.cStr[i] == 'x') {
      csSetf(&csRegex, "(?x:%s)", csRegex.cStr);
      continue;
    }
    if (csFlags.cStr[i] == 'i') {
      prxMatcher->ui32Opts |= PCRE2_CASELESS;
      continue;
    }
    if (csFlags.cStr[i] == 'm') {
      prxMatcher->ui32Opts |= PCRE2_MULTILINE;
      continue;
    }
    if (csFlags.cStr[i] == 's') {
      prxMatcher->ui32Opts |= PCRE2_DOTALL;
      continue;
    }
    if(pcsErr != NULL) csSetf(pcsErr, "Unkown option '%c'", csFlags.cStr[i]);
    iErr = RX_ERROR;
    goto free_and_exit;
  }

  // Compile regex
  prxMatcher->pRegex = pcre2_compile(
    (PCRE2_SPTR) csRegex.cStr,  // the pattern
    PCRE2_ZERO_TERMINATED,      // indicates pattern is zero-terminated
    prxMatcher->ui32Opts,       // options
    &iErrNo,                    // for error number
    &iErrOff,                   // for error offset
    NULL                        // use default compile context
  );

  // A fail will set pcsErr with the error string and return RX_ERROR.
  if (prxMatcher->pRegex == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(iErrNo, buffer, sizeof(buffer));
    csSetf(pcsErr, "Compilation failed at %d: %s", iErrOff, buffer);
    iErr = RX_ERROR;
  }

free_and_exit:
  csFree(&csFlags);
  csFree(&csRegex);

  return iErr;
}

/*******************************************************************************
 * Name: rxFreeMatcher
 *******************************************************************************/
void rxFreeMatcher(t_rx_matcher* prxMatcher) {
  pcre2_match_data_free(prxMatcher->pMatchData);
  pcre2_code_free(prxMatcher->pRegex);
  daFreeEx(prxMatcher->dacsMatch, cStr);
  daFree(prxMatcher->dasStart);
  daFree(prxMatcher->dasEnd);
}

/*******************************************************************************
 * Name: rxMatch
 *******************************************************************************/
int rxMatch(t_rx_matcher* prxMatcher, size_t sStartPos, const char* pcSearchStr, size_t sSearchLenMax, int* piErr, cstr* pcsErr) {
  PCRE2_SPTR  pcStr       = (PCRE2_SPTR) pcSearchStr;
  size_t      sStrLength  = 0;
  PCRE2_SPTR  pcSubStr    = NULL;
  cstr        csSubStr    = csNew("");
  size_t      sSubStrLen  = 0;
  int         iMatchCount = 0;
  int         iRv         = RX_RV_CONT;
  PCRE2_SIZE* psOvector   = NULL;

  if (sSearchLenMax == RX_LEN_MAX)
    sStrLength = strlen(pcSearchStr);
  else
    sStrLength = sSearchLenMax;

  // Init error value to none.
  if (piErr != NULL) *piErr = RX_NO_ERROR;

  //****************************************************************************
  //* The actual matching function block.
  //****************************************************************************

  // Create enough space for all parentheses and match strings.
  if (prxMatcher->pMatchData != NULL)
    pcre2_match_data_free(prxMatcher->pMatchData);
  prxMatcher->pMatchData = pcre2_match_data_create_from_pattern(prxMatcher->pRegex, NULL);

  // Set pos to start from if wanted.
  if (sStartPos != RX_KEEP_POS)
    prxMatcher->sPos = sStartPos;

  iMatchCount = pcre2_match(
    prxMatcher->pRegex,       // the compiled pattern
    pcStr,                    // the subject string
    sStrLength,               // the length of the subject
    prxMatcher->sPos,         // start at offset iPos
    prxMatcher->ui32Opts,     // options
    prxMatcher->pMatchData,   // block for storing the result
    NULL                      // use default match context
  );

  //****************************************************************************
  //* Error handling.
  //****************************************************************************

  if (iMatchCount == PCRE2_ERROR_NOMATCH) {   // Just no match, no error!
    if (pcsErr != NULL) csSet(pcsErr, "No match");
    if (piErr  != NULL) *piErr = RX_NO_MATCH;
    iRv    = RX_RV_END;
    prxMatcher->sPos = 0;
    goto free_and_exit;
  }
  if (iMatchCount < 0) {                      // Matching failed.
    if (pcsErr != NULL) csSetf(pcsErr, "Matching error %d", iMatchCount);
    if (piErr  != NULL) *piErr = RX_ERROR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }
  if (iMatchCount == 0) {                     // Creating output vector failed.
    if (pcsErr != NULL) csSet(pcsErr, "'ovector' was not big enough for all captured substrings");
    if (piErr  != NULL) *piErr = RX_NO_VECTOR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }

  //****************************************************************************
  //* Match succeded. Get a pointer to the output vector, where string offsets
  //* are stored. Cram all matches into a dynamic cstr array.
  //****************************************************************************

  psOvector = pcre2_get_ovector_pointer(prxMatcher->pMatchData);
  daClearEx(cstr, prxMatcher->dacsMatch, cStr);
  daClear(size_t, prxMatcher->dasStart);
  daClear(size_t, prxMatcher->dasEnd);

  for (int i = 0; i < iMatchCount; ++i) {
    // Get offset and length of a match ...
    pcSubStr   = pcStr               + psOvector[O_START(i)];
    sSubStrLen = psOvector[O_END(i)] - psOvector[O_START(i)];

    // ... save start and end offsets in dynamic arrays, too ...
    daAdd(size_t, prxMatcher->dasStart, psOvector[O_START(i)]);
    daAdd(size_t, prxMatcher->dasEnd,   psOvector[O_END(i)]);

    // ... and add it to the dynamic array via temporary cstr.
    csSetf(&csSubStr, "%.*s", sSubStrLen, pcSubStr);
    daAdd(cstr, prxMatcher->dacsMatch, csNew(csSubStr.cStr));
  }

  // Store end of complete match as pos().
  prxMatcher->sPos = psOvector[O_END(0)];

  // If the match was an empty string, hop along one pos.
  if (psOvector[O_END(0)] == psOvector[O_START(0)]) {
    if (pcsErr != NULL) csSet(pcsErr, "Empty string");
    if (piErr  != NULL) *piErr = RX_NO_ERROR;
    iRv    = RX_RV_CONT;
    ++prxMatcher->sPos;
    goto free_and_exit;
  }

  if (psOvector[0] >= sStrLength) {
    if (pcsErr != NULL) csSet(pcsErr, "End of subject");
    if (piErr  != NULL) *piErr = RX_NO_ERROR;
    iRv    = RX_RV_END;
    prxMatcher->sPos = 0;
    goto free_and_exit;
  }

free_and_exit:
  csFree(&csSubStr);

  return iRv;
}


#endif // C_MY_REGEX_H

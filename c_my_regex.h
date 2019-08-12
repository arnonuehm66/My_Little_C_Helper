/*******************************************************************************
 ** Name: c_my_regex.h
 ** Purpose:  Provides an easy interface for pcre.h.
 ** Author: (JE) Jens Elstner
 ** Version: v0.2.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 19.09.2018  JE    Created version 0.0.1
 ** 30.11.2018  JE    Changed processing of flag 'x'.
 ** 07.03.2019  JE    Now structs are all named.
 ** 01.07.2019  JE    Now create and free matching array internally.
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
#include "c_dynamic_arrays.h"


//******************************************************************************
//* defines and macros

#define RX_RV_END  0x00
#define RX_RV_CONT 0x01

#define RX_NO_ERROR  0x00
#define RX_NO_MATCH  0x01
#define RX_NO_VECTOR 0x02
#define RX_ERROR     0x03


//******************************************************************************
//* type definition

// Control struct for global matching.
typedef struct s_rx_matcher {
  int               iPos;
  pcre2_match_data* pMatchData;
  pcre2_code*       pRegex;
  PCRE2_SPTR        pcSubject;
  size_t            sSubjectLength;
  uint32_t          ui32Opts;
  t_array_cstr      dacsMatches;
} t_rx_matcher;


//******************************************************************************
//* function forward declarations
//* Makes for a better function's arrangement.


//******************************************************************************
//* public functions

int  rxInitMatcher(t_rx_matcher* prxMatcher, int iStartPos, const char* pcSearchStr, const char* pcRegex, const char* pcFlags, cstr *pcsErr);
void  rxDelMatcher(t_rx_matcher* prxMatcher);
int        rxMatch(t_rx_matcher* prxMatcher, int *piErr, cstr *pcsErr);


//******************************************************************************
//* public functions

/*******************************************************************************
 * Name: rxInitMatcher
 *******************************************************************************/
int rxInitMatcher(t_rx_matcher* prxMatcher, int iStartPos, const char* pcSearchStr, const char* pcRegex, const char* pcFlags, cstr *pcsErr) {
  cstr       csOPtions = csNew(pcFlags);
  cstr       csRegex   = csNew(pcRegex);
  int        iErr      = RX_NO_ERROR;
  int        iErrNo    = 0;
  PCRE2_SIZE iErrOff   = 0;

  prxMatcher->iPos           = iStartPos;
  prxMatcher->pMatchData     = NULL;
  prxMatcher->pRegex         = NULL;
  prxMatcher->pcSubject      = NULL;
  prxMatcher->sSubjectLength = 0;
  prxMatcher->ui32Opts       = 0;

  // Init cstr array, which holds all matches.
  dacsInit(&prxMatcher->dacsMatches);

  // Convert option string into options and init everything to work global.
  // Because PCRE2_EXTENDED don't work, I use the implicit form '(?x:...)'.
  for (int i = 0; i < csOPtions.len; ++i) {
    if (csOPtions.cStr[i] == 'x') csSetf(&csRegex, "(?x:%s)", csRegex.cStr);
    if (csOPtions.cStr[i] == 'i') prxMatcher->ui32Opts |= PCRE2_CASELESS;
    if (csOPtions.cStr[i] == 'm') prxMatcher->ui32Opts |= PCRE2_MULTILINE;
    if (csOPtions.cStr[i] == 's') prxMatcher->ui32Opts |= PCRE2_DOTALL;
  }

  // Cast to 'char*' works, because of 8 bit code-unit width.
  prxMatcher->pcSubject      = (PCRE2_SPTR) pcSearchStr;
  prxMatcher->sSubjectLength = strlen(pcSearchStr);

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

  csFree(&csOPtions);
  csFree(&csRegex);

  return iErr;
}

/*******************************************************************************
 * Name: rxDelMatcher
 *******************************************************************************/
void rxDelMatcher(t_rx_matcher* prxMatcher) {
  pcre2_match_data_free(prxMatcher->pMatchData);
  pcre2_code_free(prxMatcher->pRegex);
  dacsFree(&prxMatcher->dacsMatches);
}

/*******************************************************************************
 * Name: rxMatch
 *******************************************************************************/
int rxMatch(t_rx_matcher* prxMatcher, int* piErr, cstr* pcsErr) {
  PCRE2_SPTR  pcSubStr    = NULL;
  cstr        csSubStr    = csNew("");
  size_t      sSubStrLen  = 0;
  int         iMatchCount = 0;
  int         iStart      = 0;
  int         iEnd        = 0;
  int         iRv         = RX_RV_CONT;
  PCRE2_SIZE* piOvector   = NULL;

  // Init error value to none.
  *piErr = RX_NO_ERROR;

  //****************************************************************************
  //* The actual matching function block.
  //****************************************************************************

  // Create enough space for all parentheses and match strings.
  prxMatcher->pMatchData = pcre2_match_data_create_from_pattern(prxMatcher->pRegex, NULL);

  iMatchCount = pcre2_match(
    prxMatcher->pRegex,           // the compiled pattern
    prxMatcher->pcSubject,        // the subject string
    prxMatcher->sSubjectLength,   // the length of the subject
    prxMatcher->iPos,             // start at offset iPos
    prxMatcher->ui32Opts,         // options
    prxMatcher->pMatchData,       // block for storing the result
    NULL                          // use default match context
  );

  //****************************************************************************
  //* Error handling.
  //****************************************************************************

  if (iMatchCount == PCRE2_ERROR_NOMATCH) {   // Just no match, no error!
    csSet(pcsErr, "No match");
    *piErr = RX_NO_MATCH;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }
  if (iMatchCount < 0) {                      // Matching failed.
    csSetf(pcsErr, "Matching error %d", iMatchCount);
    *piErr = RX_ERROR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }
  if (iMatchCount == 0) {                     // Creating output vector failed.
    csSet(pcsErr, "'ovector' was not big enough for all captured substrings");
    *piErr = RX_NO_VECTOR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }

  //****************************************************************************
  //* Match succeded. Get a pointer to the output vector, where string offsets
  //* are stored. Cram all matches into a dynamic cstr array.
  //****************************************************************************

  piOvector = pcre2_get_ovector_pointer(prxMatcher->pMatchData);
  dacsClear(&prxMatcher->dacsMatches);

  for (int i = 0; i < iMatchCount; ++i) {
    iStart = 2 * i;       // Next even index.
    iEnd   = 2 * i + 1;   // Next odd index.

    // Get offset and length of a match ...
    pcSubStr   = prxMatcher->pcSubject + piOvector[iStart];
    sSubStrLen = piOvector[iEnd] - piOvector[iStart];

    // ... and add it to the dynamic array via temporary cstr.
    csSetf(&csSubStr, "%.*s", sSubStrLen, pcSubStr);
    dacsAdd(&prxMatcher->dacsMatches, csSubStr.cStr);
  }

  // Store end of complete match as pos().
  prxMatcher->iPos = piOvector[1];

  // If the match was an empty string, hop along one pos.
  if (piOvector[1] == piOvector[0]) {
    csSet(pcsErr, "Empty string");
    *piErr = RX_NO_ERROR;
    iRv    = RX_RV_CONT;
    ++prxMatcher->iPos;
    goto free_and_exit;
  }

  if (piOvector[0] >= prxMatcher->sSubjectLength) {
    csSet(pcsErr, "End of subject");
    *piErr = RX_NO_ERROR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }

free_and_exit:
  csFree(&csSubStr);

  return iRv;
}


#endif // C_MY_REGEX_H

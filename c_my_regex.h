/*******************************************************************************
 ** Name: c_my_regex.h
 ** Purpose:  Provides an easy interface for pcre.h.
 ** Author: (JE) Jens Elstner
 ** Version: v0.1.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 19.09.2018  JE    Created version 0.0.1
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
typedef struct {
  int               iPos;
  pcre2_match_data* pMatchData;
  pcre2_code*       pRegex;
  PCRE2_SPTR        pcSubject;
  size_t            iSubjectLength;
  uint32_t          ui32Opts;
} t_rx_matcher;


//******************************************************************************
//* function forward declarations
//* Makes for a better function's arrangement.


//******************************************************************************
//* public functions

int  rxInitMatcher(t_rx_matcher* prxMatcher, int iStartPos, const char* pcSearchStr, const char* pcRegex, const char* pcFlags, cstr *pcsErr);
void rxDelMatcher(t_rx_matcher* prxMatcher);
int  rxMatch(t_array_cstr* pdacsSubMatches, t_rx_matcher* prxMatcher, int *piErr, cstr *pcsErr);


//******************************************************************************
//* public functions

/*******************************************************************************
 * Name: rxInitMatcher
 *******************************************************************************/
int rxInitMatcher(t_rx_matcher* prxMatcher, int iStartPos, const char* pcSearchStr, const char* pcRegex, const char* pcFlags, cstr *pcsErr) {
  cstr       csOPtions = csNew(pcFlags);
  int        iErr      = RX_NO_ERROR;
  int        iErrNo    = 0;
  PCRE2_SIZE iErrOff   = 0;

  prxMatcher->iPos           = iStartPos;
  prxMatcher->pMatchData     = NULL;
  prxMatcher->pRegex         = NULL;
  prxMatcher->pcSubject      = NULL;
  prxMatcher->iSubjectLength = 0;
  prxMatcher->ui32Opts       = 0;

  // Convert option string into options and init everything to work global.
  for (int i = 0; i < csOPtions.len; ++i) {
    if (csOPtions.cStr[i] == 'x') prxMatcher->ui32Opts |= PCRE2_EXTENDED;
    if (csOPtions.cStr[i] == 'i') prxMatcher->ui32Opts |= PCRE2_CASELESS;
    if (csOPtions.cStr[i] == 'm') prxMatcher->ui32Opts |= PCRE2_MULTILINE;
    if (csOPtions.cStr[i] == 's') prxMatcher->ui32Opts |= PCRE2_DOTALL;
  }

  // Cast to 'char*' works, because of 8 bit code-unit width.
  prxMatcher->pcSubject      = (PCRE2_SPTR) pcSearchStr;
  prxMatcher->iSubjectLength = strlen(pcSearchStr);

  // Compile regex
  prxMatcher->pRegex = pcre2_compile(
    (PCRE2_SPTR) pcRegex,   // the pattern
    PCRE2_ZERO_TERMINATED,  // indicates pattern is zero-terminated
    prxMatcher->ui32Opts,   // options
    &iErrNo,                // for error number
    &iErrOff,               // for error offset
    NULL                    // use default compile context
  );

  // A fail will set pcsErr with the error string and return RX_ERROR.
  if (prxMatcher->pRegex == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(iErrNo, buffer, sizeof(buffer));
    csSetf(pcsErr, "Compilation failed at %d: %s\n", iErrOff, buffer);
    iErr = RX_ERROR;
  }

  csFree(&csOPtions);

  return iErr;
}

/*******************************************************************************
 * Name: rxDelMatcher
 *******************************************************************************/
void rxDelMatcher(t_rx_matcher* prxMatcher) {
  pcre2_match_data_free(prxMatcher->pMatchData);
  pcre2_code_free(prxMatcher->pRegex);
}

/*******************************************************************************
 * Name: rxMatch
 *******************************************************************************/
int rxMatch(t_array_cstr* pdacsSubMatches, t_rx_matcher* prxMatcher, int *piErr, cstr *pcsErr) {
  PCRE2_SPTR  pcSubStr    = NULL;
  cstr        csSubStr    = csNew("");
  size_t      iSubStrLen  = 0;
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
                  prxMatcher->iSubjectLength,   // the length of the subject
                  prxMatcher->iPos,             // start at offset iPos
                  prxMatcher->ui32Opts,         // options
                  prxMatcher->pMatchData,       // block for storing the result
                  NULL                          // use default match context
                );

  //****************************************************************************
  //* Error handling.
  //****************************************************************************

  if (iMatchCount == PCRE2_ERROR_NOMATCH) {   // Just no match, no error!
    csSet(pcsErr, "No match\n");
    *piErr = RX_NO_MATCH;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }
  if (iMatchCount < 0) {                      // Matching failed.
    csSetf(pcsErr, "Matching error %d\n", iMatchCount);
    *piErr = RX_ERROR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }
  if (iMatchCount == 0) {                     // Creating output vector failed.
    csSet(pcsErr, "'ovector' was not big enough for all captured substrings\n");
    *piErr = RX_NO_VECTOR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }

  //****************************************************************************
  //* Match succeded. Get a pointer to the output vector, where string offsets
  //* are stored. Cram all matches into a dynamic string array.
  //****************************************************************************

  piOvector = pcre2_get_ovector_pointer(prxMatcher->pMatchData);
  dacsClear(pdacsSubMatches);
  for (int i = 0; i < iMatchCount; ++i) {
    iStart = 2 * i;       // Next even index.
    iEnd   = 2 * i + 1;   // Next odd index.

    // Get offset and length of a match ...
    pcSubStr   = prxMatcher->pcSubject + piOvector[iStart];
    iSubStrLen = piOvector[iEnd] - piOvector[iStart];

    // ... and add it to the dynamic array via temporary cstr.
    csSetf(&csSubStr, "%.*s", (int) iSubStrLen, (char*) pcSubStr);
    dacsAdd(pdacsSubMatches, csSubStr.cStr);
  }

  // Store end of complete match as pos().
  prxMatcher->iPos = piOvector[1];

  // If the match was an empty string
  if (piOvector[1] == piOvector[0]) {
    csSet(pcsErr, "Empty string\n");
    *piErr = RX_NO_ERROR;
    iRv    = RX_RV_CONT;
    ++prxMatcher->iPos;
    goto free_and_exit;
  }

  if (piOvector[0] >= prxMatcher->iSubjectLength) {
    csSet(pcsErr, "End of subject\n");
    *piErr = RX_NO_ERROR;
    iRv    = RX_RV_END;
    goto free_and_exit;
  }

free_and_exit:
  csFree(&csSubStr);

  return iRv;
}


#endif // C_MY_REGEX_H

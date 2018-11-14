/*******************************************************************************
 ** Name: skeleton_main.c
 ** Purpose: What the programm should do.
 ** Author: (JE) Jens Elstner <jens.elstner@bka.bund.de>
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 01.01.2018  JE    Created program.
 ** 03.03.2018  JE    Now uses c_string_v0.5.3.h
 ** 27.08.2018  JE    Added getHexIntParm().
 ** 29.08.2018  JE    Added 'ox=n' kind of option.
 ** 11.09.2018  JE    Now uses c_string_v0.9.1.h with csSetf().
 ** 13.09.2018  JE    Added time funktion test options and extended debug().
 ** 17.09.2018  JE    Changed shift() to avoid a cstr memory leak.
 ** 02.10.2018  JE    Adde lower case letters to getHexIntParm().
 ** 10.10.2018  JE    Changed ticks2datetime() to avoid a cstr memory leak.
 ** 13.11.2018  JE    Added PCRE2 wrapper c_my_regex.h v0.1.1.
 ** 13.11.2018  JE    Added '--rx' to test regex with string provided by '-X'.
 *******************************************************************************
 ** Skript tested with:
 ** TestDvice 123a.
 *******************************************************************************/


//******************************************************************************
//* includes & namespaces

#define _XOPEN_SOURCE 700  // To get POSIX 2008 (SUS) strptime() and mktime()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>

//#include "../../libs/c_string.h"
//#include "../../libs/c_my_regex.h"
//#include "../../libs/c_dynamic_arrays.h"
#include "c_string.h"
#include "c_my_regex.h"
#include "c_dynamic_arrays.h"


//******************************************************************************
//* defines & macros

#define ME_NAME    "skeleton_main.c"
#define ME_VERSION "0.0.15"

#define ERR_NOERR 0x00
#define ERR_ARGS  0x01
#define ERR_FILE  0x02
#define ERR_ELSE  0xff

#define sERR_ARGS "Argument error"
#define sERR_FILE "File error"
#define sERR_ELSE "Unknown error"

// isNumber()
#define NUM_NONE  0x00
#define NUM_INT   0x01
#define NUM_FLOAT 0x02

// getOptions(): Defines empty values.
#define NO_TICK -1


//******************************************************************************
//* typedefs

// For convienience.
typedef unsigned int  uint;
typedef unsigned char uchar;
typedef long double   ldbl;
typedef long long     ll;
//typedef int           bool;
//const   int           false = 0;
//const   int           true  = 1;

// Arguments and options.
typedef struct {
  long lByteOff;
  int  iTestMode;
  int  iPrtOff;
  int  iOptX;     // Integer verion.
  cstr csOptX;    // String version.
  cstr csRx;
  int  iTicksMin;
  int  iTicksMax;
} t_options;


//******************************************************************************
//* Global variables

// Arguments
t_options    g_tOpts; // CLI options and arguments.
t_array_cstr g_tArgs; // Free arguments.


//******************************************************************************
//* Functions

/*******************************************************************************
 * Name:  version
 * Purpose: Print version and exit program.
 *******************************************************************************/
void version(void) {
  cstr csOut = csNew(ME_NAME);

  csCat(&csOut, csOut.cStr, " v" ME_VERSION);
  printf("%s\n", csOut.cStr);

  csFree(&csOut);

  exit(ERR_NOERR);
}

/*******************************************************************************
 * Name:  usage
 * Purpose: Print help text and exit program.
 *******************************************************************************/
void usage(int iErr, char* pcMsg) {
  cstr csMsg = csNew(pcMsg);

  // Print at least one newline with message.
  if (csMsg.len != 0)
    csCat(&csMsg, csMsg.cStr, "\n\n");

  csCat(&csMsg, csMsg.cStr,
//|************************ 80 chars width ****************************************|
  "usage: " ME_NAME " [-t] [-o] [-x n] [-X <str> [--rx <regex>]] [-e hex] [ox=hex] [-y yyyy [-Y yyyy]] file1 [file2 ...]\n"
  "       " ME_NAME " [-h|--help|-v|--version]\n"
  " What the programm should do.\n"
  " '-e' and 'ox=' can be entered as hexadecimal with '0x' prefix or as decimal\n"
  " with postfix K, M, G (meaning Kilo- Mega- and Giga-bytes based on 1024).\n"
  "  -t:            don't execute printed commands (default execute)\n"
  "  -o:            print additional offset column\n"
  "  -x n:          this is an option eating n\n"
  "  -X <str>:      this is an option eating a string\n"
  "  --rx <regex>:  gives an regex to match string provided by '-X'\n"
  "  -e hex:        this is an hex/dec option eating a hex/dec string\n"
  "  ox=hex:        this is an hex/dec option eating a hex/dec string\n"
  "  -y yyyy:       min year to consider a track as valid (default 2002)\n"
  "  -Y yyyy:       max year to consider a track as valid (default to 'now')\n"
  "  -h|--help:     print this help\n"
  "  -v|--version:  print version of program\n"
//|************************ 80 chars width ****************************************|
        );

  if (iErr == ERR_NOERR)
    printf("%s", csMsg.cStr);
  else
    fprintf(stderr, "%s", csMsg.cStr);

  csFree(&csMsg);

  exit(iErr);
}

/*******************************************************************************
 * Name:  dispatchError
 * Purpose: Print out specific error message, if any occurres.
 *******************************************************************************/
void dispatchError(int rv, char* pcMsg) {
  cstr csMsg = csNew(pcMsg);
  cstr csErr = csNew("");

  if (rv == ERR_NOERR) return;

  if (rv == ERR_ARGS) csSet(&csErr, sERR_ARGS);
  if (rv == ERR_FILE) csSet(&csErr, sERR_ARGS);
  if (rv == ERR_ELSE) csSet(&csErr, sERR_ARGS);

  // Set to '<err>: <message>', if a message was given.
  if (csMsg.len != 0) csSetf(&csErr, "%s: %s", csErr.cStr, csMsg.cStr);

  usage(rv, csErr.cStr);
}

/*******************************************************************************
* Name: shift
* Purpose: Shifts one argument from CLI and increments the counter.
*******************************************************************************/
void shift(cstr* pcsRv, int* pI, int argc, char* argv[]) {
   csSet(pcsRv, "");
   if (*pI < argc) csSet(pcsRv, argv[(*pI)++]);
}

/*******************************************************************************
 * Name:  isNumber
 * Purpose: Check if string is a int or float number.
 *******************************************************************************/
int isNumber(cstr sString, int* piSign) {
  int iDecPt = 0;

  // Assume no sign.
  *piSign = 0;

  // Check for plus or minus sign in front of number.
  if (sString.cStr[0] == '-')
    *piSign = -1;

  if (sString.cStr[0] == '+')
    *piSign = 1;

  // Continuation depends wether sign was found.
  // Check for digits and decimal point.
  for (int i = (*piSign != 0) ? 1 : 0; i < sString.len; ++i) {
    if (sString.cStr[i] == '.') {
      // Only one decimal point allowed!
      if (iDecPt)
        return NUM_NONE;
      else {
        iDecPt = 1;
        continue;
      }
    }

    // Not a digit, no number.
    if (sString.cStr[i] < '0' || sString.cStr[i] > '9')
      return NUM_NONE;
  }

  if (iDecPt)
    return NUM_FLOAT;

  return NUM_INT;
}

/*******************************************************************************
 * Name:  ticks2datetime
 * Purpose: Converts integer to "2017/11/03, 11:14:23" + txt string.
 *******************************************************************************/
void ticks2datetime(cstr* pcsTxt, const char* pacTxt, time_t tTicks) {
  char       acTime[30] = {0};
  struct tm* psTime     = gmtime(&tTicks);

  // Returns "2017/11/03, 11:14:23" => 21 Bytes including '\0' Byte.
  strftime(acTime, sizeof(acTime), "%Y/%m/%d, %H:%M:%S", psTime);
  csSetf(pcsTxt, "%s%s", acTime, pacTxt);
}

/*******************************************************************************
 * Name:  datetime2ticks
 * Purpose: Converts ""2017/11/03, 11:14:23 (UTC)" string to ticks.
 *******************************************************************************/
time_t datetime2ticks(int fUseString, const char* pcTime,
                      int iYear, int iMonth, int iDay,
                      int iHour, int iMin,   int iSec) {
  cstr      csItem  = csNew("");
  struct tm sTime   = {0};

  //                   1111111111222222
  //         01234567890123456789012345
  // Assume "2017/11/03, 11:14:23 (UTC)"
  if (fUseString) {
    csMid(&csItem, pcTime,  0, 4);
    iYear = (int) cstr2ll(csItem);

    csMid(&csItem, pcTime,  5, 2);
    iMonth = (int) cstr2ll(csItem);

    csMid(&csItem, pcTime,  8, 2);
    iDay = (int) cstr2ll(csItem);

    csMid(&csItem, pcTime, 12, 2);
    iHour = (int) cstr2ll(csItem);

    csMid(&csItem, pcTime, 15, 2);
    iMin = (int) cstr2ll(csItem);

    csMid(&csItem, pcTime, 18, 2);
    iSec = (int) cstr2ll(csItem);
  }

  // Corrections
  iYear  -= 1900;
  iMonth -= 1;

  // Fille struct;
  sTime.tm_year = iYear;    // Year	- 1900.
  sTime.tm_mon  = iMonth;   // Month.   [0-11]
  sTime.tm_mday = iDay;     // Day.     [1-31]
  sTime.tm_hour = iHour;    // Hours.   [0-23]
  sTime.tm_min  = iMin;     // Minutes. [0-59]
  sTime.tm_sec  = iSec;     // Seconds. [0-60] (1 leap second)

  sTime.tm_isdst = -1;

  // Just tick away ...
  return mktime(&sTime) - timezone;
}

/*******************************************************************************
 * Name:  getHexIntParm
 * Purpose: Converts parameter entered as hexadecimal with '0x' prefix or as
 *          decimal with postfix K, M, G (meaning Kilo- Mega- and Giga-bytes
 *          based on 1024).
 *******************************************************************************/
int getHexIntParm(cstr csParm, int* piErr) {
  cstr csPre  = csNew("");
  cstr csPost = csNew("");
  int  fHex   = 0;
  int  iPost  = 1;
  int  iSign  = 0;
  int  iVal   = 0;

  *piErr = 0;

  // Sanity check.
  if (csParm.len == 0) {
    *piErr = 1;
    return 0;
  }

  // Get possible pre- and postfixes.
  csMid(&csPre,  csParm.cStr,  0, 2);
  csMid(&csPost, csParm.cStr, -1, 1);

  // Found hex prefix.
  if (!strcmp(csPre.cStr, "0x")) fHex = 1;

  // Calc possible multiplier from integer postfix.
  if (csPost.cStr[0] == 'k') iPost = 1024;
  if (csPost.cStr[0] == 'K') iPost = 1024;
  if (csPost.cStr[0] == 'm') iPost = 1024 * 1024;
  if (csPost.cStr[0] == 'M') iPost = 1024 * 1024;
  if (csPost.cStr[0] == 'g') iPost = 1024 * 1024 * 1024;
  if (csPost.cStr[0] == 'G') iPost = 1024 * 1024 * 1024;

  // Hex or integer
  if (fHex == 1)
    iVal = (int) csHex2ll(csParm);
  else
    iVal = ((int) cstr2ll(csParm)) * iPost;

  // Remove postfix to use isNumber().
  if (iPost > 1) csMid(&csParm, csParm.cStr, 0, csParm.len - 1);

  // Error checks.
  if (csParm.len == 0)                                  *piErr = 1;
  if (fHex == 1 && iPost > 1)                           *piErr = 1;
  if (fHex == 0 && isNumber(csParm, &iSign) != NUM_INT) *piErr = 1;

  csFree(&csPre);
  csFree(&csPost);

  return iVal;
}

/*******************************************************************************
 * Name:  getOptions
 * Purpose: Filters command line.
 *******************************************************************************/
void getOptions(int argc, char* argv[]) {
  cstr csArgv = csNew("");
  cstr csRv   = csNew("");
  cstr csOpt  = csNew("");
  int  iArg   = 1;  // Omit programm name in arg loop.
  int  iChar  = 0;
  char cOpt   = 0;
  int  iRv    = 0;
  int  iPos   = 0;
  int  iErr   = 0;
  int  iSign  = 0;

  // Set defaults.
  g_tOpts.iTestMode   = 0;
  g_tOpts.lByteOff    = 0;
  g_tOpts.iPrtOff     = 0;
  g_tOpts.iOptX       = 0;
  g_tOpts.csOptX      = csNew("0f:aa:08:7e:50");
  g_tOpts.csRx        = csNew("([0-9a-fA-F]{2})(:?)");
  g_tOpts.iTicksMin   = 2002;   // This will be converted into unix ticks.
  g_tOpts.iTicksMax   = NO_TICK;

  // Init free argument's dynamic array.
  dacsInit(&g_tArgs);

  // Loop all arguments from command line POSIX style.
  while (iArg < argc) {
next_argument:
    shift(&csArgv, &iArg, argc, argv);
    if(strcmp(csArgv.cStr, "") == 0)
      continue;

    // Long options:
    if (csArgv.cStr[0] == '-' && csArgv.cStr[1] == '-') {
      if (!strcmp(csArgv.cStr, "--help")) {
        usage(ERR_NOERR, "");
      }
      if (!strcmp(csArgv.cStr, "--version")) {
        version();
      }
      if (!strcmp(csArgv.cStr, "--test")) {
        g_tOpts.iTestMode = 1;
        continue;
      }
      if (!strcmp(csArgv.cStr, "--off")) {
        g_tOpts.iPrtOff = 1;
        continue;
      }
      if (!strcmp(csArgv.cStr, "--rx")) {
        shift(&csRv, &iArg, argc, argv);
        if (csRv.len == 0)
          dispatchError(ERR_ARGS, "OptX is missing");
        csSet(&g_tOpts.csRx, csRv.cStr);
        continue;
      }
      dispatchError(ERR_ARGS, "Invalid long option");
    }

    // Short options:
    if (csArgv.cStr[0] == '-') {
      for (iChar = 1; iChar < csArgv.len; ++iChar) {
        cOpt = csArgv.cStr[iChar];
        if (cOpt == 'h') {
          usage(ERR_NOERR, "");
        }
        if (cOpt == 'v') {
          version();
        }
        if (cOpt == 't') {
          g_tOpts.iTestMode = 1;
          continue;
        }
        if (cOpt == 'o') {
          g_tOpts.iPrtOff = 1;
          continue;
        }
        // This version ...
        if (cOpt == 'x') {
          shift(&csRv, &iArg, argc, argv);
          if (csRv.len == 0 || isNumber(csRv, &iSign) != NUM_INT)
            dispatchError(ERR_ARGS, "No valid OptX or missing");
          g_tOpts.iOptX = (int) cstr2ll(csRv);
          continue;
        }
        // ... or hex.
        if (cOpt == 'e') {
          shift(&csRv, &iArg, argc, argv);
          iRv = getHexIntParm(csRv, &iErr);
          if (iErr)
            dispatchError(ERR_ARGS, "No valid OptX or missing");
          g_tOpts.iOptX = iRv;
          continue;
        }
        // ... or that.
        if (cOpt == 'X') {
          shift(&csRv, &iArg, argc, argv);
          if (csRv.len == 0)
            dispatchError(ERR_ARGS, "OptX is missing");
          csSet(&g_tOpts.csOptX, csRv.cStr);
          continue;
        }
        if (cOpt == 'y') {
          shift(&csRv, &iArg, argc, argv);
          if (csRv.len == 0 || isNumber(csRv, &iSign) != NUM_INT)
            dispatchError(ERR_ARGS, "Min year is missing");
          g_tOpts.iTicksMin = (int) cstr2ll(csRv);
          continue;
        }
        if (cOpt == 'Y') {
          shift(&csRv, &iArg, argc, argv);
          if (csRv.len == 0 || isNumber(csRv, &iSign) != NUM_INT)
            dispatchError(ERR_ARGS, "Max year is missing");
          g_tOpts.iTicksMax = (int) cstr2ll(csRv);
          continue;
        }
        dispatchError(ERR_ARGS, "Invalid short option");
      }
      goto next_argument;
    }
    // Equality options:
    iPos = csInStr(csArgv.cStr, "=");
    if (iPos > 0) {
      csSplit(&csOpt, &csRv, csArgv.cStr, "=");
      if (!strcmp(csOpt.cStr, "ox")) {
        iRv = getHexIntParm(csRv, &iErr);
        if (iErr)
          dispatchError(ERR_ARGS, "No valid ox or missing");
        g_tOpts.iOptX = iRv;
        continue;
      }
      dispatchError(ERR_ARGS, "Invalid equality option");
    }
    // Else, it's just a filename.
    dacsAdd(&g_tArgs, csArgv.cStr);
  }

  // Sanity check of arguments and flags.
  if (g_tArgs.iCount == 0)
    dispatchError(ERR_ARGS, "No file");

  if (g_tOpts.iTicksMin < 1970 || g_tOpts.iTicksMin > 2038)
    dispatchError(ERR_ARGS, "Min year out of limits (1970 - 2038)");

  if (g_tOpts.iTicksMax != NO_TICK &&
     (g_tOpts.iTicksMax < 1970 || g_tOpts.iTicksMax > 2038))
    dispatchError(ERR_ARGS, "Max year out of limits (1970 - 2038)");

  // Get timestamps limits for verification.
  g_tOpts.iTicksMin = datetime2ticks(0, "", g_tOpts.iTicksMin, 1, 1, 0, 0, 0);
  if (g_tOpts.iTicksMax == NO_TICK)
    g_tOpts.iTicksMax = time(NULL);
  else
    // To fit this year it must end 1 sec befor end of last year!
    g_tOpts.iTicksMax = datetime2ticks(0, "", g_tOpts.iTicksMax - 1, 12, 31, 23, 59, 59);

  if (g_tOpts.iTicksMin >= g_tOpts.iTicksMax)
    dispatchError(ERR_ARGS, "'-Y' should be grater than '-y'");

  // Free string memory.
  csFree(&csArgv);
  csFree(&csRv);
  csFree(&csOpt);

  return;
}

/*******************************************************************************
 * Name:  round
 * Purpose: Returns float, rounded to given count of digits.
 *******************************************************************************/
ldbl roundN(ldbl ldA, int iDigits) {
  int iFactor = 1;
  while (iDigits--) iFactor *= 10;
  return ((ldbl) ((int) (ldA * iFactor + 0.5))) / iFactor;
}

/*******************************************************************************
 * Name:  toWgs84
 * Purpose: Converts coordinates to WGS84.
 *******************************************************************************/
int toWgs84(ldbl* pldblLon, ldbl* pldblLat) {
  // Convert given cordinates to WGS84.
  *pldblLon = *pldblLon / 1e5;
  *pldblLat = *pldblLat / 1e5;

  // Round to 5 digits.
  *pldblLon = roundN(*pldblLon, 5);
  *pldblLat = roundN(*pldblLat, 5);

  // Check coordinate's integrity.
  if (*pldblLon < -180 || *pldblLon > 180) return 0;
  if (*pldblLat <  -90 || *pldblLat >  90) return 0;

  return 1;
}

/*******************************************************************************
 * Name:  printHeader
 * Purpose: Prints generic csv file header.
 *******************************************************************************/
void printHeader(void) {
  printf("Remark\tLongitude\tLatitude\tLabel");
  if (g_tOpts.iPrtOff)
    printf("\tOffset");
  printf("\n");
}

/*******************************************************************************
 * Name:  getData
 * Purpose: Gets raw bytes and converts them to readable data.
 *******************************************************************************/
int getData(FILE* hFile) {
  return 0;
}

/*******************************************************************************
 * Name:  printEntry
 * Purpose: Prints generic csv file entry.
 *******************************************************************************/
void printEntry(int iOffset) {
  printf("<Remark>\t<Longitude>\t<Latitude>\t<Label>");
  if (g_tOpts.iPrtOff)
    printf("\t%d", iOffset);
  printf("\n");
}

/*******************************************************************************
 * Name:
 * Purpose: .
 *******************************************************************************/
void doRegex(const char* pcToSearch, const char* pcRegex) {
  t_rx_matcher rxMatcher = {0};
  t_array_cstr dacsMatch = {0};
  cstr         csErr     = csNew("");
  int          iErr      = 0;

  printf("\nMatch: %s\n", pcToSearch);
  printf("With:  %s\n",   pcRegex);

  // Init cstr array, which holds all macthes.
  dacsInit(&dacsMatch);

  // Compile regex and init global matcher struct.
  if (rxInitMatcher(&rxMatcher, 0, pcToSearch, pcRegex, "", &csErr) != RX_NO_ERROR) {
    printf("%s\n", csErr.cStr);
    goto free_and_exit;
  }

  // rxMatch() crams all sub-macthes into cstr array and signals, if matching
  // reached end of string.
  while (rxMatch(&dacsMatch, &rxMatcher, &iErr, &csErr)) {
    for (int i = 0; i < dacsMatch.iCount; ++i)
      printf("$%d = '%s'\n", i, dacsMatch.pcsData[i].cStr);
    printf("----\n");
  }

  // Free memory of all used structs.
free_and_exit:
  rxDelMatcher(&rxMatcher);
  dacsFree(&dacsMatch);
}

/*******************************************************************************
 * Name:  debug
 *******************************************************************************/
void debug(void) {
  cstr csMin   = csNew("");
  cstr csMax   = csNew("");
  cstr csSubRx = csNew("");
  cstr csRx    = csNew("");

  ticks2datetime(&csMin, " (UTC)", g_tOpts.iTicksMin);
  ticks2datetime(&csMax, " (UTC)", g_tOpts.iTicksMax);

  printf("g_tOpts.iTestMode   = %d\n",        g_tOpts.iTestMode);
  printf("g_tOpts.iPrtOff     = %d\n",        g_tOpts.iPrtOff);
  printf("g_tOpts.iOptX       = %d\n",        g_tOpts.iOptX);
  printf("g_tOpts.csOptX.cStr = '%s'\n",      g_tOpts.csOptX.cStr);
  printf("g_tOpts.csRx.cStr   = '%s'\n",      g_tOpts.csRx.cStr);
  printf("g_tOpts.iTicksMin   = %10d (%s)\n", g_tOpts.iTicksMin, csMin.cStr);
  printf("g_tOpts.iTicksMax   = %10d (%s)\n", g_tOpts.iTicksMax, csMax.cStr);

  printf("Free argument's dynamic array: ");
  for (int i = 0; i < g_tArgs.iCount - 1; ++i)
    printf("%s, ", g_tArgs.pcsData[i].cStr);
  printf("%s\n", g_tArgs.pcsData[g_tArgs.iCount - 1].cStr);

  // How to assemble regex via cstr variables.
  // "([0-9a-fA-F]{2}):([0-9a-fA-F]{2}):([0-9a-fA-F]{2}):([0-9a-fA-F]{2}):([0-9a-fA-F]{2})"
  csSet(&csSubRx, "([0-9a-fA-F]{2})");
  csSetf(&csRx, "%s:%s:%s:%s:%s", csSubRx.cStr, csSubRx.cStr, csSubRx.cStr, csSubRx.cStr, csSubRx.cStr);
  doRegex("0f:aa:08:7e:50", csRx.cStr);

  doRegex(g_tOpts.csOptX.cStr, g_tOpts.csRx.cStr);

  csFree(&csMin);
  csFree(&csMax);
  csFree(&csSubRx);
  csFree(&csRx);

  exit(0);
}


//******************************************************************************
//* main

int main(int argc, char *argv[]) {
  FILE* hFile;

  // Get options and dispatch errors, if any.
  getOptions(argc, argv);

  // For timezone var in datetime2ticks().
  tzset();

  debug();  // Delete, when debugging is finished.

  printHeader();

  // Get all data from all files.
  for (int i = 0; i < g_tArgs.iCount; ++i) {
    if (!(hFile = fopen(g_tArgs.pcsData[i].cStr, "rb"))) {
      cstr csMsg = csNew("");
      csSetf(&csMsg, "Can't open '%s'", g_tArgs.pcsData[i].cStr);
      dispatchError(ERR_FILE, csMsg.cStr);
    }
//-- file ----------------------------------------------------------------------
    while (!feof(hFile)) {
      getData(hFile);
      printEntry(0);
    }
//-- file ----------------------------------------------------------------------
    fclose(hFile);
  }

  // Free all used memory, prior end of program.
  dacsFree(&g_tArgs);

  return ERR_NOERR;
}

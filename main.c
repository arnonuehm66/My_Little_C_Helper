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
 ** 30.11.2018  JE    Now doRegex() uses flags directly and adjustet ouput.
 ** 01.11.2018  JE    Added '--rxF' to add flags to rx compilation.
 ** 24.12.2018  JE    Simplified version().
 ** 10.01.2019  JE    Added 'typedef long int li'.
 ** 31.01.2019  JE    Now uses c_string_v0.9.3.h with improved csSet().
 ** 12.02.2019  JE    Now free csItem in datetime2ticks() to avoid memory leak.
 ** 07.03.2019  JE    Now structs are all named.
 ** 23.04.2019  JE    Now use c_dynamic_arrays.h v0.3.3.
 ** 24.04.2019  JE    Changed int's to time_t's ticks.
 ** 31.05.2019  JE    Added openFile() and getFileSize() for convenience.
 ** 11.06.2019  JE    Now use c_string.h v0.11.1 with UTF-8 support.
 ** 11.06.2019  JE    Extended debug() and added printCsInternals().
 ** 11.06.2019  JE    Now use c_my_regex.h v0.2.1.
 ** 04.07.2019  JE    Extended doRegex() to add pos after match in output.
 ** 10.07.2019  JE    Added toInt() working with endian.h.
 ** 17.07.2019  JE    Now tm_isdst is set permanentely to 0 in datetime2ticks().
 ** 17.07.2019  JE    Added '-d' for string -> ticks -> string conversion test.
 ** 30.11.2019  JE    Outsourced standard functions to stdfcns.c.
 ** 07.01.2020  JE    Added a default '-d' whwn nothing is given.
 ** 17.01.2020  JE    Now use functions, includes and defines from 'stdfcns.c'.
 ** 21.01.2020  JE    Now use v0.4.2 rxMatcher() functions.
 ** 21.01.2020  JE    Now use c_my_regex.h v0.6.1.
 ** 11.03.2020  JE    Now use stdfcns.c v0.3.1.
 ** 11.03.2020  JE    Minor fixes in printf format string.
 ** 20.03.2020  JE    Added t_coord and changed toWgs84() accordingly.
 ** 20.03.2020  JE    Now use c_string.h v0.13.1.
 ** 24.03.2020  JE    Now use c_my_regex.h v0.6.3.
 ** 31.03.2020  JE    Now use c_my_regex.h v0.7.2.
 ** 31.03.2020  JE    Now use c_dynamic_array.h v0.4.1.
 ** 02.04.2020  JE    Added ability to read big files in chunks of 1GiB + 1KiB.
 ** 02.04.2020  JE    Added regex and converter functions.
 ** 04.04.2020  JE    Now use c_string.h v0.14.1.
 ** 12.04.2020  JE    Now use stdfcns.c v0.5.2.
 ** 15.04.2020  JE    Now use stdfcns.c v0.6.1.
 ** 05.08.2020  JE    Now use stdfcns.c v0.7.1 and g_csMename.
 ** 01.10.2020  JE    Added initGlobalVars() and moved init of type and prec.
 ** 01.10.2020  JE    Removed 'g_csMename = csNew("")'.
 ** 05.02.2021  JE    Added '-p' for printing progress to stderr.
 ** 05.02.2021  JE    Changed all long to size_t.
 ** 19.04.2021  JE    Now use dynamic-array-macros.
 ** 30.11.2021  JE    Added csFree() in main() for all global cstr vars.
 ** 30.11.2021  JE    Added freeEntry().
 ** 11.09.2025  JE    Now use csEq() family of functions.
 *******************************************************************************
 ** Skript tested with:
 ** TestDvice 123a.
 *******************************************************************************/


//******************************************************************************
//* includes & namespaces

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "../../libs/c_string.h"
//#include "../../libs/c_my_regex.h"
//#include "../../libs/c_dynamic_arrays_macros.h"
#include "c_string.h"
#include "c_my_regex.h"
#include "c_dynamic_arrays_macros.h"


//******************************************************************************
//* defines & macros

#define ME_VERSION "0.0.53"
cstr g_csMename;

#define ERR_NOERR 0x00
#define ERR_ARGS  0x01
#define ERR_FILE  0x02
#define ERR_ICONV 0x03
#define ERR_REGEX 0x04
#define ERR_ELSE  0xff

#define sERR_ARGS  "Argument error"
#define sERR_FILE  "File error"
#define sERR_ICONV "iconv error"
#define sERR_REGEX "Regex error"
#define sERR_ELSE  "Unknown error"

// getOptions(): Defines empty values.
#define NO_TICK ((time_t) ~0)   // Fancy contruction to get a (-1). ;o)


//******************************************************************************
//* outsourced standard functions, includes and defines

//#include "../../libs/stdfcns.c"
#include "stdfcns.c"


//******************************************************************************
//* typedefs

// toWgs84().
typedef struct s_coord {
  ldbl ldlVal;
  cstr csVal;
} t_coord;

// Binary data chunks from file.
typedef struct s_data {
  uchar*  pBytes;
  size_t  sSize;
} t_data;

// Arguments and options.
typedef struct s_options {
  int    iTestMode;
  size_t sByteOff;
  int    iPrtOff;
  int    iPrtPrgrs;
  int    iOptX;     // Integer verion.
  cstr   csOptX;    // String version.
  cstr   csRx;
  cstr   csRxF;
  time_t tTicksMin;
  time_t tTicksMax;
  cstr   csDateTime;
  time_t tDateTime;
} t_options;

// Entry composition
typedef struct s_entry {
  int     iType;
  t_coord tcLon;
  t_coord tcLat;
  cstr    csLbl;
} t_entry;

// Dynamic array macro sruct declaration.
// s_array(cstr);
// Used already in regex!


//******************************************************************************
//* Global variables

char* g_cType[8] = {0};

t_rx_matcher g_rx_c2Lbl        = {0};
t_rx_matcher g_rx_c2Coords     = {0};
t_rx_matcher g_rx_c7TomTomLive = {0};

t_entry g_tE;

// Arguments
t_options     g_tOpts;  // CLI options and arguments.
t_array(cstr) g_tArgs;  // Free arguments.


//******************************************************************************
//* Functions

/*******************************************************************************
 * Name:  usage
 * Purpose: Print help text and exit program.
 *******************************************************************************/
void usage(int iErr, const char* pcMsg) {
  cstr csMsg = csNew(pcMsg);

  // Print at least one newline with message.
  if (csMsg.len != 0)
    csCat(&csMsg, csMsg.cStr, "\n\n");

  csSetf(&csMsg, "%s"
//|************************ 80 chars width ****************************************|
   "usage: %s [-t] [-b n] [-o] [-p] [-x n] [-X <str> [--rx <regex>] [--rxF <flags>]] [-e hex] [ox=hex] [-y yyyy [-Y yyyy]] file1 [file2 ...]\n"
   "       %s [-h|--help|-v|--version]\n"
   " What the programm should do.\n"
   " '-e' and 'ox=' can be entered as hexadecimal with '0x' prefix or as decimal\n"
   " with postfix K, M, G (meaning Kilo- Mega- and Giga-bytes based on 1024).\n"
   "  -t:            don't execute printed commands (default execute)\n"
   "  -b n:          byte offset per file (default 0)\n"
   "  -o:            print additional offset column\n"
   "  -x n:          this is an option eating n\n"
   "  -X <str>:      this is an option eating a string\n"
   "  --rx <regex>:  gives an regex to match string provided by '-X'\n"
   "  --rxF <flags>: flags with wich regex will be compiled (i.e. 'xims')\n"
   "  -e hex:        this is an hex/dec option eating a hex/dec string\n"
   "  ox=hex:        this is an hex/dec option eating a hex/dec string\n"
   "  -y yyyy:       min year to consider a track as valid (default 2002)\n"
   "  -Y yyyy:       max year to consider a track as valid (default to 'now')\n"
   "  -d 'yyyy/mm/dd, hh:mm:ss':\n"
   "                 convert string to ticks and back to string for testing\n"
   "  -h|--help:     print this help\n"
   "  -v|--version:  print version of program\n"
//|************************ 80 chars width ****************************************|
         ,csMsg.cStr,
         g_csMename.cStr, g_csMename.cStr
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
void dispatchError(int rv, const char* pcMsg) {
  cstr csMsg = csNew(pcMsg);
  cstr csErr = csNew("");

  if (rv == ERR_NOERR) return;

  if (rv == ERR_ARGS)  csSet(&csErr, sERR_ARGS);
  if (rv == ERR_FILE)  csSet(&csErr, sERR_FILE);
  if (rv == ERR_ICONV) csSet(&csErr, sERR_ICONV);
  if (rv == ERR_REGEX) csSet(&csErr, sERR_REGEX);
  if (rv == ERR_ELSE)  csSet(&csErr, sERR_ELSE);

  // Set to '<err>: <message>', if a message was given.
  if (csMsg.len != 0) csSetf(&csErr, "%s: %s", csErr.cStr, csMsg.cStr);

  usage(rv, csErr.cStr);
}

/*******************************************************************************
 * Name:  initEntry
 * Purpose: Inits entry's struct to 'zero'.
 *******************************************************************************/
void initEntry() {
  g_tE.iType        = 0;
  g_tE.tcLon.ldlVal = 0.0;
  g_tE.tcLon.csVal  = csNew("-");
  g_tE.tcLat.ldlVal = 0.0;
  g_tE.tcLat.csVal  = csNew("-");
  g_tE.csLbl        = csNew("");
}

/*******************************************************************************
 * Name:  freeEntry
 * Purpose: Frees entry's cstr vars.
 *******************************************************************************/
void freeEntry() {
  csFree(&g_tE.tcLon.csVal);
  csFree(&g_tE.tcLat.csVal);
  csFree(&g_tE.csLbl);
}

/*******************************************************************************
 * Name:  getOptions
 * Purpose: Filters command line.
 *******************************************************************************/
void getOptions(int argc, char* argv[]) {
  cstr csArgv = csNew("");
  cstr csRv   = csNew("");
  cstr csOpt  = csNew("");
  int  iArg   = 1;  // Omit program name in arg loop.
  int  iChar  = 0;
  char cOpt   = 0;
  int  iPos   = 0;

  // Set defaults.
  g_tOpts.iTestMode  = 0;
  g_tOpts.sByteOff   = 0;
  g_tOpts.iPrtOff    = 0;
  g_tOpts.iOptX      = 0;
  g_tOpts.csOptX     = csNew("0f:aa:08:7e:50");
  g_tOpts.csRx       = csNew("([0-9a-fA-F]{2})(:?)");
  g_tOpts.csRxF      = csNew("");
  g_tOpts.tTicksMin  = 2002;   // This will be converted into unix ticks.
  g_tOpts.tTicksMax  = NO_TICK;
  g_tOpts.csDateTime = csNew("");
  g_tOpts.tDateTime  = NO_TICK;

  // Init free argument's dynamic array.
  daInit(cstr, g_tArgs);

  // Loop all arguments from command line POSIX style.
  while (iArg < argc) {
next_argument:
    shift(&csArgv, &iArg, argc, argv);
    if (csEq(csArgv, ""))
      continue;

    // Long options:
    if (csArgv.cStr[0] == '-' && csArgv.cStr[1] == '-') {
      if (csEq(csArgv, "--help")) {
        usage(ERR_NOERR, "");
      }
      if (csEq(csArgv, "--version")) {
        version();
      }
      if (csEq(csArgv, "--test")) {
        g_tOpts.iTestMode = 1;
        continue;
      }
      if (csEq(csArgv, "--off")) {
        g_tOpts.iPrtOff = 1;
        continue;
      }
      if (csEq(csArgv, "--rx")) {
        if (! getArgStr(&g_tOpts.csRx, &iArg, argc, argv, ARG_CLI, NULL))
          dispatchError(ERR_ARGS, "rx is missing");
        continue;
      }
      if (csEq(csArgv, "--rxF")) {
        if (! getArgStr(&g_tOpts.csRxF, &iArg, argc, argv, ARG_CLI, NULL))
          dispatchError(ERR_ARGS, "rxF is missing");
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
        if (cOpt == 'b') {
          if (! getArgLong((ll*) &g_tOpts.sByteOff, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "Byte offset is missing");
          continue;
        }
        if (cOpt == 'o') {
          g_tOpts.iPrtOff = 1;
          continue;
        }
        // This version ...
        if (cOpt == 'x') {
          if (! getArgLong((ll*) &g_tOpts.iOptX, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "No valid OptX or missing");
          continue;
        }
        // ... or hex.
        if (cOpt == 'e') {
          if (! getArgHexLong((ll*) &g_tOpts.iOptX, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "No valid OptX or missing");
          continue;
        }
        // ... or that.
        if (cOpt == 'X') {
          if (! getArgStr(&g_tOpts.csOptX, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "OptX is missing");
          continue;
        }
        if (cOpt == 'y') {
          if (! getArgTime(&g_tOpts.tTicksMin, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "Min year is missing");
          continue;
        }
        if (cOpt == 'Y') {
          if (! getArgTime(&g_tOpts.tTicksMax, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "Max year is missing");
          continue;
        }
        if (cOpt == 'd') {
          if (! getArgStr(&g_tOpts.csDateTime, &iArg, argc, argv, ARG_CLI, NULL))
            dispatchError(ERR_ARGS, "Date time missing");
          continue;
        }
        dispatchError(ERR_ARGS, "Invalid short option");
      }
      goto next_argument;
    }
    // Equality options:
    iPos = csInStr(0, csArgv.cStr, "=");
    if (iPos > 0) {
      csSplit(&csOpt, &csRv, csArgv.cStr, "=");
      if (csEq(csOpt, "ox")) {
        if (! getArgHexLong((ll*) &g_tOpts.iOptX, NULL, 0, NULL, ARG_VAL, csRv.cStr))
          dispatchError(ERR_ARGS, "No valid ox or missing");
        continue;
      }
      dispatchError(ERR_ARGS, "Invalid equality option");
    }
    // Else, it's just a filename.
    daAdd(cstr, g_tArgs, csNew(csArgv.cStr));
  }

  // Sanity check of arguments and flags.
  if (g_tArgs.sCount == 0)
    dispatchError(ERR_ARGS, "No file");

  if (g_tOpts.tTicksMin < 1970 || g_tOpts.tTicksMin > 2038)
    dispatchError(ERR_ARGS, "Min year out of limits (1970 - 2038)");

  if (g_tOpts.tTicksMax != NO_TICK &&
     (g_tOpts.tTicksMax < 1970 || g_tOpts.tTicksMax > 2038))
    dispatchError(ERR_ARGS, "Max year out of limits (1970 - 2038)");

  // Get timestamps limits for verification.
  g_tOpts.tTicksMin = datetime2ticks(0, "", g_tOpts.tTicksMin, 1, 1, 0, 0, 0);
  if (g_tOpts.tTicksMax == NO_TICK)
    g_tOpts.tTicksMax = time(NULL);
  else
    // To fit this year it must end 1 sec befor end of last year!
    g_tOpts.tTicksMax = datetime2ticks(0, "", g_tOpts.tTicksMax - 1, 12, 31, 23, 59, 59);

  if (g_tOpts.tTicksMin >= g_tOpts.tTicksMax)
    dispatchError(ERR_ARGS, "'-Y' should be grater than '-y'");

  // Free string memory.
  csFree(&csArgv);
  csFree(&csRv);
  csFree(&csOpt);
}

/*******************************************************************************
 * Name:  initMatcher
 * Purpose: Initialize Matcher struct with regex string.
 *******************************************************************************/
void initMatcher(t_rx_matcher* pMatcher, const char* pcRegex) {
  cstr csErr = csNew("");
  if (rxInitMatcher(pMatcher, pcRegex, "", &csErr) != RX_NO_ERROR)
    dispatchError(ERR_REGEX, csErr.cStr);
}

/*******************************************************************************
 * Name:  initGlobalVars
 * Purpose: Initializes all global varaiables.
 *******************************************************************************/
void initGlobalVars(void) {
  g_cType[0] = "-";
  g_cType[1] = "Entered via map; zip or coord."; // 1
  g_cType[2] = "Entered via favorite";           // 2
  g_cType[3] = "Home location";                  // 3
  g_cType[4] = "Entered via address";            // 4
  g_cType[5] = "Entered via POI";                // 5
  g_cType[6] = "Start of last calculated route"; // 6
  g_cType[7] = "-";
}

/*******************************************************************************
 * Name:  initGlobalRegexes
 * Purpose: Assembles all global regexes.
 *******************************************************************************/
void initGlobalRegexes(void) {
  char* C               = "[\\x00-\\xff]";
  cstr  cs_rx_cPrec     = csNew("");
  cstr  cs_rx_cType     = csNew("");
  cstr  cs_rx_c2Coords1 = csNew("");
  cstr  cs_rx_c2Coords2 = csNew("");
  cstr  cs_rx_temp      = csNew("");

  //  Main header fields:          Tag   Length  Tag Length  Value
  csSetf(&cs_rx_cPrec,     "(?x: \\x81\\x19  \\x03  \\x68  \\x01  (%s) )", C);
  csSetf(&cs_rx_cType,     "(?x: \\x82\\x19  \\x03  \\x68  \\x01  (%s) )", C);
  csSetf(&cs_rx_c2Coords1, "(?x: \\x83\\x19  \\x0a  \\x66  \\x08  (%s{4})(%s{4}) )", C, C);
  csSetf(&cs_rx_c2Coords2, "(?x: \\x84\\x19  \\x0a  \\x66  \\x08  (%s{4})(%s{4}) )", C, C);

  // Rest of labels and coordinates.
  csSetf(&cs_rx_temp, "(?x: \\x85\\x19 (%s) \\x64 (%s) )", C, C);
  initMatcher(&g_rx_c2Lbl, cs_rx_temp.cStr);

  csSetf(&cs_rx_temp, "(?x: \\xf6\\x1c \\x0a \\x66 \\x08 (%s{4})(%s{4}) )", C, C);
  initMatcher(&g_rx_c2Coords, cs_rx_temp.cStr);

  //  qr/
  //    rx_cPrec rx_cType.cStr rx_c2Coords_1.cStr rx_c2Coords_2.cStr
  //      (C+?)
  //    (?= (?: rx_cPrec rx_cType rx_c2Coords_1 rx_c2Coords_2 ) | \Z | )
  //  /xo
  // Last " | )" must be there to prevent ([\x00-\xff]?) causing limit errors!
  csSetf(&cs_rx_temp,
           "(?x:"
             "%s %s %s %s"
               "(%s?)"
             "(?= (?: %s %s %s %s ) | \\Z | )"
           ")",
           cs_rx_cPrec.cStr, cs_rx_cType.cStr, cs_rx_c2Coords1.cStr, cs_rx_c2Coords2.cStr,
           C,
           cs_rx_cPrec.cStr, cs_rx_cType.cStr, cs_rx_c2Coords1.cStr, cs_rx_c2Coords2.cStr
        );
  initMatcher(&g_rx_c7TomTomLive, cs_rx_temp.cStr);

  csFree(&cs_rx_cPrec);
  csFree(&cs_rx_cType);
  csFree(&cs_rx_c2Coords1);
  csFree(&cs_rx_c2Coords2);
  csFree(&cs_rx_temp);
}

/*******************************************************************************
 * Name:  freeRxStructs
 * Purpose: Free all global regex structs.
 *******************************************************************************/
void freeRxStructs(void) {
  rxFreeMatcher(&g_rx_c2Lbl);
  rxFreeMatcher(&g_rx_c2Coords);
  rxFreeMatcher(&g_rx_c7TomTomLive);
}

/*******************************************************************************
 * Name:  printHeader
 * Purpose: Prints generic csv file header.
 *******************************************************************************/
void printHeader(void) {
  printf("Remark\tLongitude\tLatitude\tLabel");
  if (g_tOpts.iPrtOff) printf("\tOffset");
  printf("\n");
}

//*******************************************************************************
//* Name:  getLable
//* Purpose: Cut labels from tlv: 85 19 l1 64 l2 "data".
//*******************************************************************************
void getLable(t_rx_matcher* rxMatcher, t_data* ptData, cstr* pcsLbl) {
  cstr csLbl = csNew("");
  int  iOff  = rxMatcher->dasEnd.pVal[2];
  int  iLen1 = toInt((char*) &ptData->pBytes[rxMatcher->dasStart.pVal[1]], 1);
  int  iLen2 = toInt((char*) &ptData->pBytes[rxMatcher->dasStart.pVal[2]], 1);

  // Error check.
  if (iLen1 != iLen2 + 2) return;

  // Create string with special printf format using a length and an offset.
  csSetf(&csLbl, "%.*s", iLen2, &ptData->pBytes[iOff]);

  csSanitize(&csLbl);

  csSet(pcsLbl, csLbl.cStr);

  csFree(&csLbl);
}

//*******************************************************************************
//* Name:  getLblWrapper
//* Purpose: Ease the use of getLable().
//*******************************************************************************`
void getLblWrapper(t_rx_matcher* rxM, t_data* ptD, size_t sOff, cstr* csLbl) {
  cstr csErr = csNew("");
  int  iErr  = 0;

  // Shorten by copying.
  char*  pB = (char*) ptD->pBytes;
  size_t sS = ptD->sSize;

  // Fetch matched string.
  if (rxMatch(rxM, sOff, pB, sS, &iErr, &csErr)) {
    getLable(rxM, ptD, csLbl);
    // No factor required, because ISO8859-1 never leaves latin range of characters!
    if (! csIconv(csLbl, csLbl, "ISO8859-1", "UTF-8//TRANSLIT", CS_ICONV_NO_GUESS))
      dispatchError(ERR_ICONV, "codepage conversion failed");
  }
}

//*******************************************************************************
//* Name:  getLables
//* Purpose: Searches for the labels and converts them to utf8.
//*******************************************************************************
void getLables(t_data* ptD, size_t sOff) {
  getLblWrapper(&g_rx_c2Lbl, ptD, sOff, &g_tE.csLbl);
}

/*******************************************************************************
 * Name:  toWgs84
 * Purpose: Converts coordinates to WGS84.
 *******************************************************************************/
int toWgs84(t_coord* ptcLon, t_coord* ptcLat) {
  // Convert given cordinates to WGS84.
  ptcLon->ldlVal = ptcLon->ldlVal / 1e5;
  ptcLat->ldlVal = ptcLat->ldlVal / 1e5;

  // Round to 5 digits.
  ptcLon->ldlVal = roundN(ptcLon->ldlVal, 5);
  ptcLat->ldlVal = roundN(ptcLat->ldlVal, 5);

  // Check coordinate's integrity.
  if (ptcLon->ldlVal < -180.0 || ptcLon->ldlVal > 180.0) {
    ptcLon->ldlVal = 0;
    ptcLat->ldlVal = 0;
    return 0;
  }
  if (ptcLat->ldlVal <  -90.0 || ptcLat->ldlVal >  90.0) {
    ptcLon->ldlVal = 0;
    ptcLat->ldlVal = 0;
    return 0;
  }

  // Write to 5 digits formated floating points into srings.
  csSetf(&ptcLon->csVal, "%.5Lf", ptcLon->ldlVal);
  csSetf(&ptcLat->csVal, "%.5Lf", ptcLat->ldlVal);

  return 1;
}

//*******************************************************************************
//* Name:  getCoord
//* Purpose: Converts one or two coordinate pairs from bin to integer.
//*******************************************************************************
void getCoord(t_rx_matcher* rxM, t_data* ptD, ldbl* ldLo1, ldbl* ldLa1, ldbl* ldLo2, ldbl* ldLa2) {
  *ldLo1 = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[1]], 4);
  *ldLa1 = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[2]], 4);
  if (! (ldLo2 != NULL && ldLa2 != NULL)) return;
  *ldLo2 = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[3]], 4);
  *ldLa2 = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[4]], 4);
}

//*******************************************************************************
//* Name:  getCordWrapper
//* Purpose: Ease the use of getCoord().
//*******************************************************************************
void getCordWrapper(t_rx_matcher* rxM, t_data* ptD, size_t sOff, t_coord* tcLon1, t_coord* tcLat1, t_coord* tcLon2, t_coord* tcLat2) {
  cstr csErr = csNew("");
  int  iErr  = 0;

  // Shorten by copying.
  char*  pB    = (char*) ptD->pBytes;
  size_t sS    = ptD->sSize;
  ldbl*  pLon1 = &tcLon1->ldlVal;
  ldbl*  pLat1 = &tcLat1->ldlVal;
  ldbl*  pLon2 = &tcLon2->ldlVal;
  ldbl*  pLat2 = &tcLat2->ldlVal;

  // Get one or two pairs of coordinates from matches.
  if (rxMatch(rxM, sOff, pB, sS, &iErr, &csErr)) {
    getCoord(rxM, ptD, pLon1, pLat1, pLon2, pLat2);
    toWgs84(tcLon1, tcLat1);
    if (tcLon2 != NULL && tcLat2 != NULL)
      toWgs84(tcLon2, tcLat2);
  }

  csFree(&csErr);
}

//*******************************************************************************
//* Name:  getCoordinates
//* Purpose: Searches for rest of coordinate pairs and converts them to wgs84.
//*******************************************************************************
void getCoordinates(t_data* ptD, size_t sOff) {
  getCordWrapper(&g_rx_c2Coords, ptD, sOff, &g_tE.tcLon, &g_tE.tcLat, NULL, NULL);
}

/*******************************************************************************
 * Name:  getData
 * Purpose: Gets raw bytes and converts them to readable data.
 *******************************************************************************/
int getData(t_rx_matcher* rxM, t_data* ptD) {
  size_t sPos = rxM->dasStart.pVal[0];

  // Convert matched bytes.
  g_tE.iType        = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[2]], 1);
  g_tE.tcLon.ldlVal = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[3]], 4);
  g_tE.tcLat.ldlVal = toInt((char*) &ptD->pBytes[rxM->dasStart.pVal[4]], 4);

  // Quick error check.
  if (g_tE.iType == 0) return 0;

  // Convert and error check.
  if (! toWgs84(&g_tE.tcLon, &g_tE.tcLat)) return 0;

  // Get rest of labels and coordinates.
  getLables(ptD, sPos);
  getCoordinates(ptD, sPos);

  return 1;
}

/*******************************************************************************
 * Name:  printEntry
 * Purpose: Prints generic csv file entry.
 *******************************************************************************/
void printEntry(int iOffset) {
  printf("<Remark>\t<Longitude>\t<Latitude>\t<Label>");
  if (g_tOpts.iPrtOff) printf("\t%d", iOffset);
  printf("\n");
}

/*******************************************************************************
 * Name:  doRegex
 * Purpose: Takes string, regex and flags to perform a Perl compatible search.
 *******************************************************************************/
void doRegex(const char* pcToSearch, const char* pcRegex, const char* pcFlags) {
  t_rx_matcher rxMatcher = {0};
  cstr         csErr     = csNew("");
  int          iErr      = 0;

  printf("\nMatch: '%s'\n", pcToSearch);
  printf("With:  '%s'\n",   pcRegex);
  printf("Flags: '%s'\n",   pcFlags);

  // Compile regex and init global matcher struct.
  if (rxInitMatcher(&rxMatcher, pcRegex, pcFlags, &csErr) != RX_NO_ERROR) {
    printf("%s\n", csErr.cStr);
    goto free_and_exit;
  }

  // rxMatch() crams all sub-matches into an intenal cstr array and signals, if
  // matching reached end of string.
  printf("Start offset = %lu\n", rxMatcher.sPos);
  while (rxMatch(&rxMatcher, RX_KEEP_POS, pcToSearch, RX_LEN_MAX, &iErr, &csErr)) {
    for (int i = 0; i < rxMatcher.dacsMatch.sCount; ++i)
      printf("$%d = '%s'\n", i, rxMatcher.dacsMatch.pVal[i].cStr);
    printf("Next offset = %lu\n", rxMatcher.sPos);
  }
  printf("----\n");

  // Free memory of all used structs.
free_and_exit:
  csFree(&csErr);
  rxFreeMatcher(&rxMatcher);
}

/*******************************************************************************
 * Name:  printCsInternals
 *******************************************************************************/
void printCsInternals(cstr* pcsStr) {
  char   cStr[5] = {0}; // To host a max UTF-8 char-string.
  size_t tSize   = 0;

  printf("cstr->len      = %lli\n", pcsStr->len);
  printf("cstr->lenUtf8  = %lli\n", pcsStr->lenUtf8);
  printf("cstr->size     = %lli\n", pcsStr->size);
  printf("cstr->capacity = %lli\n", pcsStr->capacity);
  printf("cstr->cstr     = %s",     pcsStr->cStr);
  if (csIsUtf8(pcsStr->cStr)) printf(" (UTF-8)\n"); else printf(" (ASCII)\n");
  printf("--------------------------------------------------------------------------------\n");
  for(size_t i = 0; i < pcsStr->lenUtf8; ++i) {
    tSize = csAtUtf8(cStr, pcsStr->cStr, i);
    printf("cstr @ [%02lu] = '%s'", i, cStr);
    if (tSize == 1) printf(" (1 byte)\n"); else printf(" (%li bytes)\n", tSize);
  }
  printf("--------------------------------------------------------------------------------\n");
}

/*******************************************************************************
 * Name:  debug
 *******************************************************************************/
void debug(void) {
  cstr csMin      = csNew("");
  cstr csMax      = csNew("");
  cstr csSubRx    = csNew("");
  cstr csRx       = csNew("");
  cstr csTest     = csNew("abcd");
  cstr csDateTime = csNew("");

  printCsInternals(&csTest);

  csTest = csNew("aßcöäüd");
  printCsInternals(&csTest);

  csTest = csNew("ñáè");
  printCsInternals(&csTest);

  printf("\n");

  ticks2datetime(&csMin, " (UTC)", g_tOpts.tTicksMin);
  ticks2datetime(&csMax, " (UTC)", g_tOpts.tTicksMax);

  // Set a proper default value, if nothng was given at cli.
  if (g_tOpts.csDateTime.len == 0) {
    csSet(&g_tOpts.csDateTime, "2019/07/12, 12:00:00");
  }

  // Convert string to ticks and back for testing of both utc time functions.
  g_tOpts.tDateTime = datetime2ticks(1, g_tOpts.csDateTime.cStr, 0, 0, 0, 0, 0, 0);
  ticks2datetime(&csDateTime, " (UTC)", g_tOpts.tDateTime);

  printf("--- String -> Ticks -> String ---\n");
  printf("g_tOpts.csDateTime  = '%s'\n",       g_tOpts.csDateTime.cStr);
  printf("g_tOpts.tDateTime   = %10li\n",      g_tOpts.tDateTime);
  printf("csDateTime          = '%s'\n",       csDateTime.cStr);
  printf("--- String -> Ticks -> String ---\n");
  printf("\n");

  printf("g_tOpts.iTestMode   = %d\n",         g_tOpts.iTestMode);
  printf("g_tOpts.sByteOff    = %ld\n",        g_tOpts.sByteOff);
  printf("g_tOpts.iPrtOff     = %d\n",         g_tOpts.iPrtOff);
  printf("g_tOpts.iOptX       = %d\n",         g_tOpts.iOptX);
  printf("g_tOpts.csOptX.cStr = '%s'\n",       g_tOpts.csOptX.cStr);
  printf("g_tOpts.csRx.cStr   = '%s'\n",       g_tOpts.csRx.cStr);
  printf("g_tOpts.csRxF.cStr  = '%s'\n",       g_tOpts.csRxF.cStr);
  printf("g_tOpts.tTicksMin   = %10li (%s)\n", g_tOpts.tTicksMin, csMin.cStr);
  printf("g_tOpts.tTicksMax   = %10li (%s)\n", g_tOpts.tTicksMax, csMax.cStr);
  printf("\n");

  printf("Free argument's dynamic array: ");
  for (int i = 0; i < g_tArgs.sCount - 1; ++i)
    printf("%s, ", g_tArgs.pVal[i].cStr);
  printf("%s\n", g_tArgs.pVal[g_tArgs.sCount - 1].cStr);

  // How to assemble regex via cstr variables.
  // "([0-9a-fA-F]{2}):([0-9a-fA-F]{2}):([0-9a-fA-F]{2}):([0-9a-fA-F]{2}):([0-9a-fA-F]{2})"
  csSet(&csSubRx, "([0-9A-F]{2})");
  csSetf(&csRx, "%s : %s : %s : %s : %s", csSubRx.cStr, csSubRx.cStr, csSubRx.cStr, csSubRx.cStr, csSubRx.cStr);
  doRegex("0f:aa:08:7e:50", csRx.cStr, "xi");

  doRegex(g_tOpts.csOptX.cStr, g_tOpts.csRx.cStr, g_tOpts.csRxF.cStr);

  csFree(&csMin);
  csFree(&csMax);
  csFree(&csSubRx);
  csFree(&csRx);
  csFree(&csTest);
  csFree(&csDateTime);
}

/*******************************************************************************
 * Name:  readBytes2ByteArray
 * Purpose: Reads count bytes at offset into the dynamic byte array.
 *******************************************************************************/
int readBytes2ByteArray(t_data* ptData, size_t sOff, size_t sCount, FILE* hFile) {
  // Set file to offset befor reading from it!
  fseek(hFile, sOff, SEEK_SET);

  // Allocate memory only once.
  if (ptData->pBytes == NULL) ptData->pBytes = (uchar*) malloc(sCount);

  // Now read all
  ptData->sSize = fread(ptData->pBytes, sizeof(char), sCount, hFile);

  // No bytes is the end of file.
  if (ptData->sSize == 0) {
    free(ptData->pBytes);
    return 0;
  }

  return 1;
}

/*******************************************************************************
 * Name:  getNextDataChunk
 * Purpose: Wrapper function for readBytes2ByteArray().
 *******************************************************************************/
int getNextDataChunk(t_data* ptData, size_t sChunkSize, size_t sChunk, size_t sTwice, FILE* hFile) {
  return readBytes2ByteArray(ptData, sChunk * sChunkSize, sChunkSize + sTwice, hFile);
}

/*******************************************************************************
 * Name:  printProgress
 * Purpose: Prints progress of search.
 *******************************************************************************/
void printProgress(const char* cFile, size_t sFileSize, size_t sOff) {
  ldbl ldPercent = (ldbl) sOff / (ldbl) sFileSize * 100;
  fprintf(stderr, "\rLast match in '%s' at %li (%.2Lf %%) of %li Bytes   ",
          cFile, sOff, ldPercent, sFileSize);
}


//******************************************************************************
//* main

int main(int argc, char *argv[]) {
  FILE*  hFile     = NULL;
  size_t sOff      = 0;
  size_t sChunk    = 0;
  size_t sFileSize = 0;

  // 1 GiB chunks with 1 KiB overlap.
  t_data tData      = {0};
  size_t sChunkSize = 1024 * 1024 * 1024;
  size_t sTwice     = 1024;

  // Regex helper vars.
  cstr csErr = csNew("");
  int  iErr  = 0;

  // Save program's name.
  getMename(&g_csMename, argv[0]);

  // Get options and dispatch errors, if any.
  getOptions(argc, argv);

  // Initialize everything.
  initGlobalVars();
  initGlobalRegexes();
  initTimeFunctions();

  // Delete, when debugging is finished.
  debug(); goto free_and_exit;
  // Delete, when debugging is finished.

  printHeader();

  // 'tData' will be initialized and destroyed in 'getNextDataChunk()'.

  // Get all data from all files.
  for (int i = 0; i < g_tArgs.sCount; ++i) {
    hFile     = openFile(g_tArgs.pVal[i].cStr, "rb");
    sFileSize = getFileSize(hFile);
//-- file ----------------------------------------------------------------------
    while (!feof(hFile)) {
      sChunk       = 0;
      tData.pBytes = NULL;
      tData.sSize  = 0;
      while (getNextDataChunk(&tData, sChunkSize, sChunk, sTwice, hFile)) {
        g_rx_c7TomTomLive.sPos = 0; // Reset start of regex for each chunk.
        while (rxMatch(&g_rx_c7TomTomLive, RX_KEEP_POS, (char*) tData.pBytes, tData.sSize, &iErr, &csErr)) {
          // Get global offset.
          sOff = sChunk * sChunkSize + g_rx_c7TomTomLive.dasStart.pVal[0];

          if (g_tOpts.iPrtPrgrs) printProgress(g_tArgs.pVal[i].cStr, sFileSize, sOff);

          if (! getData(&g_rx_c7TomTomLive, &tData)) continue;
          printEntry(sOff);
        }
        ++sChunk;
      }
    }
//-- file ----------------------------------------------------------------------
    if (g_tOpts.iPrtPrgrs) fprintf(stderr, "\n");
    fclose(hFile);
  }

free_and_exit:
  // Free all used memory, prior end of program.
  daFreeEx(g_tArgs, cStr);
  csFree(&csErr);
  csFree(&g_tOpts.csOptX);
  csFree(&g_tOpts.csRx);
  csFree(&g_tOpts.csRxF);
  csFree(&g_tOpts.csDateTime);
  csFree(&g_csMename);
  freeRxStructs();

  return ERR_NOERR;
}

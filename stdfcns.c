/*******************************************************************************
 ** Name: stdfcns.c
 ** Purpose:  Keeps standard functions in one place for better maintenance.
 ** Author: (JE) Jens Elstner
 ** Version: v0.5.2
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 30.11.2019  JE    Created file.
 ** 17.01.2020  JE    Added necessary includes to run with nodiff.
 ** 10.03.2020  JE    Added 'stdint.h' to use C99 compatible uint32_t type.
 ** 11.03.2020  JE    Added invInt(), isDigit() and checkDateTime();
 ** 12.04.2020  JE    Added getArg*() function family.
 ** 12.04.2020  JE    Deleted boolean constants.
 *******************************************************************************/


//******************************************************************************
//* includes => see 'main.c'!

#define _XOPEN_SOURCE 700 // To get POSIX 2008 (SUS) strptime() and mktime()
#include <time.h>
#include <endian.h>       // To get __LITTLE_ENDIAN
#include <stdint.h>       // For uint8_t, etc. typedefs


//******************************************************************************
//* defines and macros

// isNumber()
#define NUM_NONE  0x00
#define NUM_INT   0x01
#define NUM_FLOAT 0x02

// checkDateTime()
#define DT_NONE  0x00
#define DT_SHORT 0x01
#define DT_LONG  0x02

// getArg*()
#define ARG_VALUE 0x00
#define ARG_CLI   0x01


//******************************************************************************
//* type definition

// For convienience.
typedef unsigned int  uint;
typedef unsigned char uchar;
typedef long double   ldbl;
typedef long long     ll;
typedef long int      li;

// toInt() bytes to int converter.
typedef union u_char2Int{
  char     ac4Bytes[4];
  uint32_t uint32;
} t_char2Int;


//******************************************************************************
//* Functions

/*******************************************************************************
 * Name:  version
 * Purpose: Print version and exit program.
 *******************************************************************************/
void version(void) {
  printf("%s v%s\n", ME_NAME, ME_VERSION);
  exit(ERR_NOERR);
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
  if (sString.cStr[0] == '-') *piSign = -1;
  if (sString.cStr[0] == '+') *piSign =  1;

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
 * Name:  dispatchError
 * Purpose: Needed as a forward declaration for 'openFile()' to work properly!
 *******************************************************************************/
void dispatchError(int rv, const char* pcMsg);

/*******************************************************************************
 * Name:  getArgStr
 * Purpose: Reads a string from cli or a value and returns it.
 *******************************************************************************/
int getArgStr(cstr* pcsRv, int* piArg, int argc, char** argv, int bShift, const char* pcVal) {
  if (bShift == ARG_CLI)   shift(pcsRv, piArg, argc, argv);
  if (bShift == ARG_VALUE) csSet(pcsRv, pcVal);

  if (pcsRv->len == 0) return 0;

  return 1;
}

/*******************************************************************************
 * Name:  getArgHexInt
 * Purpose: Reads an hex integer from cli or a value and returns it.
 *******************************************************************************/
int getArgHexInt(int* piRv, int* piArg, int argc, char** argv, int bShift, const char* pcVal) {
  cstr csRv = csNew("");
  int  iErr = 0;

  if (bShift == ARG_CLI)   shift(&csRv, piArg, argc, argv);
  if (bShift == ARG_VALUE) csSet(&csRv, pcVal);

  if (csRv.len == 0) return 0;

  *piRv = getHexIntParm(csRv, &iErr);
  if (iErr == 1) return 0;

  csFree(&csRv);
  return 1;
}

/*******************************************************************************
 * Name:  getArgInt
 * Purpose: Reads an integer from cli or a value and returns it.
 *******************************************************************************/
int getArgInt(int* piRv, int* piArg, int argc, char** argv, int bShift, const char* pcVal) {
  cstr csRv  = csNew("");
  int  bSign = 0;

  if (bShift == ARG_CLI)   shift(&csRv, piArg, argc, argv);
  if (bShift == ARG_VALUE) csSet(&csRv, pcVal);

  if (csRv.len  == 0)                    return 0;
  if (isNumber(csRv, &bSign) != NUM_INT) return 0;

  *piRv = (int) cstr2ll(csRv);

  csFree(&csRv);
  return 1;
}

/*******************************************************************************
 * Name:  getArgTime
 * Purpose: Reads an time_t from cli or a value and returns it.
 *******************************************************************************/
int getArgTime(time_t* ptRv, int* piArg, int argc, char** argv, int bShift, const char* pcVal) {
  cstr csRv  = csNew("");
  int  bSign = 0;

  if (bShift == ARG_CLI)   shift(&csRv, piArg, argc, argv);
  if (bShift == ARG_VALUE) csSet(&csRv, pcVal);

  if (csRv.len  == 0)                    return 0;
  if (isNumber(csRv, &bSign) != NUM_INT) return 0;

  *ptRv = (time_t) cstr2ll(csRv);

  csFree(&csRv);
  return 1;
}

/*******************************************************************************
 * Name:  openFile
 * Purpose: Opens a file or throws an error.
 *******************************************************************************/
FILE* openFile(const char* pcName, const char* pcFlags) {
  FILE* hFile = NULL;

  if (!(hFile = fopen(pcName, pcFlags))) {
    cstr csMsg = csNew("");
    csSetf(&csMsg, "Can't open '%s'", pcName);
    dispatchError(ERR_FILE, csMsg.cStr);
  }
  return hFile;
}

/*******************************************************************************
 * Name:  getFileSize
 * Purpose: Returns size of file in bytes.
 *******************************************************************************/
li getFileSize(FILE* hFile) {
  li liSize = 0;

  fseek(hFile, 0, SEEK_END);
  liSize = (li) ftell(hFile);
  fseek(hFile, 0, SEEK_SET);

  return liSize;
}

/*******************************************************************************
 * Name:  toInt
 * Purpose: Converts up to 4 bytes to integer.
 *******************************************************************************/
int toInt(char* pc4Bytes, int iCount) {
  t_char2Int tInt = {0};
  for (int i = 0; i < iCount; ++i)
#if __BYTE_ORDER == __LITTLE_ENDIAN
    tInt.ac4Bytes[i] = pc4Bytes[i];
#else
  tInt.ac4Bytes[i] = pc4Bytes[iCount - i - 1];
#endif
  return tInt.uint32;
}

/*******************************************************************************
 * Name:  invInt
 * Purpose: Revers byte order to little-endian.
 *******************************************************************************/
uint32_t invInt(uint32_t uiTicks) {
  t_char2Int uTicks    = {0};
  t_char2Int uInvTicks = {0};

  // Invert bytes in uTicks.
  uTicks.uint32 = uiTicks;
  for (int i = 0; i < 4; ++i) uInvTicks.ac4Bytes[i] = uTicks.ac4Bytes[3 - i];

  return uInvTicks.uint32;
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
 * Name:  isDigit
 * Purpose: Checks if char is a digit.
 *******************************************************************************/
int isDigit(const char cDigit) {
  if (cDigit < '0' || cDigit > '9') return 0;
  return 1;
}

/*******************************************************************************
 * Name:  checkDateTime
 * Purpose: Checks datetime formats 'YYYY/MM/DD' and 'YYYY/MM/DD, hh:mm:ss'.
 *******************************************************************************/
int checkDateTime(cstr* pcsDt) {
  int iRv = DT_NONE;

  // Check short and long version lengths.
  if (pcsDt->len != 10 && pcsDt->len != 20) return iRv;

  //                         1 1   1 1   1 1
  // 0 1 2 3   5 6   8 9     2 3   5 6   8 9
  // Y Y Y Y / M M / D D ,   h h : m m : s s

  // Check digits at the right places.

  // Short version.
  if (isDigit(pcsDt->cStr[0]) && isDigit(pcsDt->cStr[1]) &&
      isDigit(pcsDt->cStr[2]) && isDigit(pcsDt->cStr[3]) &&
      isDigit(pcsDt->cStr[5]) && isDigit(pcsDt->cStr[6]) &&
      isDigit(pcsDt->cStr[8]) && isDigit(pcsDt->cStr[9]))
    iRv = DT_SHORT;

  // Long version.
  if (pcsDt->len == 20)
    if (isDigit(pcsDt->cStr[12]) && isDigit(pcsDt->cStr[13]) &&
        isDigit(pcsDt->cStr[15]) && isDigit(pcsDt->cStr[16]) &&
        isDigit(pcsDt->cStr[18]) && isDigit(pcsDt->cStr[19]))
      iRv = DT_LONG;

    return iRv;
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
 * Purpose: Converts "2017/11/03, 11:14:23" string to ticks.
 *******************************************************************************/
time_t datetime2ticks(int fUseString, const char* pcTime,
                      int iYear, int iMonth, int iDay,
                      int iHour, int iMin,   int iSec) {
  cstr      csItem  = csNew("");
  struct tm sTime   = {0};

  //                   1111111111
  //         01234567890123456789
  // Assume "2017/11/03, 11:14:23"
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

  // UTC should have no daylight saving time!
  sTime.tm_isdst = 0;

  csFree(&csItem);

  // Just tick away ...
  return mktime(&sTime) - timezone;
}

/*******************************************************************************
 * Name:  initTimeFunctions
 * Purpose: Initialise local timezone variables for using 'time.h' finctions.
 *******************************************************************************/
void initTimeFunctions(void) {
  // For timezone var in datetime2ticks().
  tzset();
}

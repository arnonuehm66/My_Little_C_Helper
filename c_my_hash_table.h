/*******************************************************************************
 ** Name: c_my_hash_table.h
 ** Purpose:  Provides a hash table.
 ** Author: (JE) Jens Elstner
 ** Version: v0.1.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 16.08.2026  JE    Created and adapted into a solo header file from
 **                   'https://github.com/jamesroutley/write-a-hash-table'.
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_MY_HASH_TABLE_H
#define C_MY_HASH_TABLE_H


//******************************************************************************
//* includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


//******************************************************************************
//* defines and macros

#define HT_INITIAL_BASE_SIZE 53
#define HT_PRIME_1 151
#define HT_PRIME_2 163


//******************************************************************************
//* type definition

typedef struct {
  char*    cKey;
  uint8_t* u8Val;
  size_t   sValLen;
} t_htItem;

typedef struct {
  size_t     sCount;
  size_t     sBaseSize;
  size_t     sCapacity;
  t_htItem** apItems;
} t_htTable;

typedef struct {
  t_htTable* _phtTable;
  size_t     _sIndex;
} t_htIter;

static t_htItem HT_DELETED_ITEM = {NULL, NULL, 0};


//******************************************************************************
//* function forward declarations
//* For a better function's arrangement.

// Internal functions.
static int        is_prime(size_t x);
static size_t     next_prime(size_t x);
static t_htTable* ht_new_table(size_t sBaseSize);
static t_htItem*  ht_new_item(char* cKey, uint8_t* u8Val, size_t sValLen);
static void       ht_del_item(t_htItem* htItem);
static size_t     ht_hash_key(char* cKey, size_t sPrime, size_t sMax);
static size_t     ht_index_from_key(char* cKey, size_t sBucketsN, size_t sTry);
static int        ht_resize(t_htTable* phtTable, size_t sBaseSize);
static int        ht_resize_up(t_htTable* phtTable);
static int        ht_resize_down(t_htTable* phtTable);

// External functions.

// Init & destroy.
t_htTable* htNewTable(void);
void       htDelTable(t_htTable* phtTable);

// API functions.
void      htSet(t_htTable* phtTable, char* cKey, uint8_t* u8Val, size_t sValLen);
uint8_t*  htGet(t_htTable* phtTable, char* cKey);
void      htDel(t_htTable* phtTable, char* cKey);
t_htIter* htIter(t_htTable* phtTable);
t_htItem* htNext(t_htIter* phtIter);


//******************************************************************************
//* private functions

/*******************************************************************************
 * Name: is_prime
 *******************************************************************************/
static int is_prime(size_t x) {
  if (x < 2)        return -1;  // 1 is undefined.
  if (x < 4)        return  1;  // 3 is prime.
  if ((x % 2) == 0) return  0;  // Even is no prime.

  size_t sMaxSearch = (size_t) floor(sqrt((double) x));

  for (size_t i = 3; i <= sMaxSearch; i += 2) {
    if ((x % i) == 0) return 0;
  }
  return 1;
}

/*******************************************************************************
 * Name: next_prime
 *******************************************************************************/
static size_t next_prime(size_t x) {  // Next prime >= x.
  while (is_prime(x) != 1) ++x;
  return x;
}

/*******************************************************************************
 * Name: ht_new_table
 *******************************************************************************/
static t_htTable* ht_new_table(size_t sBaseSize) {
  t_htTable* phtTable = (t_htTable*) malloc(sizeof(t_htTable));

  phtTable->sBaseSize = sBaseSize;
  phtTable->sCapacity = next_prime(sBaseSize);
  phtTable->sCount    = 0;
  phtTable->apItems   = (t_htItem**) calloc(phtTable->sCapacity, sizeof(t_htItem*));

  return phtTable;
}

/*******************************************************************************
 * Name: ht_new_item
 *******************************************************************************/
static t_htItem* ht_new_item(char* cKey, uint8_t* u8Val, size_t sValLen) {
  t_htItem* phtItem = (t_htItem*) malloc(sizeof(t_htItem));

  phtItem->cKey = strdup(cKey);

  phtItem->u8Val = (uint8_t*) calloc(1, sValLen);
  for (size_t i = 0; i < sValLen; ++i)
    phtItem->u8Val[i] = u8Val[i];
  phtItem->sValLen = sValLen;

  return phtItem;
}

/*******************************************************************************
 * Name: ht_del_item
 *******************************************************************************/
static void ht_del_item(t_htItem* phtItem) {
  free(phtItem->cKey);
  free(phtItem->u8Val);
  free(phtItem);
}

/*******************************************************************************
 * Name: ht_hash_key
 *******************************************************************************/
static size_t ht_hash_key(char* cKey, size_t sPrime, size_t sMax) {
  size_t sHash = 0;
  size_t sLen  = strlen(cKey);

  for (size_t i = 0; i < sLen; ++i) {
    sHash += (size_t) pow(sPrime, sLen - (i + 1)) * cKey[i];
    sHash  = sHash % sMax;
  }

  return sHash;
}

/*******************************************************************************
 * Name: ht_index_from_key
 *******************************************************************************/
static size_t ht_index_from_key(char* cKey, size_t sBucketsN, size_t sTry) {
  size_t hash_a = ht_hash_key(cKey, HT_PRIME_1, sBucketsN);
  size_t hash_b = ht_hash_key(cKey, HT_PRIME_2, sBucketsN);

  return (hash_a + (sTry * (hash_b + 1))) % sBucketsN;
}

/*******************************************************************************
 * Name: ht_resize
 *******************************************************************************/
static int ht_resize(t_htTable* phtTable, size_t sBaseSize) {
  if (sBaseSize < HT_INITIAL_BASE_SIZE) return 0;

  t_htTable* phtTableNew = ht_new_table(sBaseSize);

  for (size_t i = 0; i < phtTable->sCapacity; ++i) {
    t_htItem* item = phtTable->apItems[i];
    if (item != NULL && item != &HT_DELETED_ITEM) {
      htSet(phtTableNew, item->cKey, item->u8Val, item->sValLen);
    }
  }

  phtTable->sBaseSize = phtTableNew->sBaseSize;
  phtTable->sCount    = phtTableNew->sCount;

  // To delete new_ht, we give it ht's size and items
  size_t tmp_size = phtTable->sCapacity;

  phtTable->sCapacity    = phtTableNew->sCapacity;
  phtTableNew->sCapacity = tmp_size;

  t_htItem** tmp_items = phtTable->apItems;

  phtTable->apItems    = phtTableNew->apItems;
  phtTableNew->apItems = tmp_items;

  htDelTable(phtTableNew);

  return 1;
}

/*******************************************************************************
 * Name: ht_resize_up
 *******************************************************************************/
static int ht_resize_up(t_htTable* phtTable) {
  size_t new_size = phtTable->sBaseSize * 2;
  return ht_resize(phtTable, new_size);
}

/*******************************************************************************
 * Name: ht_resize_down
 *******************************************************************************/
static int ht_resize_down(t_htTable* phtTable) {
  size_t new_size = phtTable->sBaseSize / 2;
  return ht_resize(phtTable, new_size);
}


//******************************************************************************
//* public functions


//******************************************************************************
//* Init & destroy functions.

/*******************************************************************************
 * Name: htNewTable
 *******************************************************************************/
t_htTable* htNewTable(void) {
  return ht_new_table(HT_INITIAL_BASE_SIZE);
}

/*******************************************************************************
 * Name: htDelTable
 *******************************************************************************/
void htDelTable(t_htTable* phtTable) {
  for (size_t i = 0; i < phtTable->sCapacity; ++i) {
    t_htItem* phtItem = phtTable->apItems[i];
    if (phtItem != NULL) {
      ht_del_item(phtItem);
    }
  }
  free(phtTable->apItems);
  free(phtTable);
}


//******************************************************************************
//* API functions.

/*******************************************************************************
 * Name: htSet
 * Purpose: Grow table if load is above 70%.
 *******************************************************************************/
void htSet(t_htTable* phtTable, char* cKey, uint8_t* u8Val, size_t sValLen) {
  if ((phtTable->sCount * 100 / phtTable->sCapacity) > 70)
    ht_resize_up(phtTable);

  size_t    sTry       = 0;
  t_htItem* phtItem    = ht_new_item(cKey, u8Val, sValLen);
  size_t    sIndex     = ht_index_from_key(phtItem->cKey, phtTable->sCapacity, sTry);
  t_htItem* phtCurItem = phtTable->apItems[sIndex];

  while (phtCurItem != NULL) {
    if (phtCurItem != &HT_DELETED_ITEM) {
      if (strcmp(phtCurItem->cKey, cKey) == 0) {
        ht_del_item(phtCurItem);
        phtTable->apItems[sIndex] = phtItem;
      }
    }
    sIndex     = ht_index_from_key(phtItem->cKey, phtTable->sCapacity, ++sTry);
    phtCurItem = phtTable->apItems[sIndex];
  }

  phtTable->apItems[sIndex] = phtItem;
  phtTable->sCount++;
}

/*******************************************************************************
 * Name: htGet
 *******************************************************************************/
uint8_t* htGet(t_htTable* phtTable, char* cKey) {
  size_t    sTry    = 0;
  size_t    sIndex  = ht_index_from_key(cKey, phtTable->sCapacity, sTry);
  t_htItem* phtItem = phtTable->apItems[sIndex];

  while (phtItem != NULL) {
    if (phtItem != &HT_DELETED_ITEM) {
      if (strcmp(phtItem->cKey, cKey) == 0) {
        return phtItem->u8Val;
      }
    }
    sIndex  = ht_index_from_key(cKey, phtTable->sCapacity, ++sTry);
    phtItem = phtTable->apItems[sIndex];
  }
  return NULL;
}

/*******************************************************************************
 * Name: htDel
 * Purpose: Shrink table if load is below 10%.
 *******************************************************************************/
void htDel(t_htTable* phtTable, char* cKey) {
  if ((phtTable->sCount * 100 / phtTable->sCapacity) < 10)
    ht_resize_down(phtTable);

  size_t    sTry    = 0;
  size_t    sIndex  = ht_index_from_key(cKey, phtTable->sCapacity, sTry);
  t_htItem* phtItem = phtTable->apItems[sIndex];

  while (phtItem != NULL) {
    if (phtItem != &HT_DELETED_ITEM) {
      if (strcmp(phtItem->cKey, cKey) == 0) {
        ht_del_item(phtItem);
        phtTable->apItems[sIndex] = &HT_DELETED_ITEM;
      }
    }
    sIndex  = ht_index_from_key(cKey, phtTable->sCapacity, ++sTry);
    phtItem = phtTable->apItems[sIndex];
  }
  phtTable->sCount--;
}

/*******************************************************************************
 * Name: htIter
 *******************************************************************************/
t_htIter* htIter(t_htTable* phtTable) {
  t_htIter* phtIter = {0};

  phtIter->_phtTable = phtTable;

  return phtIter;
}

/*******************************************************************************
 * Name: htNext
 *******************************************************************************/
t_htItem* htNext(t_htIter* phtIter) {
  t_htTable* phtTable = phtIter->_phtTable;
  size_t     sIndex   = phtIter->_sIndex;

  while ((sIndex < phtTable->sCapacity) &&
         (phtTable->apItems[sIndex] == NULL ||
          phtTable->apItems[sIndex] == &HT_DELETED_ITEM)) {
    ++sIndex;
  }

  if (sIndex >= phtTable->sCapacity) return NULL;
  phtIter->_sIndex = sIndex;

  return phtTable->apItems[sIndex];
}


#endif // C_MY_HASH_TABLE_H

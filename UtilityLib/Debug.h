/*
 * Debug.h : some datatypes for unit-testing the library
 */

#ifndef	DEBUG_SWL_H
#define	DEBUG_SWL_H

typedef struct
{
	ListNode node;
	int      value;

} ListItem;

static int CheckList(ListHead * head);
static int CompareItem(ListNode * nodeA, ListNode * nodeB);
static int CheckFileList(STRPTR dir, STRPTR file, APTR ud);
static uint32_t crc32(unsigned char * block, int length);
static void ThreadFunc(void *);
static int HashScanFree(APTR k, APTR v, APTR d);
static int HashScanCount(APTR k, APTR v, APTR d);
static void CheckRes(STRPTR ident, Variant, int, APTR);

#endif

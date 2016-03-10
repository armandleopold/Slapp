/*
 * List.c : double linked list management.
 *
 * Written by T.Pierron, feb 14, 2006.
 */

#include "SWL.h"
#include <string.h>

/**h* SWL/List
 * INTRODUCTION
 *	This little module provides very common functions for handling double
 *	linked list. Things that every C programmer has to {re}^826578-write
 *	because it hopelessly lack in the libc.
 *
 *	The list is managed through 2 datatypes : one header which gather general
 *	info on the list (head, tail, number of items) and a list node which
 *	all objects inserted must have (it is a simple prev/next record).
 *
 *	For this very simple module, direct access to datatypes is allowed, since
 *	functions might just produce overkill code, especially looping through
 *	items of the list. Public fields for the list header are :
 *
 *		struct ListHead_t
 *		{
 *			ListNode * lh_Head;
 *			ListNode * lh_Tail;
 *			ULONG      lh_Count;
 *		};
 *		typedef struct ListHead_t     ListHead;
 *
 *	This datatype is intended to be declared as value, not pointer. To
 *	initialize it, just memset() it with 0 or use ListNew() macro.
 *
 *	The other datatype has to be declared in each item you may want to insert
 *	in the list. Again, you have to declare it as value, not as pointer :
 *
 *		struct ListNode_t
 *		{
 *			struct ListNode_t * ln_Next;
 *			struct ListNode_t * ln_Prev;
 *		};
 *		typedef struct ListNode_t    ListNode;
 */


/**f* List/ListNew
 * NAME
 *	ListNew - init a ListHead struct
 *
 * SYNOPSIS
 *	void ListNew(ListHead *);
 *
 * FUNCTION
 *	Simple macro that zeros all fields.
 */

/**f* List/ListAddHead, List/ListAddTail
 * NAME
 *	ListAddHead - Add an item at the start of a list.
 *	ListAddTail - Add an item at the end of a list.
 *
 * SYNOPSIS
 *	void ListAddHead(ListHead * head, ListNode * item);
 *	void ListAddTail(ListHead * head, ListNode * item);
 *
 * FUNCTION
 *	ListAddHead() simply adds the specified item to the beginning of the
 *	list. This is equivalent to ListInsert(head, NULL, item);, althrough
 *	it is a bit more optimized.
 *
 *	ListAddTail() adds the specified item to the end of the list.
 *	This is equivalent to ListInsert(head, head->lh_Tail, item);,
 *	though it is a bit more optimized and better reflect intended
 *	action.
 *
 * SEE ALSO
 *	ListInsert
 */
DLLIMP void ListAddHead(ListHead * head, ListNode * item)
{
	ListNode * first = head->lh_Head;

	if (first) first->ln_Prev = item;
	else       head->lh_Tail  = item;
	head->lh_Count ++;
	head->lh_Head = item;
	item->ln_Next = first;
	item->ln_Prev = NULL;
}

DLLIMP void ListAddTail(ListHead * head, ListNode * item)
{
	ListNode * last = head->lh_Tail;

	if (last) last->ln_Next = item;
	else      head->lh_Head = item;
	head->lh_Count ++;
	head->lh_Tail = item;
	item->ln_Prev = last;
	item->ln_Next = NULL;
}


/**f* List/ListInsert
 * NAME
 *	ListInsert - Insert an item anywhere in the list.
 *
 * SYNOPSIS
 *	void ListInsert(ListHead * head, ListNode * item, ListNode * ins_after);
 *
 * FUNCTION
 *	This function may let you insert a node anywhere in the list. NULL for
 *	ins_after parameter is synonym for beginning of list.
 *
 * INPUT
 *	head      - header of list, must have been properly initialized.
 *	item      - item to insert, must not be NULL.
 *	ins_after - 'item' will be inserted after this parameter, or at
 *	            beginning if set to NULL.
 *
 * SEE ALSO
 *	ListInsertSort, ListRemove, ListAddHead
 */
DLLIMP void ListInsert(ListHead * head, ListNode * item, ListNode * insert_after)
{
	ListNode * next;
	if (insert_after == NULL)
	{
		/* Add Head */
		next = head->lh_Head;
		head->lh_Head = item;
	}
	else /* Add Tail ~ Middle */
	{
		next = insert_after->ln_Next;
		insert_after->ln_Next = item;
	}
	if (next) next->ln_Prev = item;
	else      head->lh_Tail = item;
	item->ln_Next = next;
	item->ln_Prev = insert_after;
	head->lh_Count ++;
}


/**f* List/ListInsertSort, List/ListSort
 * NAME
 *	ListInsertSort - Insert an item anywhere, while keeping list sorted.
 *	ListSort - sort an unordered list.
 *
 * SYNOPSIS
 *	void ListInsertSort(ListHead * head, ListNode * item, ListSortFunc cb);
 *	void ListSort(ListHead * head, ListSortFunc cb);
 *
 * FUNCTION
 *	The ListInsertSort() function allows you to build a list with an order
 *	relation set on eqch items. Do not use this function on a list that is
 *	not already sorted, otherwise result may not be relevant.
 *
 *	ListSort() reorder all item to be sorted using user defined sorting
 *	callback. Sort algorithm is a simple sort insert from a emptied list.
 *
 * INPUT
 *	head - header of list, must have been properly initialized.
 *	item - item to insert, must not be NULL.
 *	cb   - user callback that set order relation for each item. Prototype
 *	       must be :
 *
 *	       int SortFunc(ListNode * itemA, ListNode * itemB);
 *
 *	       It should return a negative value if itemA < itemB, positive
 *	       if itemA > itemB or 0 if they are equal.
 *
 * SEE ALSO
 *	ListRemove, ListInsert
 */
DLLIMP void ListInsertSort(ListHead * head, ListNode * item, ListSortFunc cb)
{
	ListNode * ins;
	ListNode * prev;

	for (prev = NULL, ins = head->lh_Head; ins && cb(ins, item) < 0;
	     prev = ins,  ins = ins->ln_Next);

	ListInsert(head, item, prev);
}

DLLIMP void ListSort(ListHead * head, ListSortFunc cb)
{
	ListNode * node;
	ListNode * next;
	ListHead   hdr;

	memset(&hdr, 0, sizeof hdr);

	for (node = head->lh_Head; node; node = next)
	{
		next = node->ln_Next;
		ListInsertSort(&hdr, node, cb);
	}
	memcpy(head, &hdr, sizeof hdr);
}

/**f* List/ListRemove
 * NAME
 *	ListRemove - Remove an item from the list
 *
 * SYNOPSIS
 *	void ListRemove(ListHead * head, ListNode * item);
 *
 * FUNCTION
 *	This function removes a node from the list. Just be sure that the item
 *	really belongs to the list, otherwise you may have invalidate another
 *	list header.
 *
 * SEE ALSO
 *	ListInsert, ListRemHead, ListAddHead
 */
DLLIMP void ListRemove(ListHead * head, ListNode * item)
{
	ListNode * node;

	node = item->ln_Prev;
	if (node) node->ln_Next = item->ln_Next;
	else      head->lh_Head = item->ln_Next;

	node = item->ln_Next;
	if (node) node->ln_Prev = item->ln_Prev;
	else      head->lh_Tail = item->ln_Prev;

	head->lh_Count --;
}

/**f* List/ListRemHead, List/ListRemTail
 * NAME
 *	ListRemHead - Remove first item of the list.
 *	ListRemTail - Remove last item.
 *
 * SYNOPSIS
 *	ListNode * ListRemHead(ListHead * head);
 *	ListNode * ListRemTail(ListHead * head);
 *
 * FUNCTION
 *	These functions are the counter part of ListAddHead() and ListAddTail().
 *	They remove an item from the list, respectively at the beginning or at
 *	the end.
 *
 * RESULT
 *	The node which has been removed or NULL if there was no item left
 *	in the list.
 *
 * SEE ALSO
 *	ListRemove, ListInsert, ListAddHead
 */
DLLIMP ListNode * ListRemHead(ListHead * head)
{
	ListNode * node = head->lh_Head;

	if (node)
	{
		ListNode * next = node->ln_Next;
		head->lh_Head = next;
		if (next) next->ln_Prev = NULL;
		else      head->lh_Tail = NULL;
		head->lh_Count --;
	}
	return node;
}

DLLIMP ListNode * ListRemTail(ListHead * head)
{
	ListNode * node = head->lh_Tail;

	if (node)
	{
		ListNode * prev = node->ln_Prev;
		head->lh_Tail = prev;
		if (prev) prev->ln_Next = NULL;
		else      head->lh_Head = NULL;
		head->lh_Count --;
	}
	return node;
}

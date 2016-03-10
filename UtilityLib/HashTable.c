/*
 * HashTable.c : hash table with auto-resize feature.
 *
 * Written by Christopher Clark <firstname.lastname@cl.cam.ac.uk>, reformatted by
 * T.Pierron.
 */

#include "SWL.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Credit for primes table: Aaron Krowne
 * http://br.endernet.org/~akrowne/
 * http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 */
static ULONG primes[] = {
	53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
	50331653, 100663319, 201326611, 402653189, 805306457, 1610612741
};

#define	prime_table_length     sizeof(primes)/sizeof(primes[0])
#define	max_load_factor        0.65
#define	indexFor(len, value)   (value % len)
#define	freekey(X)

/**h* SWL/Hash
 * INTRODUCTION
 *	Basic usage is as follow:
 *
 *		HashTable h;
 *		HashKey   k;
 *		HashValue v;
 *		HashValue found;
 *
 *		static ULONG hash_from_key_fn(APTR k);
 *		static int   keys_equal_fn(APTR key1, APTR key2);
 *
 *		h = HashCreate(16, hash_from_key_fn, keys_equal_fn);
 *		k = malloc(sizeof *k);
 *		v = malloc(sizeof *v);
 *
 *		// (initialise k and v to suitable values)
 *
 *		if (! HashInsert(h, k, v))
 *			exit(-1);
 *
 *		if (NULL == (found = HashSearch(h, k)))
 *			printf("Not found !\n");
 *
 *		// You'll have to manage memory of your keys and values yourself
 *		if (NULL == (found = HashRemove(h, k)))
 *			printf("Not found !\n");
 *
 *		// Keys and values won't be freed - use HashIterInit for that
 *		HashDestroy(h);
 */

/**t* Hash/HashCb, Hash/EqualCb
 * SYNOPSIS
 *	typedef ULONG (*HashCb)(APTR k);
 *	typedef int   (*EqualCb)(APTR k1, APTR k2);
 *
 * SEE ALSO
 *	HashCreate, HashStr, HashStrCmp
 */

/**f* Hash/HashCreate
 * NAME
 *	HashCreate - Create a hash table
 *
 * SYNOPSIS
 *	HashTable HashCreate(ULONG minsize, HashCb hcb, EqualCb, ecb);
 *
 * FUNCTION
 *	Create the initial structure for a hash table.
 *
 * INPUT
 *	- minsize : minimum initial size of hashtable
 *	- hcb     : function for hashing keys. Prototype expected:
 *	                ULONG hcb(APTR key);
 *	- ecb     : function for determining key equality (return !0 if equals).
 *	                int ecb(APTR key1, APTR key2);
 *
 * RESULT
 *	newly created hashtable or NULL on failure
 *
 * SEE ALSO
 *	HashInsert, HashSearch, HashIterInit, HashDestroy
 */
DLLIMP HashTable HashCreate(ULONG minsize, HashCb hashf, EqualCb eqf)
{
	HashTable h;
	ULONG     pindex;
	ULONG     size = primes[0];

	/* Check requested hashtable isn't too large */
	if (minsize > (1u << 30)) return NULL;

	/* Enforce size as prime */
	for (pindex=0; pindex < prime_table_length; pindex++) {
		if (primes[pindex] > minsize) {
			size = primes[pindex];
			break;
		}
	}

	h = malloc(sizeof *h);

	if (h)
	{
		h->table = calloc(sizeof (Entry), size);

		if (h->table)
		{
			h->tablelength = size;
			h->primeindex  = pindex;
			h->entrycount  = 0;
			h->hashfn      = hashf;
			h->eqfn        = eqf;
			h->loadlimit   = size * max_load_factor + 0.5;
		}
		else free(h), h = NULL;
	}
	return h;
}

static ULONG hash(HashTable h, APTR k)
{
	/* Aim to protect against poor hash functions by adding logic here
	 * - logic taken from java 1.4 hashtable source */
	ULONG i = h->hashfn(k);
	i += ~(i << 9);
	i ^=  ((i >> 14) | (i << 18)); /* >>> */
	i +=  (i << 4);
	i ^=  ((i >> 10) | (i << 22)); /* >>> */
	return i;
}

static int HashExpand(HashTable h)
{
	/* Double the size of the table to accomodate more entries */
	Entry * newtable;
	Entry * pE;
	Entry   e;
	ULONG   newsize, i, index;

	/* Check we're not hitting max capacity */
	if (h->primeindex == (prime_table_length - 1)) return 0;
	newsize = primes[++ h->primeindex];

	newtable = malloc(sizeof e * newsize);

	if (newtable)
	{
		memset(newtable, 0, newsize * sizeof e);

		/* This algorithm is not 'stable'. ie. it reverses the list
		 * when it transfers entries between the tables */
		for (i = 0; i < h->tablelength; i++)
		{
			while (NULL != (e = h->table[i]))
			{
				h->table[i] = e->next;
				index = indexFor(newsize, e->h);
				e->next = newtable[index];
				newtable[index] = e;
			}
		}
		free(h->table);
		h->table = newtable;
	}
	else /* Plan B: realloc instead */
	{
		newtable = realloc(h->table, newsize * sizeof e);

		if (NULL == newtable) {
			h->primeindex --;
			return 0;
		}
		h->table = newtable;
		memset(newtable + h->tablelength, 0, newsize - h->tablelength);
		for (i = 0; i < h->tablelength; i++)
		{
			for (pE = newtable + i, e = *pE; e != NULL; e = *pE)
			{
				index = indexFor(newsize, e->h);
				if (index != i)
				{
					*pE = e->next;
					e->next = newtable[index];
					newtable[index] = e;
				}
				else pE = &e->next;
			}
		}
	}
	h->tablelength = newsize;
	h->loadlimit   = newsize * max_load_factor + 0.5;
	return -1;
}

/**f* Hash/HashCount
 * NAME
 *	HashCount - Returns number of item in the hash table
 *
 * SYNOPSIS
 *	ULONG HashCount(HashTable h);
 */
DLLIMP ULONG HashCount(HashTable h)
{
	return h ? h->entrycount : 0;
}

/**f* Hash/HashInsert
 * NAME
 *	HashInsert - Insert an item into the table
 *
 * SYNOPSIS
 *	int HashInsert(HashTable h, APTR k, APTR v);
 *
 * FUNCTION
 *	This function will cause the table to expand if the insertion would take
 *	the ratio of entries to table size over the maximum load factor.
 *
 *	If key already exists, it will just replace current value.
 *
 * INPUT
 *	- h : the hashtable to insert into
 *	- k : the key - hashtable claims ownership and will free on removal
 *	- v : the value - does not claim ownership
 *
 * RESULT
 *	non-zero for successful insertion
 *
 * SEE ALSO
 *	HashSearch, HashRemove
 */
DLLIMP int HashInsert(HashTable h, APTR k, APTR v)
{
	/* This method allows duplicate keys - but they shouldn't be used */
	ULONG index;
	ULONG hashv;
	Entry e;

	if (h->entrycount > h->loadlimit)
	{
		/* Ignore the return value. If expand fails, we should
		 * still try cramming just this value into the existing table
		 * -- we may not have memory for a larger table, but one more
		 * element may be ok. Next time we insert, we'll try expanding again.
		 */
		HashExpand(h);
	}
	hashv = hash(h, k);
	index = indexFor(h->tablelength, hashv);

	/* Check if key already exists */
	for (e = h->table[index]; e; e = e->next)
	{
		/* Check hash value to short circuit heavier comparison */
		if (hashv == e->h && h->eqfn(k, e->k))
		{
			e->v = v;
			return -1;
		}
	}

	e = malloc(sizeof *e);
	if (e)
	{
		e->h = hashv;
		e->k = k;
		e->v = v;
		e->next = h->table[index];
		h->table[index] = e;
		h->entrycount ++;
		return -1;
	}
	return 0;
}

/**f* Hash/HashSearch
 * NAME
 *	HashSearch - Search for a value associated to a key
 *
 * SYNOPSIS
 *	APTR HashSearch(HashTable h, APTR k);
 *
 * INPUT
 * - h : the hashtable to search
 * - k : the key to search for  - does not claim ownership
 *
 * RESULT
 *	The value associated with the key, or NULL if none found.
 *
 * SEE ALSO
 *	HashInsert
 */
DLLIMP APTR HashSearch(HashTable h, APTR k)
{
	Entry e;
	ULONG hashv, index;

	if (h == NULL) return NULL;

	hashv = hash(h, k);
	index = indexFor(h->tablelength, hashv);

	for (e = h->table[index]; e; e = e->next)
	{
		/* Check hash value to short circuit heavier comparison */
		if (hashv == e->h && h->eqfn(k, e->k))
			return e->v;
	}
	return NULL;
}

/**f* Hash/HashRemove
 * NAME
 *	HashRemove - Remove an item in the hash table
 *
 * SYNOPSIS
 *	APTR HashRemove(HashTable h, APTR k);
 *
 * INPUT
 *	- h : the hashtable to remove the item from
 *	- k : the key to search for  - does not claim ownership
 *
 * RESULT
 *	the value associated with the key, or NULL if none found
 *
 * SEE ALSO
 *	HashInsert
 */
DLLIMP APTR HashRemove(HashTable h, APTR k)
{
	/* TODO: consider compacting the table when the load factor drops enough,
	 *       or provide a 'compact' method. */
	Entry   e;
	Entry * pE;
	APTR    v;
	ULONG   hashvalue, index;

	hashvalue = hash(h,k);
	index = indexFor(h->tablelength,hash(h,k));
	pE = h->table + index;
	e = *pE;

	while (e)
	{
		/* Check hash value to short circuit heavier comparison */
		if ((hashvalue == e->h) && (h->eqfn(k, e->k)))
		{
			*pE = e->next;
			h->entrycount--;
			v = e->v;
			free(e);
			return v;
		}
		pE = &(e->next);
		e = e->next;
	}
	return NULL;
}

/**f* Hash/HashDestroy
 * NAME
 *	HashDestroy - Destroy all memory allocated for hash table.
 *
 * SYNOPSIS
 *	void HashDestroy(HashTable h, int free_values);
 *
 * INPUT
 *	- h : the hashtable
 *	- free_values : whether to call 'free' on the remaining values
 *
 * SEE ALSO
 *	HashCreate
 */
DLLIMP void HashDestroy(HashTable h, int free_values)
{
	ULONG   i;
	Entry   e, f;
	Entry * table;

	if (h == NULL) return;

	for (i = 0, table = h->table; i < h->tablelength; i++)
	{
		e = table[i];
		while (e)
		{
			f = e;
			e = e->next;
			if (free_values) free(f->v);
			free(f);
		}
	}
	free(h->table);
	free(h);
}


/**f* Hash/HashIterInit
 * NAME
 *	HashIterInit - Initialize a object to scan values of table.
 *
 * SYNOPSIS
 *	void HashIterInit(HashTable h, HashTableItr);
 *
 * FUNCTION
 *	This function enable you to scan every entries of the hash table. The only
 *	operation allowed while doing this is removing an item. This has to be done
 *	using the special function HashIterRem(), NOT using HashRemove().
 *
 * SEE ALSO
 *	HashIterNext, HashScan, HashIterRem
 */
DLLIMP void HashIterInit(HashTable h, HashTableItr itr)
{
	ULONG i, tablelength;

	itr->h = h;
	itr->e = NULL;
	itr->parent = NULL;
	tablelength = h->tablelength;
	itr->index = tablelength;

	if (0 == h->entrycount) return;

	for (i = 0; i < tablelength; i++)
	{
		if (NULL != h->table[i])
		{
			itr->e = h->table[i];
			itr->index = i;
			break;
		}
	}
}

/**f* Hash/HashIterNext
 * NAME
 *	HashIterNext - advance the iterator to the next element.
 *
 * SYNOPSIS
 *	int HashIterNext(HashTableItr itr);
 *
 * RESULT
 *	Returns zero if reached end of table
 *
 * SEE ALSO
 *	HashIterRem, HashIterInit
 */
DLLIMP int HashIterNext(HashTableItr itr)
{
	ULONG   j,tablelength;
	Entry * table;
	Entry   next;

	if (NULL == itr->e) return 0; /* stupidity check */

	next = itr->e->next;
	if (next)
	{
		itr->parent = itr->e;
		itr->e = next;
		return -1;
	}
	tablelength = itr->h->tablelength;
	itr->parent = NULL;
	if (tablelength <= (j = ++(itr->index)))
	{
		itr->e = NULL;
		return 0;
	}
	table = itr->h->table;

	while (NULL == (next = table[j]))
	{
		if (++j >= tablelength)
		{
			itr->index = tablelength;
			itr->e = NULL;
			return 0;
		}
	}
	itr->index = j;
	itr->e = next;
	return -1;
}

/**f* Hash/HashIterRem
 * NAME
 *	HashIterRem - remove current element and advance the iterator to the next element.
 *
 * SYNOPSIS
 *	int HashIterRem(HashTableItr);
 *
 * FUNCTION
 *	Remove the current item pointed by iterator and move to next item. If you
 *  need the value to free it, read it before removing (beware memory leaks!).
 *
 * RESULT
 *	0 if reached the end of table.
 *
 * SEE ALSO
 *	HashIterNext
 */
DLLIMP int HashIterRem(HashTableItr itr)
{
	Entry remember_e, remember_parent;
	int   ret;

	/* Do the removal */
	if (NULL == itr->parent)
		/* element is head of a chain */
		itr->h->table[itr->index] = itr->e->next;
	else
		/* element is mid-chain */
		itr->parent->next = itr->e->next;

	/* itr->e is now outside the hashtable */
	remember_e = itr->e;
	itr->h->entrycount--;

	/* Advance the iterator, correcting the parent */
	remember_parent = itr->parent;
	ret = HashIterNext(itr);
	if (itr->parent == remember_e)
		itr->parent = remember_parent;
	free(remember_e);
	return ret;
}

/**f* Hash/HashScan
 * NAME
 *	HashScan - walk through items of table using callback.
 *
 * SYNOPSIS
 *	int HashScan(HashTable h, ScanCb cb, APTR data);
 *
 * FUNCTION
 *	Scan each item of the table using a callback instead of using a loop. You
 *	can remove the current item by returning a special code in the callback.
 *
 * INPUT
 *	- h    : table to scan.
 *	- cb   : function called on each item of the table. Prototype must be :
 *
 *	             int cb(APTR key, APTR value, APTR data);
 *
 *	         where (key, value) is the current key/value pair found and data is
 *	         the data parameter passed to HashScan.
 *
 *	         Possible return values for callback are :
 *	         - 0 : continue scan.
 *	         - 1 : remove current item from table and continue.
 *	         - 2 : stop scan now.
 *	         - 3 : remove and stop now.
 *
 *	- data : custom value that will be transmitted to your callback.
 *
 * SEE ALSO
 *	HashIterInit
 */
DLLIMP void HashScan(HashTable h, ScanCb cb, APTR data)
{
	HashIterator iter;

	if (h->entrycount > 0)
	{
		int next;
		HashIterInit(h, &iter);
		do {
			int ret = cb(HashIterKey(&iter), HashIterValue(&iter), data);

			next = (ret & 1 ? HashIterRem(&iter) : HashIterNext(&iter)) && ! (ret & 2);
		}
		while (next);
	}
}

/**f* Hash/HashStr, Hash/HashStrCmp, Hash/HashStrCaseCmp
 * NAME
 *	HashStr - Compute hash from a NULL-terminated string
 *	HashStrCmp - Compare case-sensitively string hash keys.
 *	HashStrCaseCmp - Compare case-insensitively string hash keys.
 *
 * SYNOPSIS
 *	ULONG HashStr(APTR k);
 *	int HashStrCmp(APTR k1, APTR k2);
 *	int HashStrCaseCmp(APTR k1, APTR k2);
 *
 * FUNCTION
 *	Those functions can be supplied to HashCreate as callback, if your keys are
 *	strings (which will certainly be often the case).
 *
 * SEE ALSO
 *	HashCreate
 */
DLLIMP ULONG HashStr(APTR k)
{
	ULONG  hash = 0;
	STRPTR p;

	for (p = k; *p; hash <<= 4, hash |= *p, p ++);

	return hash;
}

DLLIMP int HashStrCmp(APTR k1, APTR k2)
{
	return strcmp(k1, k2) == 0;
}

DLLIMP int HashStrCaseCmp(APTR k1, APTR k2)
{
	return strcasecmp(k1, k2) == 0;
}

#if	0
void HashDebug(HashTable h)
{
	int i;

	#ifdef	DEBUG
	#define	LOG_Info(fmt, ...)   fprintf(stderr, fmt,  ## __VA_ARGS__)
	#endif

	LOG_Info("count = %ld, limit = %ld, prime = %ld\n", h->entrycount, h->loadlimit, h->primeindex);
	LOG_Info("items:\n");

	for (i = 0; i < h->tablelength; i ++)
	{
		Entry e, p;
		if (h->table[i] == NULL) continue;

		for (p = NULL, e = h->table[i]; e; p = e, e = e->next)
		{
			TEXT buffer[10];
			sprintf(buffer, "%03d.", i);
			LOG_Info("%s (%p) %s => %lu (%08lx - %p)\n", p ? "    " : buffer, e, e->k, (ULONG) e->v, e->h, e->next);
		}
	}
	#undef	LOG_Info
}
#endif

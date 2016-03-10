/*
 * Argv.v : create an arbitrary list of key/value pairs used for basic object creation.
 *
 * Written by T.Pierron, Oct 2007.
 */

#include "SWL.h"
#include <stdlib.h>
#include <string.h>

/**h* SWL/ARGV
 * INTRODUCTION
 *	This module is useful for creating simple read-only object, where you don't
 *	want to bother creating a dedicated structure and all the hassle associated.
 *
 *	The basic type to store such object is "ArgList", which is basically a
 *	string table composed of key/value pairs.
 *
 *	Basic creation function MakeArgList() let you provide integer, double or
 *	string for initializing your object, and simple macro let you retrieve them
 *	in their original form.
 *
 *	Since they are mostly intended to be read only, allocation of such object is
 *	done in one chunk, reducing memory fragmentation, and allowing to free them
 *	using a single call to the standard free() function.
 *
 *	Moreover a free field is included in the object, that is left untouched and
 *	where you can store whatever pointer you want. It is declared as is :
 *
 *		APTR al_UserData;
 *
 *	Example:
 *		ArgList object = MakrArgList(
 *			"page_digits",   ARG_INT, cfg->vic_PagePad,
 *			"delete_delay",  ARG_INT, cfg->vic_DelDelay,
 *			"backup_folder", cfg->vic_BackupFolder,
 *			"import_folder", cfg->vic_ImportDir,
 *			NULL, NULL
 *		);
 *
 *		// Get values
 *		digits = NUM(object, page_digits);
 *		backup = PROP(object, backup_folder);
 *
 *		FreeArgvList(object);
 *
 *	Note the double NULL token at the end of MakeArgList parameter list (see doc
 *	for explanation), and absense of " in the NUM and PROP macro for the key
 *	name.
 */

/**f* ARGV/MakeArgList
 * NAME
 *	MakeArgList - build generic objects
 *
 * SYNOPSIS
 *	ArgList MakeArgList(STRPTR id, ...);
 *
 * FUNCTION
 *	This function create a list of key/value pairs, that you can query through
 *	GetProperty(), using al_Array field of returned object. It is useful in case
 *	you don't want to create a dedicated structure and object is mostly read-only.
 *
 * INPUT
 *	- Give your list of key/value pair terminated by NULL. Those special value
 *	  can be given as key :
 *		- ARG_INT : value following will be extracted as (int) and converter to string (%d).
 *		- ARG_DBL : value will be extracted as (double) and converter to string (%g).
 *	  If value is NULL, empty string will be used instead.
 *	- After the list, this function expect to get a pointer to next object that
 *	  will linked into al_Next field of object created. This is used to create
 *	  linked list of object.
 *
 *	  You can also provide ARG_REV as object, and then a real ArgList pointer.
 *	  The al_Next field of this object will be set to the newly created object.
 *	  Useful when creating a series of object inside a loop.
 *
 * EXAMPLE
 *	ArgList list =
 *		MakeArgList("id", ARG_INT, 1, "value", "choice_1",
 *		MakeArgList("id", ARG_INT, 2, "value", "choice_2",
 *		MakeArgList("id", ARG_INT, 3, "value", "choice_3")));
 *
 *	while (list)
 *	{
 *		printf("id = %d, value = %s\n", NUM(list, id), PROP(list, value));
 *		FREE_AND_GONEXT(list);
 *	}
 *
 * SEE ALSO
 *	MakeArgListFromCGI, FreeArgvList, NUM, PROP, FREE_AND_GONEXT, GetProperty
 */
ArgList MakeArgList(STRPTR id, ...)
{
	ArgList ret;
	va_list args;
	STRPTR  key, value, buf;
	int     max, nb;

	/* Count argument size */
	va_start(args, id);
	key   = id;
	max   = nb = 0;
	value = va_arg(args, STRPTR);
	for (;;)
	{
		TEXT buf[32];
		if (value == ARG_INT) snprintf(value = buf, sizeof buf, "%d", va_arg(args, int)); else
		if (value == ARG_DBL) snprintf(value = buf, sizeof buf, "%g", va_arg(args, double));
		if (value == NULL)    value = "";
		max  += strlen(key) + strlen(value) + 2;
		nb   += 2;
		key   = va_arg(args, STRPTR);  if (key == NULL) break;
		value = va_arg(args, STRPTR);
	}
	va_end(args);

	/* malloc a new node */
	nb ++;
	va_start(args, id);
	max   = sizeof *ret + nb * sizeof (STRPTR) + max;
	ret   = malloc(max);
	buf   = (STRPTR) (ret->al_Array + nb);
	nb    = 0;
	key   = id;
	value = va_arg(args, STRPTR);
	for (;;)
	{
		ret->al_Array[nb] = buf; nb ++;
		buf += sprintf(buf, "%s", key) + 1;
		ret->al_Array[nb] = buf; nb ++;
		if (value == ARG_INT) buf += sprintf(buf, "%d", va_arg(args, int)); else
		if (value == ARG_DBL) buf += sprintf(buf, "%g", va_arg(args, double));
		else buf += sprintf(buf, "%s", value ? value : ""); buf ++;
		key   = va_arg(args, STRPTR);  if (key == NULL) break;
		value = va_arg(args, STRPTR);
	}
	memset(ret, 0, sizeof *ret);
	ret->al_Size = max;
	ret->al_Next = va_arg(args, ArgList);
	ret->al_Array[nb] = NULL;

	if (ret->al_Next == ARG_REV)
	{
		ArgList prev = va_arg(args, ArgList);
		if (prev) prev->al_Next = ret;
		ret->al_Next = NULL;
	}

	va_end(args);

	return ret;
}

/**f* ARGV/MakeArgListFromCGI
 * NAME
 *	MakeArgListFromCGI - Build an ArgList object from a url form-encoded request.
 *
 * SYNOPSIS
 *	ArgList MakeArgListFromCGI(STRPTR request);
 *
 * FUNCTION
 *	This function will decode the url form-encoded string and build an object,
 *	you can query using GetProperty(). It is based on ParseCGI.
 *
 * INPUT
 *	request - string to parse, a copy will be made. Original will be left untouched.
 *
 * SEE ALSO
 *	MakeArgList
 */
DLLIMP ArgList MakeArgListFromCGI(STRPTR request)
{
	ArgList list;
	Request req;
	STRPTR  buf;
	int     nb = StrCount(request, '&') * 2 + 3;
	int     i;

	list = malloc(sizeof *list + strlen(request) + 1 + nb * sizeof(STRPTR));
	if (list == NULL) return NULL;
	memset(list, 0, sizeof *list);
	list->al_Next = NULL;
	buf = (STRPTR) (list->al_Array + nb);
	strcpy(buf, request);
	ParseCGI(&req, buf);

	if (nb > req.req_NbArg)
		nb = req.req_NbArg;

	for (i = 0; i < nb; i ++)
		list->al_Array[i] = req.req_Arg[i];

	list->al_Array[i] = NULL;

	return list;
}

/**f* ARGV/ArgvAppend
 * NAME
 *	ArgvAppend - Append an object at the end of an ArgList.
 *
 * SYNOPSIS
 *	void ArgvAppend(ArgList * first, ArgList item);
 *
 * FUNCTION
 *	Add the object 'item' at the end of linked object list, whose first item
 *	reference is 'first'. 'item' should not be part of another list, since it
 *	will broke link.
 *
 * SEE ALSO
 *	ArgvRemove, ArgvInsert
 */
DLLIMP void ArgvAppend(ArgList * first, ArgList item)
{
	ArgList prev, list;

	for (list = *first, prev = NULL; list; prev = list, list = list->al_Next);

	if (prev) prev->al_Next = item;
	else      *first = item;
}

/**f* ARGV/ArgvRemove
 * NAME
 *	ArgvRemove - remove an object from a list and free memory
 *
 * SYNOPSIS
 *	void ArgvRemove(ArgList * first, ArgList item);
 *
 * FUNCTION
 *	Removes item from given list. Memory will also be freed.
 *
 * SEE ALSO
 *	ArgvAppend, ArgvInsert
 */
DLLIMP void ArgvRemove(ArgList * first, ArgList item)
{
	ArgList prev, list;

	for (list = *first, prev = NULL; list && list != item; prev = list, list = list->al_Next);

	if (list != item) return; // Not in this list ?

	if (prev) prev->al_Next = item->al_Next;
	else      *first = item->al_Next;

	if (item->al_Extra) free(item->al_Extra);
	free(item);
}

/**f* ARGV/ArgvInsert
 * NAME
 *	ArgvInsert - insert an argv object anywhere in the list
 *
 * SYNOPSIS
 *	void ArgvInsert(ArgList * first, ArgList insert_after, ArgList item);
 *
 * FUNCTION
 *	Insert item 'item' after 'insert_after' or beginning of list if
 *	'insert_after' is NULL.
 *
 * SEE ALSO
 *	ArgvAppend, ArgvRemove
 */
DLLIMP void ArgvInsert(ArgList * first, ArgList insert_after, ArgList item)
{
	if (insert_after)
	{
		item->al_Next = insert_after->al_Next;
		insert_after->al_Next = item;
	}
	else item->al_Next = *first, *first = item;
}

/**f* ARGV/FreeArgvList
 * NAME
 *	FreeArgvList - Free memory of a whole object list
 *
 * SYNOPSIS
 *	void FreeArgvList(ArgList start);
 *
 * SEE ALSO
 *	MakeArgList
 */
DLLIMP void FreeArgvList(ArgList start)
{
	while (start)
	{
		ArgList next = start->al_Next;
		if (start->al_Extra) free(start->al_Extra);
		free(start);
		start = next;
	}
}

/**f* ARGV/NUM, ARGV/PROP, ARGV/FREE_AND_GONEXT
 * NAME
 *	NUM - Retrieve a numeric property from a ArgList object.
 *	PROP - Retrieve a string property.
 *	FREE_AND_GONEXT - Step through item list.
 *
 * SYNOPSIS
 *	int NUM(ArgList obj, prop);
 *	STRPTR PROP(ArgList obj, prop);
 *	void FREE_AND_GONEXT(ArgList first);
 *
 * FUNCTION
 *	These are actually macros (based on GetProperty for NUM and PROP). 'prop' has
 *	indeed no type because, this parameter will be automatically stringified with
 *	macro expansion. Thus if you specify "property" as parameter for prop, the
 *	instruction will actually search for "\"property\"".
 *
 *	FREE_AND_GONEXT is used to browse through a list of object and free them in
 *	the same way.
 *
 * INPUT
 *	prop - property, without quotes. I.e. NUM(obj, name), will search for
 *	       a property "name" in the list.
 */

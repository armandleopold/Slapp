/*
 * String.c : Helper function for string manipulation.
 *
 * Written by T.Pierron, feb 17, 2006.
 */

#include "SWL.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/**h* SWL/String
 * INTRODUCTION
 *	Standard string.h functions are next to useless, and extremely dangerous for
 *	some (strcpy() and strncpy() at least).
 *
 *	Functions that might still be interesting in string.h are :
 *	- memset  : set a chunk of memory to a specified byte value.
 *	- memcpy  : copy chunk of memory.
 *	- memmove : like memcpy with overlapping check.
 *	- memchr  : search for a character in a chunk of memory.
 *	- strstr  : search for a substring in a c string.
 *	- strchr  : like memchr, but in a c string.
 *	- strrchr : last occurency of a character in a c string.
 *	- strcmp  : compare case-sensitively two c strings.
 *	- strncmp : compare two strings with maximal length check.
 *	- strcasecmp : compare case-insensitively two c strings.
 *	- strncasecmp
 *
 *	Be careful with following functions :
 *	- strcpy  : if you REALLY know the size of your string.
 *	- strcat  : append a string to another one. You have to check space
 *	            yourself.
 *
 *	AVOID :
 *	- strncpy : history may have forgotten it's purpose.
 *	- strncat : the count parameter is often misused.
 */

/**f* String/CopyString
 * INTRODUCTION
 *	CopyString - decent replacement for strncpy(). Keep your head safe.
 *
 * SYNOPSIS
 *	void CopyString(STRPTR destination, STRPTR source, int max);
 *
 * FUNCTION
 *	Purpose of this function is rather simple: copy string "source" into
 *	"destination", writing no more "max" character (including 0), ensuring
 *	that destination will be always NULL-terminated.
 *
 * SEE ALSO
 *	RawDoFmt, CopyStringEnc
 */
DLLIMP void CopyString(STRPTR destination, STRPTR source, int max)
{
	STRPTR d = destination;
	STRPTR s = source;
	int    i = max;

	if (s == NULL) s = "";

	while (i > 0 && (*d++ = *s++)) i --;

	if (i   > 0) d[ 0] = 0; else
	if (max > 0) d[-1] = 0;
}

void SkipPrintfArgs(STRPTR fmt, va_list * args)
{
	STRPTR p, type;

	for (p = fmt; *p; p ++)
	{
		int size;
		if (p[0] != '%') continue;
		if (p[1] == '%') { p ++; continue; }

		for (size = 0; *p && ! isalpha(*p); p ++);
		if (*p == 'h') p ++;
		if (*p == 'h') p ++;
		if (*p == 'l') p ++, size ++;
		if (*p == 'l') p ++, size ++;
		if (*p == 'L') p ++, size += 2;
		if (size > 2)  size = 2;

		type = strchr("1diuoxXcC2feEgG3sSpn", *p);

		if (type == NULL) return; /* Unrecognized format - not good */
		if (*p == 'c' || *p == 'C') size = 0;

		while (! isdigit(*type)) type --;
		switch (*type) {
		case '1':
			switch (size) {
			case 0: va_arg(*args, int); break;
			case 1: va_arg(*args, long); break;
			case 2: va_arg(*args, long long);
			}
			break;
		case '2':
			switch (size) {
			case 0:
			case 1: va_arg(*args, double); break;
			case 2: va_arg(*args, long double);
			}
			break;
		case '3':
			va_arg(*args, void *);
		}
	}
}

/**f* String/RawDoFmt
 * NAME
 *	RawDoFmt - printf-like function with buffer allocation.
 *
 * SYNOPSIS
 *	STRPTR RawDoFmt(STRPTR format, va_list * args);
 *
 * FUNCTION
 *	This function will call printf on the format and to the arguments passed,
 *	allocating a large enough buffer to hold the result.
 *
 *	Since orginal libc printf() is used, you can use almost all possible
 *	format specifier, including the nasty positionnal arguments. There is
 *	one restriction : the argument size specifier. Actually for portability
 *	only the following size specifier are recognized :
 *	- h, hh, l, ll, L
 *
 *	This is because RawDoFmt will modify 'args' so that applying va_arg on
 *	it, will give you the next argument passed in the var arg list.
 *
 * RESULT
 *	Formatted string into a malloc()'ed buffer, that you must free yourself.
 *
 * SEE ALSO
 *	printf(3)
 */
DLLIMP STRPTR RawDoFmt(STRPTR fmt, va_list * args)
{
	int     max = 256, length = 0;
	STRPTR  ret = malloc(max);

	while (ret)
	{
		length = vsnprintf(ret, max, fmt, *args);

		/* Okay, return value of snprintf is somewhat system dependant though
		 * standard says that it should return the number of bytes that would
		 * have been written whatever size of output buffer is.
		 */
		if (length < 0 || length >= max - 1)
		{
			/* Seems not enough large */
			STRPTR newstr = realloc(ret, max += 256);

			if (newstr == NULL) { free(ret); return NULL; }

			ret = newstr;
		}
		else break;
	}
	if (++ length < max)
	{
		STRPTR newstr = realloc(ret, max);

		if (newstr) ret = newstr;
	}
	SkipPrintfArgs(fmt, args);
	return ret;
}


/**f* String/StrCatM, String/StrCat, String/StrCatLen
 * NAME
 *	StrCatM   - catenate a number of strings into a buffer
 *	StrCat    - wrapper for StrCatM
 *	StrCatLen - Also limit bytes to copy from source.
 *
 * SYNOPSIS
 *	int StrCatM(STRPTR dest, int max, int c, ...);
 *	int StrCat(STRPTR dest, STRPTR src, int max);
 *	int StrCatLen(STRPTR dst, STRPTR src, int max, int len);
 *
 * FUNCTION
 *	StrCatM will catenate NULL-terminated vararg list of strings into
 *	the supplied buffer, which is "max" bytes length. Resulting string will
 *	always be 0-terminated.
 *
 *	StrCat is a simple macro, that will expand to "StrCatM(dest, max, -1, src, NULL)".
 *
 *	StrCatLen will copy at most "len" bytes from "src" into "dst", which is
 *	"max" bytes long.
 *
 * INPUT
 *	dest - Buffer where result will be written. Must be already initialized will
 *	       a NULL-terminated string.
 *	max  - available bytes in dest.
 *	c    - character to put between each string, -1 for none, if character
 *	       is already present at the end of a string, it won't be appended.
 *	...  - vararg list of string to catenate. MUST BE NULL terminated !!
 *
 * RESULT
 *	Number of character in "dest" (not including final 0), so that
 *	"dest + result" should bring you to the end of string.
 *
 * SEE ALSO
 *	StrCatDup
 */
DLLIMP int StrCatM(STRPTR dest, int max, int c, ...)
{
	va_list args;
	STRPTR src;
	STRPTR ptr = dest;
	STRPTR eof = dest + max;
	Bool   add = False;

	if (dest == NULL) return 0;

	for (eof = dest + max; *ptr; ptr ++);
	add = (c >= 0 && ptr > dest && ptr[-1] != c);

	for (va_start(args, c); ptr < eof && (src = va_arg(args, STRPTR)); )
	{
		STRPTR s;
		if (add) *ptr ++ = c;
		for (s = src; *s && ptr < eof; *ptr ++ = *s ++);
		add = (c >= 0 && (s == src || s[-1] != c));
	}
	if (dest < ptr)
	{
		if (ptr < eof) ptr[ 0] = 0;
		else           ptr[-1] = 0;
	}
	va_end(args);
	return ptr - dest;
}

DLLIMP int StrCatLen(STRPTR dst, STRPTR src, int max, int len)
{
	STRPTR start = dst;

	if (src == NULL) return strlen(dst);
	for (; *dst; dst ++, max --);

	while (max > 0 && len) *dst++ = *src++, max --, len --;

	if (max <= 0) dst --;
	dst[0] = 0;

	return start - dst;
}

/**f* String/StrCatDup
 * NAME
 *	StrCatDup - catenate a list of strings into a malloc()'ed buffer
 *
 * SYNOPSIS
 *	Bool StrCatDup(STRPTR * buffer, int c, ...);
 *
 * FUNCTION
 *	This function will catenate NULL-terminated vararg list of strings and
 *	alloc a buffer large enough to hold the result. You can specify a
 *	character that will be inserted between, unless the string has already
 *	a such character at the end.
 *
 * INPUT
 *	buffer - pointer to allocated buffer will be written here. It must point
 *	         to a valid malloc()'ed buffer or NULL.
 *	c      - character to insert between each string or -1 for none.
 *	...    - NULL-terminated vararg list of string to catenate.
 *
 * RESULT
 *	True if buffer was successfully expanded/allocated. False if something
 *	went wrong (usually not enough mem). In the latter case, the value of
 *	(*buffer) will not be changed.
 *
 * SEE ALSO
 *	StrCat
 */
DLLIMP Bool StrCatDup(STRPTR * buffer, int c, ...)
{
	va_list args;
	STRPTR src;
	STRPTR dst;
	Bool   add;
	int    sz, length;

	length = *buffer ? strlen(*buffer) : 0;

	/* First count size to alloc */
	for (va_start(args, c), sz = length; (src = va_arg(args, STRPTR)); )
	{
		if (sz > length && c >= 0) sz ++;
		while (*src) { src ++; sz ++; }
	}
	va_end(args);

	/* Same size as before */
	if (sz == length) return True;

	/* Enlarge/alloc buffer */
	dst = realloc(*buffer, sz + 1);

	if (dst == NULL) return False;

	for (va_start(args, c), *buffer = dst, dst = *buffer + length, add = False;
	    (src = va_arg(args, STRPTR)); )
	{
		STRPTR s;
		if (add) *dst ++ = c;
		for (s = src; *s; *dst ++ = *s ++);
		add = (c >= 0 && (s == src || s[-1] != c));
	}
	*dst = 0;
	return True;
}

/**f* String/StripCRLF
 * NAME
 *	StripCRLF - remove trailing newline at end of string
 *
 * SYNOPSIS
 *	STRPTR StripCRLF(STRPTR in);
 *
 * FUNCTION
 *	This function will modify given string, removing any trailing newline at the
 *	end of buffer. This is usually useful when reading a text file line by line.
 *	Note that it will removes both Unix (LF), DOS (CRLF) or Mac (CR) newlines.
 *
 * RESULT
 *	The address of input buffer is returned.
 */
DLLIMP STRPTR StripCRLF(STRPTR in)
{
	STRPTR eol = strchr(in, 0);

	while (eol > in && (eol[-1] == '\r' || eol[-1] == '\n')) *--eol = 0;

	return in;
}


/**f* String/StrLenM
 * NAME
 *	StrLenM - Computer length of multiple string in one call.
 *
 * SYNOPSIS
 *	int StrLenM(STRPTR src, ...);
 *
 * FUNCTION
 *	It will computer length (NOT including final 0) of the NULL-terminated
 *	list passed as arguments.
 */
DLLIMP int StrLenM(STRPTR src, ...)
{
	va_list args;
	int     len = strlen(src);

	va_start(args, src);
	while ((src = va_arg(args, STRPTR)))
	{
		len += strlen(src);
	}
	va_end(src);
	return len;
}

/**f* String/AssignStrings
 * NAME
 *	AssignStrings - Copy strings and fill an array pointing to their location.
 *
 * SYNOPSIS
 *	void AssignStrings(STRPTR * list, void * buffer, ...);
 *
 * FUNCTION
 *	Copy the NULL-terminated list of strings passed as varargs. Strings will
 *	be copied, starting at 'buffer'. 'list' will be filled to point to the
 *	start of each string copied.
 *
 * INPUT
 *	list   - Array that will point the start of each string copied.
 *	buffer - Start of buffer where string will be copied one after the other.
 *	...    - list of string to copy.
 *
 * EXAMPLE
 *	This function is useful for object initialization, which contains a lot of
 *	strings. Like :
 *		MyObject obj = malloc(sizeof *obj + StrLenM(str1, str2, str3, NULL) + 3);
 *
 *		if (obj)
 *		{
 *			AssignStrings(&obj->str1_field, obj + 1, str1, str2, str3, NULL);
 *			// ...
 *		}
 *
 *	This assume that MyObject is a structure containing a STRPTR field 'str1_field'
 *	and followed by two fields of the same kind.
 */
DLLIMP void AssignStrings(STRPTR * list, void * buffer, ...)
{
	va_list args;
	STRPTR  src;
	STRPTR  p = buffer;

	va_start(args, buffer);
	while ((src = va_arg(args, STRPTR)))
	{
		*list++ = p;
		strcpy(p, src);
		p += strlen(p) + 1;
	}
	va_end(src);
}

/**f* String/StrCount
 * NAME
 *	StrCount - Count character occuring in given string
 *
 * SYNOPSIS
 *	int StrCount(STRPTR src, int chr);
 *
 * INPUT
 *	src - input buffer to scan. If NULL, 0 is returned.
 *	chr - character to check, will be truncated to TEXT.
 *
 * SEE ALSO
 *	CharInSet
 */
DLLIMP int StrCount(STRPTR list, int chr)
{
	STRPTR p;
	int    n;

	if (list == NULL) return 0;
	for (n = 0, p = list; *p; p ++)
		if (*p == (TEXT) chr) n ++;

	return n;
}

/**f* String/CharInSet
 * NAME
 *	CharInSet - Count characters occuring in given string
 *
 * SYNOPSIS
 *	int CharInSet(STRPTR src, STRPTR set);
 *
 * FUNCTION
 *	This function will count how many times given characters in 'set' appear in
 *	the string 'src'. Can be useful to compute size of an escaped version of the
 *	string.
 *
 * SEE ALSO
 *	StrCount
 */
DLLIMP int CharInSet(STRPTR str, STRPTR set)
{
	STRPTR p;
	int nb;

	for (p = str, nb = 0; *p; p ++)
	{
		if (strchr(set, *p)) nb ++;
	}
	return nb;
}

/**f* String/StrReplace
 * NAME
 *	StrReplace - Replace first occurence of a string.
 *
 * SYNOPSIS
 *	Bool StrReplace(STRPTR src, STRPTR find, STRPTR rep, int max);
 *
 * FUNCTION
 *	Replace first occurence of 'find' in 'src', replacing it by 'rep'. 'src'
 *	buffer is supposed to be at most 'max' bytes. If buffer is too small to
 *	contain a larger replacement string, buffer will be truncated.
 *
 * RESULT
 *	True if an occurence has been replaced, False otherwise.
 *
 * SEE ALSO
 *	StrCat
 */
DLLIMP Bool StrReplace(STRPTR src, STRPTR find, STRPTR rep, int max)
{
	STRPTR ins = strstr(src, find);
	int    szr = strlen(rep);
	int    szf = strlen(find);

	if (ins)
	{
		int lg = strlen(ins) + szr - szf + 1;
		max -= (ins - src);
		if (lg > max) lg = max;
		memmove(ins + szr, ins + szf, lg);
		memcpy(ins, rep, szr);
		ins[lg-1] = 0;
		return True;
	}
	return False;
}

/**f* String/NthWord
 * NAME
 *	NthWord - return pointer to nth word in a string
 *
 * SYNOPSIS
 *	STRPTR NthWord(STRPTR str, int nth, int chr);
 *
 * FUNCTION
 *	Search for nth word (where n is 0-based. I.e. 0 is first item), in 'chr'
 *	separated list, encoded as string.
 *
 * RESULT
 *	Pointer within string pointing to start of nth item or NULL if list is
 *	too small.
 */
DLLIMP STRPTR NthWord(STRPTR str, int nth, int chr)
{
	while (nth > 0)
	{
		str = strchr(str, chr);
		if (str) str ++; else break;
		nth --;
	}
	return str;
}

/**f* String/FindInList
 * NAME
 *	FindInList - Simple way to convert string to integer
 *
 * SYNOPSIS
 *	int FindInList(STRPTR list, STRPTR word, int len);
 *
 * INPUT
 *	list - string encoded list, with words separated by comma.
 *	word - whole word to be matched case-insentively in the list.
 *	len  - bytes length of word to compare (<= 0 to compare strlen(word) first
 *         bytes).
 *
 * RESULT
 *	0 if word matched with first item of list, 1 if matched with second, ...
 *	-1 if matched with nothing.
 *
 * EXAMPLE
 *	FindInList("apple,orange,pear", "orange", 0);
 *		=> returns 1
 *
 *	FindInList("apple,orange,pear,oran", "orange", 4);
 *		=> returns 3 (words must match wholy).
 *
 * NOTE
 *	Words of the list should not contains leading or starting spaces. Otherwise
 *	they will be considered being part of word. I.e. : "word, separated, list"
 *	will be composed of words "word", " separated" and " list".
 *
 */
DLLIMP int FindInList(STRPTR list, STRPTR word, int len)
{
	int pos = 0;

	if (list == NULL || word == NULL) return -1;

	if (len <= 0)
		len = strlen(word);

	while (list)
	{
		if (strncasecmp(list, word, len) == 0 && (list[len] == ',' || list[len] == 0))
			return pos;

		pos ++;
		list = strchr(list, ',');
		if (list) list ++;
	}
	return -1;
}

/**f* String/FnMatch
 * NAME
 *	FnMatch - Match DOS pattern against a string
 *
 * SYNOPSIS
 *	Bool FnMatch(STRPTR pattern, STRPTR string, int flags);
 *
 * FUNCTION
 *	This function is like fnmatch(3).
 *
 * RESULT
 *	True if pattern matched the string, False otherwise.
 */
DLLIMP Bool FnMatch(STRPTR pattern, STRPTR string, int flags)
{
	STRPTR p = pattern, n = string;
	TEXT   c;

	/* Note that this evaluates C many times.  */
# define FOLD(c) ((flags & FNM_CASEFOLD) && isupper (c) ? tolower (c) : (c))

	while ((c = *p++))
	{
		c = FOLD(c);

		switch (c) {
		case '?':
			if (*n == '\0')
				return False;
			else if ((flags & FNM_PATHNAME) && *n == '/')
				return False;
			else if ((flags & FNM_PERIOD) && *n == '.' &&
			         (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
				return False;
			break;

		case '\\':
			if (!(flags & FNM_NOESCAPE))
			{
				c = *p++;
				if (c == '\0')
					/* Trailing \ loses.  */
					return False;
				c = FOLD (c);
			}
			if (FOLD (*n) != c)
				return False;
			break;

		case '*':
			if ((flags & FNM_PERIOD) && *n == '.' &&
			        (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
				return False;

			for (c = *p++; c == '?' || c == '*'; c = *p++)
			{
				if ((flags & FNM_PATHNAME) && *n == '/')
					/* A slash does not match a wildcard under FNM_FILE_NAME */
					return False;
				else if (c == '?')
				{
					/* A ? needs to match one character */
					if (*n == '\0')
						/* There isn't another character; no match */
						return False;
					else
						++n;
				}
			}

			if (c == '\0')
				return True;

			{
				TEXT c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
				c1 = FOLD (c1);
				for (--p; *n != '\0'; ++n)
					if ((c == '[' || FOLD (*n) == c1) && FnMatch (p, n, flags & ~FNM_PERIOD))
						return True;
				return False;
			}

		case '[':
		{
			/* Nonzero if the sense of the character class is inverted */
			register int neg;

			if (*n == '\0') return False;

			if ((flags & FNM_PERIOD) && *n == '.' &&
			        (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
				return False;

			neg = (*p == '!' || *p == '^');
			if (neg) ++p;

			c = *p++;
			for (;;)
			{
				register char cstart = c, cend = c;

				if (!(flags & FNM_NOESCAPE) && c == '\\')
				{
					if (*p == '\0')
						return False;
					cstart = cend = *p++;
				}

				cstart = cend = FOLD (cstart);

				/* [ (unterminated) loses */
				if (c == '\0') return False;

				c = *p++;
				c = FOLD(c);

				if ((flags & FNM_PATHNAME) && c == '/')
					/* [/] can never match.  */
					return False;

				if (c == '-' && *p != ']')
				{
					cend = *p++;
					if (!(flags & FNM_NOESCAPE) && cend == '\\')
						cend = *p++;
					if (cend == '\0')
						return False;
					cend = FOLD(cend);

					c = *p++;
				}

				if (FOLD(*n) >= cstart && FOLD (*n) <= cend)
					goto matched;

				if (c == ']') break;
			}
			if (! neg)
				return False;
			break;

			matched:

			/* Skip the rest of the [...] that already matched.  */
			while (c != ']')
			{
				/* [... (unterminated) loses.  */
				if (c == '\0') return False;

				c = *p++;
				if (!(flags & FNM_NOESCAPE) && c == '\\')
				{
					if (*p == '\0') return False;
					/* XXX 1003.2d11 is unclear if this is right.  */
					++p;
				}
			}
			if (neg) return False;
		}
		break;

		default:
			if (c != FOLD (*n))
				return False;
		}
		++n;
	}

	if (*n == '\0') return True;

	if ((flags & FNM_LEADING_DIR) && *n == '/')
		/* The FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
		return True;

	return False;

# undef FOLD
}


DLLIMP uint32_t LE32ToHost(uint32_t raw)
{
	uint8_t * n = (uint8_t *) &raw;
	return n[0] | (n[1] << 8) | (n[2] << 16) | (n[3] << 24);
}

DLLIMP uint32_t HostToBE32(uint32_t raw)
{
	uint8_t buf[4];

	buf[0] = (raw >> 24);
	buf[1] = (raw >> 16) & 0xff;
	buf[2] = (raw >> 8)  & 0xff;
	buf[3] = (raw)       & 0xff;

	return * (uint32_t *) buf;
}


/*
 * CGI.c : Common Gateway Interface helper functions.
 *
 * Written by T.Pierron, feb 15, 2006.
 */

#include "SWL.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/**h* SWL/CGI
 * INTRODUCTION
 *	This module contains a few functions to deal with CGI requests. Functions are
 *	very basic, don't expect too much, it just avoid the hassle to rewrite them
 *	for the hundreedth time.
 *
 *	CGI functions make use of the following datatype:
 *
 *		struct Request_t
 *		{
 *			STRPTR req_Arg[MAXKEYS];
 *			int    req_NbArg;
 *		};
 *
 *	Field names aren't important, types are. MAXKEYS is a macro defined in
 *	the main header (default : 64).
 *
 *	ParseCGI() is used to parse a url-encoded string and setup the key/value
 *	pairs in the req_Arg and req_NbArg fields, so that you can furthermore query
 *	for specific values using GetValue, GetNumValue, ...
 *
 *	This datatypes can be changed, to add more fields. Since heritage is only a
 *	C++ feature, here the hack to work around this limitation:
 *
 *	- Define a macro CUSTOM_REQ before including the main header.
 *	- Declare your own struct Request_t somewhere else with those very same
 *	  initial fields (name does not matter). Follow them by whatever you want.
 *
 *	Not perfect, but it works at least.
 */

DLLIMP int HexToBin(TEXT c)
{
	if ('0' <= c && c <= '9') return c - '0';
	if ('a' <= c && c <= 'f') return c - 'a' + 10;
	return c - 'A' + 10;
}

/**f* CGI/ParseCGI
 * NAME
 *	ParseCGI - Parse a url-encoded string and split it into key/value pairs.
 *
 * SYNOPSIS
 *	void ParseCGI(Request * req, STRPTR cgi);
 *
 * FUNCTION
 *	This function will parse a string encoded as URL, i.e.: key1=value1&key2=value2&...
 *	keys and values can contains '+' and '%XX' sequences, which will be converted
 *	in ' ' and ASCII value represented by hex sequence respectively.
 *
 *	No memory are allocated in the process, "cgi" string will be modified and be
 *	referenced through "req", thus be sure "cgi" lifespan last as long as "req".
 *
 * INPUT
 *	req - object that will hold the key/value pairs.
 *	cgi - string to parse. Will be modified, avoid constant strings.
 *
 * SEE ALSO
 *	Unescape, GetValue
 */
DLLIMP void ParseCGI(Request * req, STRPTR cgi)
{
	STRPTR s, d, * q;

	memset(req, 0, sizeof *req);

	for (s = d = cgi, q = req->req_Arg, *q++ = s; *s && q - req->req_Arg < MAXKEYS; s++, d++)
	{
		switch (*s) {
		case '+': *d = ' '; break;
		case '=': *d = 0; *q++ = d+1; break;
		case '%': *d = (HexToBin(s[1]) << 4) | HexToBin(s[2]); s += 2; break;
		case '&': *d = 0; *q++ = s+1; d = s; break;
		default : *d = *s;
		}
	}
	*d = 0;
	req->req_NbArg = q - req->req_Arg;
	if (req->req_NbArg & 1)
		req->req_NbArg --;
}

/**f* CGI/Unescape
 * NAME
 *	Unescape - Convert %\x\x sequence into characters.
 *
 * SYNOPSIS
 *	void Unescape(STRPTR io);
 *
 * FUNCTION
 *	This function will convert %\x\x sequence into the corresponding
 *	8bit character. Unicode sequence %u\x\x\x\x are converted into UTF-8.
 *
 * INPUT
 *	io - string to convert.
 *
 * SEE ALSO
 *	ParseCGI
 */
DLLIMP void Unescape(STRPTR src)
{
	STRPTR dst;

	for (dst = src; *src; src ++)
	{
		if (*src == '%')
		{
			if (src[1] == 'u')
			{
				/* Unicode char */
				int ch = (HexToBin(src[2]) << 12) | (HexToBin(src[3]) << 8) |
				         (HexToBin(src[4]) <<  4) | (HexToBin(src[5]));

				/* Unicode CP to UTF-8 */
				if (ch <= 0x7F) {
					*dst++ = ch;
				} else if (ch <= 0x7FF) {
					*dst++ = 0xC0 | ((ch >> 6) & 0x1F);
					*dst++ = 0x80 | (ch & 0x3F);
				} else if (ch <= 0xFFFF) {
					*dst++ = 0xE0 | ((ch >> 12) & 0x0F);
					*dst++ = 0x80 | ((ch >> 6) & 0x3F);
					*dst++ = 0x80 | (ch & 0x3F);
				} else if (ch <= 0x10FFFF) {
					*dst++ = 0xF0 | ((ch >> 18) & 0x07);
					*dst++ = 0x80 | ((ch >> 12) & 0x3F);
					*dst++ = 0x80 | ((ch >> 6)  & 0x3F);
					*dst++ = 0x80 | (ch & 0x3F);
				}
				src += 3;
			}
			else *dst++ = (HexToBin(src[1]) << 4) | HexToBin(src[2]);
			src += 2;
		}
		else *dst++ = *src;
	}
	*dst = 0;
}


/**f* CGI/GetValue, CGI/GetNumValue, CGI/GetBoolValue, CGI/ExtractValue, CGI/SetValue
 * NAME
 *	GetValue     - Get value associated to given key.
 *	GetNumValue  - Get numeric value associated to key (always decimal).
 *	GetBoolValue - Get numeric value associated to key, assuming value is a bool.
 *	ExtractValue - Like GetValue, but also removes key/value pair from list.
 *	SetValue     - Add or change a value associated to key.
 *
 * SYNOPSIS
 *	STRPTR GetValue(Request * req, STRPTR key);
 *	ULONG  GetNumValue(Request * req, STRPTR key);
 *	Bool   GetBoolValue(Request * req, STRPTR key);
 *	STRPTR ExtractValue(Request * req, STRPTR key);
 *	STRPTR SetValue(Request * req, STRPTR key, STRPTR value);
 *
 * FUNCTION
 *	These functions retrieve value associated to given key. ExtractValue will
 *	also remove the key/value pair from object.
 *
 *	GetNumValue will convert the string into an unsigned integer. Value will
 *	always supposed to be a decimal number (ie: even if there is leading 0s or
 *	0x prefix, function will try to convert whatever it can). If function
 *	somehow fails.
 *
 *	SetValue will change the value associated to given key. If it does not
 *	exists, the key/value pair will be added. Both key and value must point
 *	to valid memory block while object is in use.
 *
 * RESULT
 *	- GetValue     : value associated to key, unconverted. NULL if not found.
 *	- GetNumValue  : value converter into ULONG, or 0 if not found.
 *	- GetBoolValue : 'True' if value is "1" or "on". 'False' otherwise. This kind
 *                   of value is usually returned by checkbox inside HTML forms.
 *	- ExtractValue : Like GetValue.
 *	- SetValue     : pointer to value or NULL if key cannot be inserted.
 *
 * SEE ALSO
 *	ParseCGI, GetProperty
 */
DLLIMP STRPTR GetValue(Request * req, STRPTR key)
{
	STRPTR * keys;

	for (keys = req->req_Arg; *keys && keys - req->req_Arg < req->req_NbArg; keys += 2)
		if (strcasecmp(key, *keys) == 0) return keys[1];

	return NULL;
}

DLLIMP ULONG GetNumValue(Request * req, STRPTR key)
{
	STRPTR * keys;

	for (keys = req->req_Arg; *keys && keys - req->req_Arg < req->req_NbArg; keys += 2)
		if (strcasecmp(key, *keys) == 0) return strtoul(keys[1], NULL, 10);

	return 0;
}

DLLIMP Bool GetBoolValue(Request * req, STRPTR key)
{
	STRPTR value = GetValue(req, key);

	if (value == NULL) return 0;
	if (strcasecmp(value, "on") == 0 || strcmp(value, "1") == 0) return 1;
	return atoi(value);
}

DLLIMP STRPTR SetValue(Request * req, STRPTR key, STRPTR value)
{
	STRPTR * keys;

	for (keys = req->req_Arg; *keys && keys - req->req_Arg < req->req_NbArg; keys += 2)
		if (strcasecmp(key, *keys) == 0) return keys[1] = value;

	if (req->req_NbArg < MAXKEYS)
	{
		*keys = key;   keys ++;
		*keys = value; req->req_NbArg += 2;
		return value;
	}
	return NULL;
}

DLLIMP STRPTR ExtractValue(Request * req, STRPTR key)
{
	STRPTR * keys;

	for (keys = req->req_Arg; *keys && keys - req->req_Arg < req->req_NbArg; keys += 2)
	{
		if (strcasecmp(key, *keys) == 0)
		{
			STRPTR value = keys[1];
			memcpy(keys, keys + 2, (req->req_NbArg - (keys - req->req_Arg - 2)) * sizeof (STRPTR));
			req->req_NbArg -= 2;
			return value;
		}
	}

	return NULL;
}

/**f* CGI/GetProperty, CGI/GetNumProperty
 * NAME
 *	GetProperty    - Get value from a NULL-terminated key/value pair table.
 *	GetNumProperty - Like GetProperty, but convert the value into ULONG.
 *
 * SYNOPSIS
 *	STRPTR GetProperty(STRPTR * list, STRPTR key);
 *	int GetNumProperty(STRPTR * list, STRPTR key);
 *
 * FUNCTION
 *	Get a value associated to a key in a table of strings. Even indexes are
 *	considered to be keys, odd are supposed to be values. Table must be
 *	NULL-terminated.
 *
 * RESULT
 *	GetProperty returns value quoted from table or NULL if not found.
 *	GetNumProperty will attempt to convert to integer. If it fails or if key is
 *	not defined, 0 will be returned.
 *
 * SEE ALSO
 *	ParseCGI, GetValue
 */
DLLIMP STRPTR GetProperty(STRPTR * list, STRPTR key)
{
	int i;

	if (list == NULL) return NULL;

	for (i = 0; list[i]; i += 2)
		if (strcasecmp(list[i], key) == 0) return list[i + 1];

	return NULL;
}

DLLIMP int GetNumProperty(STRPTR * list, STRPTR key)
{
	int i;

	if (list == NULL) return 0;

	for (i = 0; list[i]; i += 2)
		if (strcasecmp(list[i], key) == 0) return strtoul(list[i+1], NULL, 10);

	return 0;
}

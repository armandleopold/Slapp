/*
 * Encodings.c : function for converting to/from UTF-16
 *
 * Written by T.Pierron, Jan 2008.
 */

#include "SWL.h"
#include <stdio.h>
#include <windows.h>
#include <wchar.h>

/**h* SWL/ENC
 * INTRODUCTION
 *	Encoding is a small module to help you converting to/from well known encoding.
 *	It supports 3 main encodings:
 *	- CP1252 (a.k.a. Windows-1252) : 8bit charset, ASCII compatible, super-set of
 *	  iso-8859-1. Barely useful for latin language.
 *
 *	- UTF8 : 8bit variable-length unicode charset. This is the preferred way to
 *	  deal with unicode string within this library. Since it is ASCII compatible,
 *	  it can be used fairly easily. Supports the full unicode range in at most
 *	  4 bytes.
 *
 *	- UTF16 : 16bit variable-length unicode charset. Somewhat similar to UTF8,
 *	  but more suited for asian glyghs, since it uses less space (2bytes, instead
 *	  of 3 for UTF8). Mainly used internally, since require careful app crafting
 *	  to be switched from an 8bit charset.
 */

/**f* ENC/CopyStringEnc
 * NAME
 *	CopyStringEnc - Copy a string with encoding conversion.
 *
 * SYNOPSIS
 *	int CopyStringEnc(STRPTR dest, int max, STRPTR src, int enc);
 *
 * FUNCTION
 *	This function will copy the NULL-terminated source string into "dest",
 *	converting "src" from encoding "enc" to UTF-16. "max" is the size in bytes
 *	of destination buffer.
 *
 *	If buffer is too small, only complete characters will be copied.
 *
 * INPUT
 *	dest - destination buffer.
 *	max  - max bytes to write.
 *	src  - source to convert from. Supposed to only contain valid characters.
 *	enc  - encoding of src, among :
 *		ENC_CP1252 - Windows-1252 charset. Superset of iso-8859-1.
 *		ENC_UTF8   - Unicode with variable-length characters.
 *		ENC_UTF16  - Unicode 16bits.
 *
 * RESULT
 *	Number of characters (NOT bytes) copied into the buffer. If you want bytes,
 *	search for NULL.
 *
 * SEE ALSO
 *	CopyString, StrDupEnc
 */
/* CP1252 is basically like unicode except characters 128 to 159 */
static uint16_t cp1252[] =
{
		0x20AC, 0x0000, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
		0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x017D, 0x0000,
		0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
		0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0000, 0x017E, 0x0178
};

DLLIMP int CopyStringEnc(STRPTR dest, int max, STRPTR str, int enc)
{
	LPWSTR  out = (LPWSTR) dest;
	LPWSTR  src16;
	data8_t src;
	int     cp, n, i, nb;

	if (max == 0) return 0;
	nb = 0;

	switch (enc) {
	case ENC_CP1252:
		for (src = str; *src && max > 0; src ++, max -= sizeof (out[0]), out ++, nb ++)
		{
			if (128 <= *src && *src < 160)
				*out = cp1252[*src - 128];
			else
				*out = *src;
		}
		break;
	case ENC_UTF8:
		for (src = str; *src && max > 0; src += 1 + n, max -= sizeof *out, nb ++)
		{
			cp = *src;
			n  = 0;
			if ((cp & 0xe0) == 0xc0) {
				cp &= 0x1f; n = 1;
			} else if ((cp & 0xf0) == 0xe0) {
				cp &= 0x0f; n = 2;
			} else if ((cp & 0xf8) == 0xf0) {
				cp &= 0x07; n = 3;
			}

			for (i = 1; i <= n; i ++)
				cp = (cp << 6) | (src[i] & 0x3f);

			if (cp >= 0x10000)
			{
				/* Need surrogate pair */
				if (max < 2 * sizeof *out) break; /* Do not produce incomplete chars */
				cp = (cp - 0x10000) & 0xfffff;
				*out++ = 0xd800 + (cp >> 10);
				*out++ = 0xdc00 + (cp & 0x3ff);
				max -= sizeof *out;
			}
			else *out++ = cp;
		}
		break;
	case ENC_UTF16:
		for (src16 = (LPWSTR) str; *src16 && max > 0; *out = *src16,
		     nb += (0xDC00 <= *out && *out <= 0xDFFF ? 0 : 1), out ++, src16 ++, max -= sizeof (out[0]));
	}
	if (max == 0)
	{
		cp = out[-1];
		if (0xDC00 <= cp && cp <= 0xDFFF) out[-2] = 0;
		else out[-1] = 0;
		nb --;
	}
	else out[0] = 0;

	return nb;
}

/**f* ENC/StrDupEnc
 * NAME
 *	StrDupEnc - allocate memory for a string, converting encoding if necessary.
 *
 * SYNOPSIS
 *	APTR StrDupEnc(STRPTR str, int enc);
 *
 * FUNCTION
 *	This function will alloc a buffer large enough to copy and convert the string
 *	and its encoding into UTF-16.
 *
 * SEE ALSO
 *	CopyStringEnc
 */
DLLIMP APTR StrDupEnc(STRPTR str, int enc)
{
	STRPTR src;
	LPWSTR ws;
	int n, nb = 1;

	switch (enc) {
	case ENC_CP1252: nb = strlen(str) + 1; break;
	case ENC_UTF16:  nb = wcslen((LPWSTR) str) + 1; break;
	case ENC_UTF8:
		for (src = str; *src; src += 1 + n, nb ++)
		{
			unsigned char cp = *src;
			n = 0;
			if ((cp & 0xe0) == 0xc0) n = 1; else
			if ((cp & 0xf0) == 0xe0) n = 2; else
			if ((cp & 0xf8) == 0xf0) n = 3;

			if (n >= 3) nb ++;
		}
	}
	nb  = (nb + 10) * sizeof *ws;
	src = malloc(nb);
	CopyStringEnc(src, nb, str, enc);
	return src;
}

/**f* ENC/NthChar
 * NAME
 *	NthChar - get nth character of string, taking care of encoding.
 *
 * SYNOPSIS
 *	STRPTR NthChar(STRPTR str, int nth, int enc);
 *
 * FUNCTION
 *	This function will take care of encoding to get start of nth character. It
 *	will also take care to not give you an address past terminal 0 character.
 *
 * SEE ALSO
 *	NextChar, CopyStringEnc
 */
DLLIMP STRPTR NthChar(STRPTR str, int nth, int enc)
{
	switch (enc) {
	case ENC_CP1252:
		while (*str && nth) str ++, nth --;
		return str;
	case ENC_UTF16:
		while (nth > 0)
		{
			wchar_t cp = * (LPWSTR) str;

			if (cp == 0) return str;
			str += 0xD800 <= cp && cp <= 0xDBFF ? 4 : 2;
			nth --;
		}
		return str;
	case ENC_UTF8:
		while (nth > 0)
		{
			unsigned char cp = *str;
			int n = 1;
			if ((cp & 0xe0) == 0xc0) n = 2; else
			if ((cp & 0xf0) == 0xe0) n = 3; else
			if ((cp & 0xf8) == 0xf0) n = 4;
			while (*str && n) str ++, n --;
			if (*str == 0) return str;
			nth --;
		}
		return str;
	}
	return NULL;
}

/**f* ENC/ConvertEnc
 * NAME
 *	ConvertEnc - Convert UTF-16 string into specified encoding.
 *
 * SYNOPSIS
 *	void ConvertEnc(STRPTR out, int max, STRPTR in, int enc);
 *
 * FUNCTION
 *	Convert the specified NULL-terminated string into encoding specified. If
 *	string does not fit in the output buffer, it will be truncated so that only
 *	complete characters will remain. Resulting string will always be
 *	NULL-terminated.
 *
 * INPUT
 *	out - destination buffer.
 *	in  - NULL-terminated source buffer.
 *	max - size of output buffer in bytes.
 *	enc - encoding of "in", see CopyStringEnc
 *
 * SEE ALSO
 *	CopyString
 */
DLLIMP void ConvertEnc(STRPTR out, int max, STRPTR in, int enc)
{
	LPWSTR src = (LPWSTR) in;
	int    nb;

	switch (enc) {
	case ENC_UTF16:
		CopyStringEnc(out, max, in, enc);
		return;
	case ENC_CP1252:
		max --;
		while (*src && max > 0)
		{
			if (0xD800 <= *src && *src <= 0xDBFF)
			{
				*out = '?';
				src ++;
			}
			else if (*src > 255)
			{
				/* Extended chars from cp1252 */
				for (nb = 0; nb < DIM(cp1252) && cp1252[nb] != *src; nb ++);
				*out = nb < DIM(cp1252) ? 128 + nb : '?';
			}
			else *out = *src;
			src ++; out ++; max --;
		}
		*out = 0;
		break;
	case ENC_UTF8:
		/* First: compute length of UTF-8 string */
		max --;
		while (*src)
		{
			int cp = *src;
			int n  = 0;
			if (0xD800 <= cp && cp <= 0xDBFF && src[1]) /* Second surrogate pair */
			{
				cp = (((cp & 0x3ff) << 10) | (src[1] & 0x3ff)) + 0x10000;
				src ++;
			}
			if (cp <= 0x00007f) n = 1; else
			if (cp <= 0x0007ff) n = 2; else
			if (cp <= 0x00ffff) n = 3; else
			if (cp <= 0x10ffff) n = 4;

			if (max < n) break;

			if (cp <= 0x7F) {
			   *out++ = cp;
			} else if (cp <= 0x7FF) {
			   *out++ = 0xC0 | ((cp >> 6) & 0x1F);
			   *out++ = 0x80 | (cp & 0x3F);
			} else if (cp <= 0xFFFF) {
			   *out++ = 0xE0 | ((cp >> 12) & 0x0F);
			   *out++ = 0x80 | ((cp >> 6) & 0x3F);
			   *out++ = 0x80 | (cp & 0x3F);
			} else if (cp <= 0x10FFFF) {
			   *out++ = 0xF0 | ((cp >> 18) & 0x07);
			   *out++ = 0x80 | ((cp >> 12) & 0x3F);
			   *out++ = 0x80 | ((cp >> 6)  & 0x3F);
			   *out++ = 0x80 | (cp & 0x3F);
			}
			src ++; max -= n;
		}
		*out = 0;
	}
}


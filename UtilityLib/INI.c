/*
 * INI.c : functions to read/write INI files
 *
 * Written by T.Pierron, Jan 2008.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "SWL.h"


/**h* SWL/INI
 * INTRODUCTION
 *	This module helps parsing and writing ini file. This are simple text files with
 *	key/value pairs, possibly preceded by a header enclosed by [].
 *
 *	Following syntax are recognized for key/value :
 *		Key = value ; comment
 *		Key = "long value" ; possible comment
 *
 *	The key is any string (possibly containing blanks). The value is any character
 *	on the right side of the equal sign. Values can be given enclosed with quotes.
 *	If no quotes are present, the value is understood as containing all characters
 *	between the first and the last non-blank characters. The following declarations
 *	are identical:
 *
 *		Key = "this is a long string value" ; comment
 *		Key = this is a long string value ; comment
 *
 *	This module is suitable for managing a file containing around 1000 key/value
 *	pairs. Above that, consider using something more appropriate, like a real
 *	database engine, e.g. SQLite.
 */

/* Type is private */
struct INIFile_t
{
	STRPTR   buffer;
	STRPTR * curkey;
	int      size;
	STRPTR   keys[1];
};

/**f* INI/ParseINI, INI/ParseINIStr
 * NAME
 *	ParseINI    - Parse a .INI file, so that you can get values later.
 *	ParseINIStr - Parse a pre-extracted INI string.
 *
 * SYNOPSIS
 *	INIFile ParseINI(STRPTR file);
 *	INIFile ParseINIStr(STRPTR buffer);
 *
 * FUNCTION
 *	Check module introduction for expected format of INI file or string.
 *
 * INPUT
 *	buffer - INI file to parse, must be 0-terminated, modifiable and have a
 *	         lifespan that equals at least the INI object.
 *
 * SEE ALSO
 *	GetINIValue, SetINIValue, FreeINI, IterINI
 */
static INIFile AllocINI(int keys, STRPTR buf)
{
	INIFile ini = malloc(sizeof *ini + (keys + 1) * sizeof (STRPTR));

	ini->curkey = NULL;
	ini->buffer = buf;
	ini->size   = strlen(buf);

	return ini;
}

DLLIMP INIFile ParseINI(STRPTR file)
{
	FILE *  in;
	int     sz = FileSize(file);
	STRPTR  buf;

	if (sz > 0)
	{
		buf = malloc(sz + 1);

		if (buf)
		{
			in = fopen(file, "rb");

			if (in)
			{
				if (fread(buf, 1, sz, in) == sz)
				{
					fclose(in);
					buf[sz] = 0;
					return ParseINIStr(buf);
				}
				else fclose(in);
			}
		}
	}
	return NULL;
}

static STRPTR ParseINILine(STRPTR line)
{
	STRPTR value = strchr(line, '=');
	STRPTR prev  = value - 1;

	StripCRLF(line);

	if (value == NULL || strchr("#;", *line)) return NULL;

	while (prev > line && isspace(*prev)) prev --; prev[1] = 0;
	for (value ++; *value && isspace(*value); value ++);

	if (*value == '\"')
	{
		value ++;
		for (prev = value; prev[0] != '\"' || prev[-1] == '\\'; prev ++)
		{
			if (prev[0] == '\\')
				strcpy(prev, prev + 1);
		}
		if (prev) *prev = 0;
	}
	else /* Check for trailing spaces or comment */
	{
		prev = strchr(value, ';');
		if (prev == NULL) prev = strchr(value, 0);
		for (prev --; prev > value && isspace(*prev); prev --);
		prev[1] = 0;
	}
	return value;
}

DLLIMP INIFile ParseINIStr(STRPTR buf)
{
	INIFile ini = AllocINI(StrCount(buf, '\n') * 2, buf);
	int     i   = 0;

	while (buf)
	{
		STRPTR next  = strchr(buf, '\n'); if (next) *next++ = 0;
		STRPTR value = ParseINILine(buf);

		if (value)
		{
			/* Key/value pair */
			ini->keys[i] = buf;   i ++;
			ini->keys[i] = value; i ++;
		}
		else if (buf[0] == '[' && (value = strrchr(buf, ']')))
		{
			/* Section header */
			ini->keys[i] = buf + 1; i ++;
			ini->keys[i] = NULL;    i ++;
			*value = 0;
		}
		buf = next;
	}
	ini->keys[i] = NULL;

	return ini;
}

/**f* INI/FreeINI
 * NAME
 *	FreeINI - Free memory allocated for parsing ini file.
 *
 * SYNOPSIS
 *	void FreeINI(INIFile ini);
 */
DLLIMP void FreeINI(INIFile ini)
{
	if (ini)
	{
		if (ini->buffer) free(ini->buffer);
		free(ini);
	}
}

/**f* INI/IterINI
 * NAME
 *	IterINI - let you iterate over key, value and header of an INI file.
 *
 * SYNOPSIS
 *	Bool IterINI(INIFile ini, STRPTR * key, STRPTR * value);
 *
 * FUNCTION
 *	Let you get each key/value pairs or headers, in the order they were found
 *	in the file.
 *
 *	When file has been newly parsed, iterator point to first key. You can then
 *	start to iterate over keys/values. Once this function will return False, the
 *	iterator will be reset to the first key, so that you can scan them again if
 *	you wish.
 *
 * RESULT
 *	"key" and "value" will be set to current key/value found in file. If value
 *	is NULL (and not an empty string), key is actually a header name.
 *
 *	True will be returned if you can start examining key/value arg, or False if
 *	there was no more entries.
 */
DLLIMP Bool IterINI(INIFile ini, STRPTR * key, STRPTR * value)
{
	if (ini == NULL) return False;
	if (ini->keys[0])
	{
		if (ini->curkey == NULL)
		{
			*key   = ini->keys[0];
			*value = ini->keys[1];
			ini->curkey = ini->keys + 2;
		}
		else if (ini->curkey[0])
		{
			*key   = ini->curkey[0];
			*value = ini->curkey[1];
			ini->curkey += 2;
		}
		else /* End of file */
		{
			ini->curkey = NULL;
			return False;
		}
		return True;
	}
	else ini->curkey = NULL;
	return False;
}

DLLIMP void IterINIReset(INIFile ini)
{
	if (ini) ini->curkey = NULL;
}

/**f* INI/GetINIValue, INI/GetINIValueInt, INI/GetINIValueStr
 * NAME
 *	GetINIValue - Get a value from an INI, possibly in a specific section.
 *	GetINIValueInt - Get a decimal value, with default if nothing found.
 *	GetINIValueStr - Get a string value, with default if nothing found.
 *
 * SYNOPSIS
 *	STRPTR GetINIValue(INIFile ini, STRPTR key);
 *	int GetINIValueInt(INIFile ini, STRPTR key, int def);
 *	STRPTR GetINIValueStr(INIFile ini, STRPTR key, STRPTR def);
 *
 * FUNCTION
 *	Get value value associated to key in the given ini file. Key can preceded
 *	by section name, separated by /.
 *
 * INPUT
 *	ini - valid reference to an ini file.
 *	key - key to search for it's value. Can be eitheir a simple key name or
 *	      in the form "section/key" to search the key only in the given section.
 *	      If no section are provided, first key to match will be taken.
 *
 * RESULT
 *	If found, the string associated to the key, or NULL if key does not exist.
 *
 * SEE ALSO
 *	ParseINI, SetINIValue
 */
DLLIMP STRPTR GetINIValue(INIFile ini, STRPTR section)
{
	STRPTR sub, key, value;
	int    length;
	Bool   sec;

	if (ini == NULL) return NULL;

	sub = strchr(section, '/');
	sec = (sub == NULL);
	if (sub) length = sub - section, sub ++;
	else     length = 0;

	while (IterINI(ini, &key, &value))
	{
		if (sub && value == NULL)
		{
			sec = strncasecmp(section, key, length) == 0 && key[length] == 0;
		}
		else if (value && sec && strcasecmp(key, sub ? sub : section) == 0)
		{
			ini->curkey = NULL;
			return value;
		}
	}
	return NULL;
}

DLLIMP int GetINIValueInt(INIFile ini, STRPTR key, int def)
{
	STRPTR value = GetINIValue(ini, key);

	if (value)
	{
		long val = strtol(value, &key, 10);

		if (key > value) return val;
	}
	return def;
}

DLLIMP STRPTR GetINIValueStr(INIFile ini, STRPTR key, STRPTR def)
{
	STRPTR value = GetINIValue(ini, key);

	return value ? value : def;
}

/**f* INI/SetINIValue, INI/SetINIValueInt
 * NAME
 *	SetINIValue - Change or add a key/value pair in an INI file.
 *	SetINIValueInt - Like SetINIValue, but takes an integer.
 *
 * SYNOPSIS
 *	Bool SetINIValue(STRPTR path, STRPTR key, STRPTR val);
 *	Bool SetINIValueInt(STRPTR path, STRPTR key, int val);
 *
 * FUNCTION
 *	This function will modify value of given key, or add it if it does not
 *	exist. "key" can have the same syntax than GetINIValue. If section is also
 *	missing, it will be added as well.
 *
 *	This function will try it's best to do minimal changes to your file. Changing
 *	only what's required. Thus if value has a comment following, it will not be
 *	removed, nor altered. Actually if the new value is the same than the one
 *	currently stored, nothing will be modified at all.
 *
 *	Notice that this function does not work with INIFile object, but will deal
 *	directly with the INI file itself.
 *
 * INPUT
 *	path - path to the file where you want it's key to be modified.
 *	key  - key to search, check GetINIValue for full syntax.
 *	val  - the new value for the key.
 *
 * RESULT
 *	True if value has been modified or added (possibly with key/section). False
 *	if something failed.
 *
 * SEE ALSO
 *	GetINIValue
 */
static Bool HasQuotes(STRPTR value)
{
	if (isspace(value[0]) || strchr(value, ';')) return True;

	STRPTR end = strchr(value, 0) - 1;

	return end >= value && isspace(*end);
}

DLLIMP Bool SetINIValue(STRPTR path, STRPTR key, STRPTR val)
{
	INIFile ini = ParseINI(path);
	STRPTR  sub = strchr(key, '/');
	Bool    esc = HasQuotes(val);
	FILE *  fh;
	int     lg;

	if (ini)
	{
		STRPTR value = GetINIValue(ini, key);
		int    flags = 0;

		if (value)
		{
			/* Key exists in file, overwrite value */
			if (strcmp(val, value) == 0)
			{
				/* Same value than currently stored : don't change anything */
				FreeINI(ini);
				return True;
			}
			flags = 2;
		}
		else if (sub) /* File exists but key do not : check if section exists */
		{
			STRPTR section;
			lg = sub - key;
			while (IterINI(ini, &section, &value))
			{
				if (value == NULL && strncasecmp(section, key, lg) == 0 && section[lg] == 0)
					break;
			}

			if (ini->curkey) /* Section exists, only add key/value after it */
			{
				value = ini->curkey[0];
				flags = 3;
			}
			else /* Add section, key and value at end of file */
				flags = 7, value = ini->buffer + ini->size;
		}
		else /* No section, only key name : add ket/value at beginning of file */
			flags = 3, value = ini->buffer;

		if (value == NULL)
		{
			/* No insertion point ? */
			FreeINI(ini);
			return False;
		}
		else if (flags > 0)
		{
			fh = fopen(path, "rb+");
			lg = strlen(value);

			if (fh == NULL) { FreeINI(ini); return False; }

			/* Optimize if only key has changed and same length */
			if (flags == 2 && strlen(val) == lg && (esc == False || value[-1] == '\"'))
			{
				fseek(fh, value - ini->buffer, SEEK_SET);
				fwrite(val, 1, strlen(val), fh);
			}
			else /* Need to shift file */
			{
				int diff = value - ini->buffer + (flags == 2 ? lg : 0);

				fseek(fh, diff, SEEK_SET);                 /* Set pointer at the end of old key */
				lg = fread(ini->buffer, 1, ini->size, fh); /* Read all up to the end */
				fseek(fh, value - ini->buffer, SEEK_SET);  /* Start of insertion point */
				if (flags & 4)
				{
					/* Add section name */
					fseek(fh, -1, SEEK_CUR);
					diff = fgetc(fh);
					fseek(fh, value - ini->buffer, SEEK_SET);
					if (strchr("\r\n", diff) == NULL) fputs("\r\n", fh);
					fputc('[', fh);
					fwrite(key, 1, sub - key, fh);
					fputs("]\r\n", fh);
				}
				if (flags & 1) fprintf(fh, "%s=", sub ? sub + 1 : key);
				sub = (esc ? "\"" : "");
				fprintf(fh, "%s%s%s%s", sub, val, sub, flags == 2 ? "" : "\r\n");
				fwrite(ini->buffer, 1, lg, fh);  /* Overwrite remaining */
				ftruncate(fileno(fh), ftell(fh));
			}
			fclose(fh);
		}
		FreeINI(ini);
		return True;
	}
	else /* File does not exist yet */
	{
		fh = fopen(path, "a");

		if (sub)
		{
			/* Add section/key/value */
			fputc('[', fh);
			fwrite(key, 1, sub - key, fh);
			fputs("]\r\n", fh);
			key = sub + 1;
		}
		sub = (esc ? "\"" : "");
		fprintf(fh, "%s=%s%s%s\r\n", key, sub, val, sub);
		fclose(fh);
	}
	return True;
}

DLLIMP Bool SetINIValueInt(STRPTR path, STRPTR key, int val)
{
	TEXT number[32];

	snprintf(number, sizeof number, "%d", val);

	return SetINIValue(path, key, number);
}

/*
 * Lang.c : manage gettext po translation files.
 *
 * Written by T.Pierron, Mar 2008.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SWL.h"


struct Lang_t
{
	ListNode  node;
	TEXT      name[32];
	HashTable strings;
	STRPTR    buffer;
	STRPTR    file;
	ULONG     ts;
};

static ListHead langs = {0};

static STRPTR LangExtractStr(STRPTR old, STRPTR buffer);

DLLIMP Lang LangAdd(STRPTR name, STRPTR path)
{
	STRPTR msgid  = NULL;
	STRPTR msgstr = NULL;
	FILE * in     = NULL;
	STRPTR line, eol;
	Lang   lang;
	int    sz;

	in = fopen(path, "r");

	if (in   == NULL) return NULL;
	if (name == NULL) name = "";

	for (lang = HEAD(langs); lang && strcasecmp(lang->name, name); NEXT(lang));
	if (lang) LangFree(lang);

	sz   = FileSize(path);
	lang = malloc(sizeof *lang + sz + strlen(path) + 2);

	lang->strings = HashCreate(100, HashStr, HashStrCmp);
	lang->buffer  = (STRPTR) (lang + 1);
	lang->file    = lang->buffer + sz + 1;
	lang->ts      = TimeStamp(path, 2);

	lang->buffer[sz] = 0;
	fread(lang->buffer, 1, sz, in);
	fclose(in);
	strcpy(lang->file, path);
	CopyString(lang->name, name, sizeof lang->name);
	ListAddTail(&langs, &lang->node);

	for (line = lang->buffer; line; line = eol)
	{
		for (eol = line; *eol && *eol != '\n' && *eol != '\r'; eol ++);
		for ( ; *eol == '\r' || *eol == '\n'; *eol ++);
		if (*eol) eol[-1] = 0; else eol = NULL;

		if (*line == '#') continue;

		if (strncasecmp(line, "msgid ", 6) == 0)
		{
			if (IsDef(msgstr) && IsDef(msgid))
			{
				/* print "adding \"$msgid\" => \"$msgstr\"\n"; */
				HashInsert(lang->strings, msgid, msgstr);
			}
			msgstr = msgid = NULL;
			msgid  = LangExtractStr(msgid, line);
		}
		else if (strncasecmp(line, "msgstr ", 7) == 0)
		{
			msgstr = LangExtractStr(msgstr, line);
		}
		else if (msgstr)
		{
			msgstr = LangExtractStr(msgstr, line);
		}
		else if (msgid)
		{
			/* continuation of msg ID */
			msgid = LangExtractStr(msgid, line);
		}
	}

	if (IsDef(msgstr) && IsDef(msgid))
	{
		HashInsert(lang->strings, msgid, msgstr);
	}
	return lang;
}

static STRPTR LangExtractStr(STRPTR old, STRPTR buffer)
{
	STRPTR quote = strchr(buffer, '\"');
	STRPTR end;

	if (quote == NULL) return old;

	for (end = ++ quote; *end && *end != '\"'; end ++)
	{
		if (*end == '\\')
		{
			switch (end[1]) {
			case 'n':  *end = '\n'; break;
			case 'r':  *end = '\r'; break;
			case 'v':  *end = '\v'; break;
			case 't':  *end = '\t'; break;
			case '\\': *end = '\\'; break;
			case '\"': *end = '\"'; break;
			}
			strcpy(end + 1, end + 2);
		}
	}
	*end = 0;
	if (old)
	{
		memmove(strchr(old, 0), quote, strlen(quote) + 1);
		return old;
	}
	return quote;
}

DLLIMP STRPTR LangStr(Lang lang, STRPTR msg, int max)
{
	STRPTR trans = NULL;

	if (lang == NULL)
		lang = HEAD(langs);

	if (lang && HashCount(lang->strings) > 0)
	{
		trans = HashSearch(lang->strings, msg);

		/* Hmmm, hack -- but whatever ... */
		if (trans == NULL && max > 0)
		{
			if (isupper(msg[0]))
			{
				msg[0] = tolower(msg[0]);
				trans  = HashSearch(lang->strings, msg);

				if (trans) CopyString(msg, trans, max);

				msg[0] = toupper(msg[0]);
			}
			else if (islower(msg[0]))
			{
				msg[0] = toupper(msg[0]);
				trans  = HashSearch(lang->strings, msg);

				if (trans) CopyString(msg, trans, max);

				msg[0] = tolower(msg[0]);
			}
			return msg;
		}
	}
	return trans ? trans : msg;
}

// XXX Hmm need to be improved
DLLIMP STRPTR LangStrPlural(Lang lang, int nb, STRPTR sing, STRPTR plur)
{
	if (nb == 1) return LangStr(lang, sing, 0);
	else         return LangStr(lang, plur, 0);
}

DLLIMP void LangFree(Lang lang)
{
	if (lang)
	{
		ListRemove(&langs, &lang->node);
		HashDestroy(lang->strings, False);
		free(lang);
	}
}

DLLIMP Lang LangSet(STRPTR root, STRPTR name)
{
	TEXT path[256];

	while (langs.lh_Count > 0)
		LangFree(HEAD(langs));

	if (! IsDef(name)) return NULL; // Use build-in strings

	CopyString(path, root, sizeof path);
	AddPart(path, "lang", sizeof path);
	AddPart(path, name, sizeof path);
	AddExt(path, "po", sizeof path, False);

	return LangAdd(name, path);
}

/* Check if language file has been modified. If yes, reload it */
DLLIMP Lang LangCheck(STRPTR root, Lang lang)
{
	if (lang == NULL) lang = HEAD(langs);

	if (lang && lang->ts != TimeStamp(lang->file, 2))
	{
		Lang update;
		ListRemove(&langs, &lang->node);
		update = LangAdd(lang->name, lang->file);
		ListAddTail(&langs, &lang->node);
		LangFree(lang);
		lang = update;
	}
	return lang;
}

/*
 * Core.c : entry point of library (initialization) + unit test for Debug build.
 *
 * Written by T.Pierron, Jan 2008.
 */

#include "SWL.h"
#include <time.h>
#include <winsock.h>
#include <fcntl.h>
#include <time.h>

/**h* SWL/TYPES
 * INTRODUCTION
 *	These are the basic datatypes used by functions of this library:
 *	- TEXT    : character
 *	- STRPTR  : NULL-terminated string.
 *	- APTR    : void pointer.
 *	- data8_t : 8bit buffer pointer.
 *	- ULONG   : unsigned long integer.
 *	- LONG    : singed long integer.
 *
 *	For fixed-size types, look in <stdint.h>.
 */

static STRPTR appName = NULL;

/**f* SWL/UtilityInit
 * NAME
 *	UtilityInit - do some initial work for some part of the library.
 *
 * SYNOPSIS
 *	void UtilityInit(STRPTR app);
 *
 * FUNCTION
 *	Some functions require to be initialized before being able to call them
 *	(especially network-oriented functions). Some modules obviously don't
 *	require any initialization (List, String, DOS, CGI, Thread, DFA, Hash,
 *	...), and it'll never hurt if you don't call this function if you plan
 *	to just use that kind of modules.
 *
 *	srand() will be initialized with current time stamp.
 *
 * INPUT
 *	- app : name of application that will be used in a few places (list below).
 *
 * SEE ALSO
 *	LOG_Info
 */
DLLIMP void UtilityInit(STRPTR app)
{
	setmode(1, O_BINARY);
	setmode(2, O_BINARY);
	appName = app;
	srand(time(NULL));
}

static void LOG_MsgA(STRPTR file, STRPTR fmt, va_list args)
{
	FILE * out = fopen(file, "a");

	if (out)
	{
		time_t epoch = time(NULL);
		STRPTR now   = ctime(&epoch);

		StripCRLF(now);
		fprintf(out, "%s: ", now);
		vfprintf(out, fmt, args);
		fclose(out);
	}
}

/**f* SWL/LOG_Info, SWL/LOG_InfoA
 * NAME
 *	LOG_Info  - Log a string into a log file.
 *	LOG_InfoA - va_list-based log function, instead of vararg.
 *
 * SYNOPSIS
 *	void LOG_Info(STRPTR fmt, ...);
 *	void LOG_InfoA(STRPTR fmt, va_list args);
 *
 * FUNCTION
 *	These functions take a printf()-like format and will append the resulting
 *	string, in the file "../logs/<appname>.log". If directory does not exists,
 *	the string will be silently discarded.
 *
 *	UtilityInit() must have been called, otherwise message will also be silently
 *	discarded.
 *
 * SEE ALSO
 *	UtilityInit
 */
DLLIMP void LOG_InfoA(STRPTR fmt, va_list args)
{
	TEXT path[128];

	if (appName)
	{
		snprintf(path, sizeof path, "../logs/%s.log", appName);
		LOG_MsgA(path, fmt, args);
	}
}

DLLIMP void LOG_Info(STRPTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	LOG_InfoA(fmt, args);
	va_end(args);
}

/*
 * Unit test : check that functions work as expected. Contains test for List.c,
 * String.c, SHA1.c, DOS.c, HashTable.c, Thread.c
 */
#ifdef	CRASH_TEST
#include <assert.h>
#include "Debug.h"

Mutex m;

int main(int nb, char * argv[])
{
	TEXT   path[1024];
	TEXT   buffer[32];
	STRPTR str = NULL;
	FILE * out;
	int    i, j;

	UtilityInit("CrashTest");
	FilenameEncoding = ENC_UTF8;

	/*
	 * List.c
	 */
	ListHead head;
	ListItem item1, item2, item3;

	memset(&head, 0, sizeof head);
	item1.value = 1;
	item2.value = 2;
	item3.value = 3;

	/* Check ListInsert */
	ListInsert(&head, &item1.node, NULL);
	ListInsert(&head, &item3.node, &item1.node);
	ListInsert(&head, &item2.node, &item1.node);

	assert(CheckList(&head) == 1);

	/* Check AddHead */
	memset(&head, 0, sizeof head);

	ListAddHead(&head, &item3.node);
	ListAddHead(&head, &item2.node);
	ListAddHead(&head, &item1.node);

	assert(CheckList(&head) == 1);

	/* Check AddTail */
	memset(&head, 0, sizeof head);

	ListAddTail(&head, &item1.node);
	ListAddTail(&head, &item2.node);
	ListAddTail(&head, &item3.node);

	assert(CheckList(&head) == 1);

	/* Check ListSortItem */
	memset(&head, 0, sizeof head);

	ListInsertSort(&head, &item3.node, CompareItem);
	ListInsertSort(&head, &item1.node, CompareItem);
	ListInsertSort(&head, &item2.node, CompareItem);

	assert(CheckList(&head) == 1);

	/* Check ListSort */
	ListSort(&head, CompareItem);

	assert(CheckList(&head) == 1);

	/* Check ListRemove */
	ListRemove(&head, &item2.node);
	ListRemove(&head, &item3.node);
	ListRemove(&head, &item1.node);

	assert(head.lh_Tail == NULL && head.lh_Head == NULL && head.lh_Count == 0);

	/* Check RemTail */
	ListAddTail(&head, &item1.node);
	ListAddTail(&head, &item2.node);
	ListAddTail(&head, &item3.node);

	assert(ListRemTail(&head) == &item3.node);
	assert(ListRemTail(&head) == &item2.node);
	assert(ListRemTail(&head) == &item1.node);
	assert(ListRemTail(&head) == NULL);

	assert(head.lh_Tail == NULL && head.lh_Head == NULL && head.lh_Count == 0);

	printf("List test passed\n");

	/*
	 * String.c
	 */

	/* Check overflow */
	CopyString(buffer, "This is a 35 characters long string", sizeof buffer);

	assert(strcmp(buffer, "This is a 35 characters long st") == 0);

	/* NULL with zero size (crash is the assert :-) */
	CopyString(NULL, "XXX", 0);

	/* Common case */
	buffer[0] = 0;
	StrCatM(buffer, sizeof buffer, ' ', "Hello", "world", "!", NULL);

	assert(strcmp(buffer, "Hello world !") == 0);

	/* Check overflow */
	buffer[0] = 0;
	StrCatM(buffer, sizeof buffer, ' ', "This", "is", "a", "35",
		"characters", "long", "string", NULL);

	assert(strcmp(buffer, "This is a 35 characters long st") == 0);

	/* Check NULL with zero size */
	StrCatM(NULL, 0, -1, "Should not crash", NULL);

	/* Check existing string */
	strcpy(buffer, "Hello");

	StrCatM(buffer, sizeof buffer, -1, " world", " !", NULL);

	assert(strcmp(buffer, "Hello world !") == 0);

	/* Check overflow */
	strcpy(buffer, "Hello");

	StrCatM(buffer, sizeof buffer, ' ', "world", "!", "Should", "not",
		"overrun", "buffer", NULL);

	assert(strcmp(buffer, "Hello world ! Should not overru") == 0);

	/* Check empty string */
	buffer[0] = 0;
	StrCatM(buffer, sizeof buffer, '\t', "arg1", "value1", "arg2", "value2",
		"arg3", "", "arg4\t", "???", NULL);

	assert(strcmp(buffer, "arg1\tvalue1\targ2\tvalue2\targ3\t\ta") == 0);

	/* Check malloc()'ed string */
	assert(StrCatDup(&str, '\t', "arg1", "value1", "arg2", "value2", "arg3",
		"", "arg4\t", "???", NULL));

	assert(strcmp(str, "arg1\tvalue1\targ2\tvalue2\targ3\t\targ4\t???") == 0);
	free(str);

	/* FindInList */
	assert(FindInList("apple,orange,pear", "orange", 0) == 1);
	assert(FindInList("apple,orange,pear,oran", "orange", 4) == 3);
	assert(FindInList("apple,orange,pear", "oran", 0) < 0);

	/* StrLenM, StrCount, CharInSet */
	assert(StrLenM("abc", "", "def", NULL) == 6);
	assert(StrCount("abcdeee", 'e') == 3);
	assert(CharInSet("TemplateName\t%N-%J", "\t%") == 3);

	/* StripCRLF */
	strcpy(buffer, "test\n");   assert(strcmp(StripCRLF(buffer), "test") == 0);
	strcpy(buffer, "test\r\n"); assert(strcmp(StripCRLF(buffer), "test") == 0);
	strcpy(buffer, "test\r");   assert(strcmp(StripCRLF(buffer), "test") == 0);

	/* FnMatch */
	strcpy(buffer, "PaTh-abc/xyz.txt");
	assert(FnMatch("*-abc*", buffer, 0));
	assert(FnMatch("*",      buffer, 0));
	assert(FnMatch("*/*",    buffer, 0));
	assert(FnMatch("pAtH-*", buffer, FNM_CASEFOLD));

	printf("String test passed\n");

	/*
	 * Calc.c
	 */
	VariantBuf v;

	#define	almostequal(val, equal)			(equal - 0.000001 <= val && val <= equal + 0.000001)

	i = ParseExpression("2 + 2", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 4);

	i = ParseExpression("2 + 2 * 3 << 1 | 0x80", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 144);

	i = ParseExpression("(2 + 2) * 3 << 1", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 24);

	i = ParseExpression("2 == 1", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 0);

	i = ParseExpression("1 == 1", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 1);

	i = ParseExpression("(1 == 2 ? (a == 0 ? 1/0 : 314.15e-2) : 3) + 2", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 5);

	i = ParseExpression("2 || 5 / 0", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 1);

	i = ParseExpression("(0 && 1 / 0) + 2", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 2);

	i = ParseExpression("- 1 - 1", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == -2);

	i = ParseExpression("3 / 2.", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_DBL && almostequal(v.v.r, 1.5));

	i = ParseExpression("3 / 2. << 1", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 2);

	i = ParseExpression("++ a", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 3);

	memset(&v, 0, sizeof v);
	i = ParseExpression("a+++2", CheckRes, &v);
	assert(i == 0 && v.type == TYPE_INT && v.v.z == 3);

	assert(ParseExpression("1 +", CheckRes, &v) == PERR_MissingOperand);

	assert(ParseExpression("2 = 1", CheckRes, &v) == PERR_LValueNotModifiable);

	printf("Calc test passed\n");

	/*
	 * Encodings.c
	 */

	/* Four Hangul characters, and 1 char outside BMP */
	static wchar_t utf16_eqv1[] = {0xD68C, 0xC0AC, 0xC18C, 0xAC1C, 0xD834, 0xDD1E, 0};
	static wchar_t utf16_eqv2[] = L"180\x20AC b\x0153" "uf safe\x2122 1000\x2030";
	static char    utf8_eqv1[]  = "\xED\x9A\x8C\xEC\x82\xAC\xEC\x86\x8C\xEA\xB0\x9C\xF0\x9D\x84\x9E";
	static char    cp1252_eqv[] = "180\x80 b\x9c" "uf safe\x99 1000\x89";

	assert(CopyStringEnc(buffer, sizeof buffer, utf8_eqv1, ENC_UTF8) == 5);
	assert(memcmp(buffer, utf16_eqv1, sizeof utf16_eqv1) == 0);

	ConvertEnc(path, sizeof path, buffer, ENC_UTF8);
	assert(memcmp(path, utf8_eqv1, sizeof utf8_eqv1) == 0);

	/* Limit size, should not produce incomplete character */
	memcpy(path, utf16_eqv1, sizeof utf16_eqv1);
	ConvertEnc(buffer, 15, path, ENC_UTF8);
	assert(memcmp(buffer, utf8_eqv1, 12) == 0 && buffer[12] == 0);

	assert(CopyStringEnc(path, 5 * sizeof (wchar_t), (STRPTR) utf16_eqv1, ENC_UTF16) == 4);
	assert(memcmp(path, utf16_eqv1, 4 * sizeof (wchar_t)) == 0);

	assert(NthChar(utf8_eqv1, 5, ENC_UTF8) == strchr(utf8_eqv1, 0));
	assert(NthChar(utf8_eqv1, 5, ENC_UTF8) - NthChar(utf8_eqv1, 4, ENC_UTF8) == 4);
	assert(NthChar((STRPTR) utf16_eqv1, 5, ENC_UTF16) == (STRPTR) (utf16_eqv1 + 6));

	str = StrDupEnc(utf8_eqv1, ENC_UTF8);
	assert(memcmp(str, utf16_eqv1, sizeof utf16_eqv1) == 0);
	free(str);

	str = StrDupEnc((STRPTR) utf16_eqv1, ENC_UTF16);
	assert(memcmp(str, utf16_eqv1, sizeof utf16_eqv1) == 0);
	free(str);

	/* CP1252 tests */
	CopyStringEnc(path, sizeof path, cp1252_eqv, ENC_CP1252);
	assert(memcmp(path, utf16_eqv2, sizeof utf16_eqv2) == 0);

	ConvertEnc(buffer, sizeof buffer, path, ENC_CP1252);
	assert(memcmp(buffer, cp1252_eqv, sizeof cp1252_eqv) == 0);

	printf("Encodings test passed\n");

	/*
	 * DOS.c
	 */

	/* AddPart */
	strcpy(path, "C:/Program files");
	AddPart(path, "Somewhere", sizeof path);
	assert(strcmp(path, "C:/Program files/Somewhere") == 0);

	strcpy(path, "C:/Temp");
	AddPart(path, "../Windows/System32", sizeof path);
	assert(strcmp(path, "C:/Windows/System32") == 0);

	strcpy(path, "C:/Temp");
	AddPart(path, "/Windows/System32", sizeof path);
	assert(strcmp(path, "C:/Windows/System32") == 0);

	strcpy(path, "C:/Temp");
	AddPart(path, "D:/My documents", sizeof path);
	assert(strcmp(path, "D:/My documents") == 0);

	strcpy(path, "Folder");
	AddPart(path, "Sub folder", sizeof path);
	assert(strcmp(path, "Folder\\Sub folder") == 0);

	strcpy(path, "Folder");
	AddPart(path, "Sub folder/Even deeper", sizeof path);
	assert(strcmp(path, "Folder/Sub folder/Even deeper") == 0);

	strcpy(path, "Folder");
	AddPart(path, "../../Sub folder/Even deeper", sizeof path);
	assert(strcmp(path, "../Sub folder/Even deeper") == 0);

	strcpy(path, "Folder");
	AddPart(path, "Sub folder\\..\\Even deeper", sizeof path);
	assert(strcmp(path, "Folder\\Even deeper") == 0);
	AddPart(path, "..", sizeof path);
	assert(strcmp(path, "Folder") == 0);
	AddPart(path, "..", sizeof path);
	assert(strcmp(path, "") == 0);

	/* CatPath */
	str = CatPath("../sub/dir", "../sub/level/", "file", NULL);

	assert(strcmp(str, "../sub/sub/level/file") == 0);

	/* ParentDir - AddPart use it but let's be sure */
	strcpy(path, "C:/Program files/Common files/");
	ParentDir(path);
	assert(strcmp(path, "C:/Program files") == 0);

	ParentDir(path);
	assert(strcmp(path, "C:") == 0);
	assert(ParentDir(path) == False);

	strcpy(path, "Folder/Sub");
	ParentDir(path);
	assert(strcmp(path, "Folder") == 0);
	ParentDir(path);
	assert(strcmp(path, "") == 0);

	/* SafePath */
	strcpy(path, "*** Hello world ! ***");
	SafePath(path);
	assert(strcmp(path, "Hello world !") == 0);

	/* BaseName */
	assert(strcmp(BaseName("C:\\Program files\\XYZ corp"), "XYZ corp") == 0);
	assert(strcmp(BaseName("Test file.txt"), "Test file.txt") == 0);
	assert(strcmp(BaseName("C:\\Temp\\"), "") == 0);

	/* GetExtension */
	assert(strcmp(GetExtension("C:\\Test\\file.txt"), "txt") == 0);
	assert(strcmp(GetExtension("C:\\Test\\file"), "") == 0);
	assert(strcmp(GetExtension("C:/Test.2/file"), "") == 0);
	assert(strcmp(GetExtension("C:/Test.2/file.3"), "3") == 0);

	/* AddExt */
	strcpy(path, "C:/Myfile.txt");
	AddExt(path, ".tif", sizeof path, True);
	assert(strcmp(path, "C:/Myfile.tif") == 0);

	strcpy(path, "C:/Myfile");
	AddExt(path, "tif", sizeof path, True);
	assert(strcmp(path, "C:/Myfile.tif") == 0);

	strcpy(path, "C:/Myfile");
	AddExt(path, "tif", 12, True);
	assert(strcmp(path, "C:/Myfile.t") == 0);

	strcpy(path, "C:/Myfile.tif");
	AddExt(path, "txt", 4, True);
	assert(strcmp(path, "C:/Myfile.tif") == 0);

	AddExt(path, "txt", sizeof path, False);
	assert(strcmp(path, "C:/Myfile.tif.txt") == 0);

	AddExt(path, "", sizeof path, True);
	assert(strcmp(path, "C:/Myfile.tif") == 0);

	AddExt(path, "", sizeof path, False);
	assert(strcmp(path, "C:/Myfile.tif") == 0);

	strcpy(path, "E:/public/temp/Hi-Res/cente/dust-center-mw.jpg");
	AddExt(path, ".tif", sizeof path, False);

	/* CreatePath, DeletePath, IsDir, DeleteDOS, ScanDir */
	assert(IsDir(".") == True);

	// Hmmm test will fail if share is not activated
	assert(CreatePath("\\\\127.0.0.1\\C$\\taiste", False) == NULL);
	assert(DeleteDOS("\\\\127.0.0.1\\C$\\taiste"));
	assert(CreatePath("/taiste/foo/bar", False) == NULL);
	assert(DeletePath("\\taiste"));

	if (! IsDir("../test"))
	{
		assert(CreatePath("..\\test", False) == NULL);
		assert(IsDir("..\\test"));
	}
	fclose(fopen("../test/myfile", "w"));

	strcpy(path, "../test/\xED\x9A\x8C\xEC\x82\xAC\xEC\x86\x8C\xEA\xB0\x9C.pdf");

	File io = FILE_Open(path, FILE_IO_WRITE);

	assert(io);
	FILE_Write(io, "Hello world !\n", 15);
	FILE_Close(io);

	/* Check NTFS extended attributes layer */
	StrCat(path, ":PolkadotsSummary", sizeof path);

	io = FILE_Open(path, FILE_IO_WRITE);
	assert(io);
	FILE_Write(io, "Hidden layer of the file", 25);
	FILE_Close(io);

	static STRPTR file_list[] = {"myfile", "\xED\x9A\x8C\xEC\x82\xAC\xEC\x86\x8C\xEA\xB0\x9C.pdf", NULL};

	ScanDir("../test", CheckFileList, file_list);

	assert(file_list[0] == ARG_INT && file_list[1] == ARG_INT);

	io = FILE_Open(path, FILE_IO_STDIO);
	assert(io);
	assert(FILE_ReadLine(io) && strcmp(io->buffer, "Hidden layer of the file") == 0);
	FILE_Close(io);

	str = CreatePath("../test/myfile", False);

	assert(strcmp(str, "myfile") == 0);

	assert(CreatePath("../test/sub/directory", False) == NULL);
	assert(DeleteDOS("../test/myfile"));
	assert(DeletePath("../test"));
	assert(IsDir("../test") == False);

	assert(CreatePath("Not safe ", False) == NULL);
	assert(IsDir("Not safe ") == True);
	assert(DeleteDOS("Not safe "));

	printf("DOS test passed\n");

	/*
	 * Stream.c
	 */
	#if	0
	File fin  = FILE_Open("/planner.db3", FILE_IO_STDIO);
	File fout = FILE_Open("/planner-copy.db3", FILE_IO_WRITE);
	if (fin && fout)
	{
		while(FILE_Read(fin))
		{
			FILE_Write(fout, fin->buffer, fin->usage);
			fin->usage = fin->read = 0;
		}
	}
	FILE_Close(fin);
	FILE_Close(fout);
	#endif

	/*
	 * Lang.c
	 */
	out = fopen("test.po", "w");

	assert(out);

	fprintf(out,
		"Plural-Forms: nplurals=2; plural=n == 1 ? 0 : 1;\n"
		"msgid \"Hello world\"\n"
		"msgstr \"Salut tout le monde\"\n"
		"\n"
		"msgid \"colors\"\n"
		"msgstr \"couleurs\"\n"
	);
	fclose(out);

	Lang lang = LangAdd("French", "test.po");

	assert(lang);
	assert(strcmp(LangStr(lang, "Hello world", 0), "Salut tout le monde") == 0);
	assert(strcmp(LangStr(lang, "colors", 0), "couleurs") == 0);

	ThreadPause(1000); // Be sure modification date of file changes

	out = fopen("test.po", "a");
	assert(out);
	fprintf(out, "\nmsgid \"Check test\"\nmsgstr \"Taiste\"\n");
	fclose(out);

	lang = LangCheck(".", lang);
	assert(lang);
	assert(strcmp(LangStr(lang, "Hello world", 0), "Salut tout le monde") == 0);
	assert(strcmp(LangStr(lang, "Check test", 0), "Taiste") == 0);

	LangFree(lang);
	DeleteDOS("test.po");

	printf("Lang test passed\n");

	/*
	 * SHA1.c
	 */
	SHA1Buf sha1;

//	SHA1Digest("polka2501", 9, sha1);
//	assert(strcmp(sha1, "06a7d8733b91563453ca1ef334853810de3d2617") == 0);

	SHA1Digest("The quick brown fox jumps over the lazy dog", 43, sha1);
	assert(strcmp(sha1, "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12") == 0);

	printf("SHA1 test passed\n");

	/*
	 * HashTable.c
	 */
	static STRPTR files[] = {
		"00003_D", "000_1", "101091", "16PAG", "16siv",
		"21131", "49-spot", "64PAG", "Akiya", "Base_",
		"BIGFrank", "CKR", "coche", "CustomLine_525x400_OLD", "demorot",
		"Dynag", "ECTU", "ed9a8", "EQDEC", "FURNITURE",
		"Gamme", "guest", "HEJO", "IDA_30", "Invert",
		"J0002", "J0003", "J0004", "J0005-TestCV&CVC2", "J0006-ForLowRes",
		"J0010", "J0011", "J0012", "J0015", "J0016",
		"J0017", "J0020", "J0021", "J0022", "J0023",
		"J0371", "J0374", "J0405", "jampp", "ligne_v",
		"Loic", "Marks", "Marks-old", "Mille", "News_Marks",
		"niou", "ORG-V", "PageListTest", "pg36", "PM3_TEST",
		"prepa", "prepa-demo", "RB-Re", "scan.pl", "SLPI-0523_",
		"Small", "T22_3", "T67", "Tall", "test",
		"testname", "thierry", "twoCo", "T_Roads", "vermont",
		"ViewI", "Visua", "WAB_1"
	};

	HashTable h = HashCreate(10, HashStr, HashStrCmp);
	HashIterator iter;

	/* Insert */
	for (i = 0; i < DIM(files); i ++)
	{
		sprintf(path, "001-%08lx", (ULONG) crc32(files[i], strlen(files[i])));

		assert(HashSearch(h, path) == NULL);
		assert(HashInsert(h, strdup(path), files[i]));
		assert(HashInsert(h, strdup(path), files[i])); /* Insert twice : must be no-op */
	}
	assert(HashCount(h) == DIM(files));

	/* Search */
	for (i = 0; i < DIM(files); i ++)
	{
		sprintf(path, "001-%08lx", (ULONG) crc32(files[i], strlen(files[i])));

		assert(HashSearch(h, path) == files[i]);
	}

	/* HashScan */
	i = 0;
	HashScan(h, HashScanCount, &i);
	assert(i == HashCount(h));

	HashScan(h, HashScanFree, &i);
	assert(0 == HashCount(h) && i == 0);

	for (i = 0; i < DIM(files); i ++)
	{
		sprintf(path, "001-%08lx", (ULONG) crc32(files[i], strlen(files[i])));

		HashInsert(h, strdup(path), files[i]);
	}
	/* Remove */
	HashIterInit(h, &iter);
	do {
		STRPTR key = HashIterKey(&iter);
		STRPTR val = HashIterValue(&iter);
		free(key);

		for (j = 0; j < DIM(files); j ++)
			if (val == files[j]) { files[j] = NULL; break; }

		assert(j < DIM(files));
	}
	while (HashIterRem(&iter));

	assert(HashCount(h) == 0);
	for (i = 0; i < h->tablelength; i ++)
	{
		if (i < DIM(files)) assert(files[i] == NULL);
		assert(h->table[i] == NULL);
	}

	HashDestroy(h, False);

	printf("Hash table test passed\n");

	/*
	 * INI.c
	 */
	strcpy(path,
		"# A commentary line\n"
		"[Section 1]\n"
		"Key1=Value\n"
		"Key2 = Value with spaces  \n"
		"Key3 = \"With quotes\"\n"
		"[Section 2]\n"
		"Key2 = Not same value\n"
		"Key4 = value ; Hello world\n"
		"Key5 = ; Empty"
	);

	out = fopen("test.ini", "w");

	assert(out);
	fputs(path, out);
	fclose(out);

	assert(SetINIValue("test.ini", "Key1", "vAlUe"));
	assert(SetINIValue("test.ini", "Key1", "vA;Ue"));
	assert(SetINIValue("test.ini", "Section 1/Key2", "Value with spaces  "));
	assert(SetINIValue("test.ini", "Key5", "Not empty"));
	assert(SetINIValue("test.ini", "NoKey", ""));
	assert(SetINIValue("test.ini", "Section 2/Key1", "Inserted before key1"));
	assert(SetINIValue("test.ini", "Section 3/Key6", "Value 6"));

	INIFile ini = ParseINI("test.ini");

	assert(ini);
	assert(strcmp(GetINIValue(ini, "Key1"), "vA;Ue") == 0);
	assert(strcmp(GetINIValue(ini, "Section 2/Key1"), "Inserted before key1") == 0);
	assert(strcmp(GetINIValue(ini, "Key2"), "Value with spaces  ") == 0);
	assert(strcmp(GetINIValue(ini, "Section 1/Key3"), "With quotes") == 0);
	assert(strcmp(GetINIValue(ini, "Key4"), "value") == 0);
	assert(strcmp(GetINIValue(ini, "NoKey"), "") == 0);
	assert(strcmp(GetINIValue(ini, "Section 3/Key6"), "Value 6") == 0);

	FreeINI(ini);
	DeleteDOS("test.ini");

	printf("INI file test passed\n");

	/*
	 * Thread.c
	 */
	m = MutexCreate();
	i = 0;

	assert(m);
	MutexEnter(m);

	ThreadCreate(ThreadFunc, &i);

	i = 1;
	ThreadPause(500);
	assert(i == 1);
	MutexLeave(m);

	ThreadPause(500);
	assert(i == 2);

	MutexEnter(m);
	MutexLeave(m);
	MutexDestroy(m);

	printf("Thread test passed\n");

	return 0;
}

static int CheckList(ListHead * head)
{
	ListNode * node;
	ListNode * prev;
	int        count;

	for (count = 0, node = head->lh_Head, prev = NULL; node; prev = node,
	     count ++, node = node->ln_Next)
	{
		if (((ListItem *)node)->value != count + 1) return 0;
		if (node->ln_Prev != prev) return 0;
	}
	if (prev && prev->ln_Next != NULL) return 0;
	if (head->lh_Tail != prev)         return 0;
	if (head->lh_Count != count)       return 0;
	return 1;
}

static int CompareItem(ListNode * nodeA, ListNode * nodeB)
{
	return ((ListItem *)nodeA)->value -
	       ((ListItem *)nodeB)->value;
}

static int CheckFileList(STRPTR dir, STRPTR file, APTR ud)
{
	STRPTR * list;

	for (list = ud; *list && (*list == ARG_INT || strcmp(*list, file)); list ++);

	if (*list) *list = ARG_INT;

	return 1;
}

uint32_t crc_tab[256];
Bool     crc_init = False;

static uint32_t crc32(unsigned char * block, int length)
{
	ULONG crc, i, j;

	if (! crc_init)
	{
		ULONG poly;

		poly = 0xEDB88320L;
		for (i = 0; i < 256; i++)
		{
			crc = i;
			for (j = 8; j > 0; j--)
			{
				if (crc & 1) crc = (crc >> 1) ^ poly;
				else crc >>= 1;
			}
			crc_tab[i] = crc;
		}
	}

	crc = 0xFFFFFFFF;
	for (i = 0; i < length; i++)
	{
	  crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
	}
	return (crc ^ 0xFFFFFFFF);
}

static int HashScanCount(APTR k, APTR v, APTR d)
{
	(* (int *) d) ++;
	return 0;
}

static int HashScanFree(APTR k, APTR v, APTR d)
{
	free(k);
	(* (int *) d) --;
	return 1;
}

static void ThreadFunc(void * arg)
{
	MutexEnter(m);
	* (int *) arg = 2;
	MutexLeave(m);
}

static void CheckRes(STRPTR ident, Variant v, int store, APTR data)
{
	if (store == 0 && ident)
		memcpy(v, data, sizeof *v);
	else
		memcpy(data, v, sizeof *v);
}


#endif

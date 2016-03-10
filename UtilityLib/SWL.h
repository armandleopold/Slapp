/*
 * SWL.h : main header  for Simple Widget Library. Contains most of useful
 *         stuff in there.
 *
 * This is NOT a documentation center. Documentation is in .c files, read it
 * instead of trying to figure their behaviour here.
 *
 * Written by T.Pierron, feb 19, 2006.
 */

#ifndef	SWL_H
#define	SWL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

// #ifdef	BUILDING_DLL
// #define	DLLIMP		__declspec(dllexport)
// #else
// #define	DLLIMP		__declspec(dllimport)
// #endif
#define	DLLIMP

/* Basic datatypes */
typedef char                     TEXT;
typedef char *                   STRPTR;
typedef void *                   APTR;
typedef uint8_t *                data8_t;
typedef unsigned long            ULONG;
typedef long                     LONG;
typedef struct ListHead_t        ListHead;
typedef struct ListNode_t        ListNode;
typedef struct Request_t         Request;
typedef struct File_t *          File;
typedef struct Entry_t *         Entry;
typedef struct HashTable_t *     HashTable;
typedef struct HashTableItr_t *  HashTableItr;
typedef struct HashTableItr_t    HashIterator;
typedef struct DFA_t *           DFA;
typedef struct ArgList_t *       ArgList;
typedef struct Variant_t *       Variant;
typedef struct Variant_t         VariantBuf;
typedef struct Lang_t *          Lang;
typedef APTR                     Mutex;
typedef APTR                     Semaphore;
typedef ULONG                    Thread;
typedef TEXT                     SHA1Buf[41];

#ifndef	False
typedef	enum { False, True }     Bool;
#endif

/* List datatype */
struct ListHead_t
{
	ListNode * lh_Head;
	ListNode * lh_Tail;
	ULONG      lh_Count;
};

struct ListNode_t
{
	ListNode * ln_Next;
	ListNode * ln_Prev;
};

#define	MAXKEYS       64

#ifndef	CUSTOM_REQ
struct Request_t
{
	STRPTR req_Arg[MAXKEYS];
	int    req_NbArg;
};
#else
struct Request_t;
#endif

/* Core.c */
DLLIMP void UtilityInit(STRPTR app);

/* List.c */
typedef int (*ListSortFunc)(ListNode *, ListNode *);

DLLIMP void       ListAddHead(ListHead *, ListNode *);
DLLIMP void       ListAddTail(ListHead *, ListNode *);
DLLIMP void       ListInsert(ListHead *, ListNode * item, ListNode * insert_after);
DLLIMP void       ListInsertSort(ListHead *, ListNode *, ListSortFunc);
DLLIMP void       ListSort(ListHead *, ListSortFunc);
DLLIMP void       ListRemove(ListHead *, ListNode *);
DLLIMP ListNode * ListRemHead(ListHead *);
DLLIMP ListNode * ListRemTail(ListHead *);

#define	ListNew(ptr)        memset(ptr, 0, sizeof *ptr)
#define	NEXT(ptr)           (ptr = (APTR) ((ListNode *)(ptr))->ln_Next)
#define	PREV(ptr)           (ptr = (APTR) ((ListNode *)(ptr))->ln_Prev)
#define	HEAD(list)          ((APTR)(list).lh_Head)
#define	TAIL(list)          ((APTR)(list).lh_Tail)

/* DOS.c prototypes */
extern Bool ForceForwardSlash;

typedef int DirScanFunc(STRPTR dir, STRPTR file, APTR data);

DLLIMP int    ScanDir(STRPTR dir, DirScanFunc, APTR data);
DLLIMP void   SafePath(STRPTR path);
DLLIMP void   AddExt(STRPTR path, STRPTR ext, int max, Bool replace);
DLLIMP Bool   AddPart(STRPTR dir, STRPTR file, int max);
DLLIMP Bool   ParentDir(STRPTR path);
DLLIMP Bool   IsDir(STRPTR path);
DLLIMP STRPTR CatPath(STRPTR path, ...);
DLLIMP STRPTR CreatePath(STRPTR path, Bool not_last);
DLLIMP STRPTR BaseName(STRPTR path);
DLLIMP STRPTR Unix2DOS(STRPTR inout);
DLLIMP STRPTR DOS2Unix(STRPTR inout);
DLLIMP STRPTR GetExtension(STRPTR file);
DLLIMP ULONG  TimeStamp(STRPTR path, int type);
DLLIMP int    Execute(STRPTR cmd);
DLLIMP ULONG  FileSize(STRPTR path);
DLLIMP void   GetSysError(STRPTR header, STRPTR buffer, int max);
DLLIMP STRPTR GetError(void);
DLLIMP Bool   IsSameFile(STRPTR file1, STRPTR file2);
DLLIMP Bool   DeleteDOS(STRPTR path);
DLLIMP Bool   DeletePath(STRPTR path);
DLLIMP APTR   LockFileMode(STRPTR path, int mode);
DLLIMP void   UnlockFileMode(APTR lock);
DLLIMP Bool   FileExists(STRPTR file);
DLLIMP Bool   FileVisible(STRPTR file);
DLLIMP Bool   FileRename(STRPTR from, STRPTR to, Bool overwrite);
DLLIMP Bool   FileCopy(STRPTR from, STRPTR to, Bool overwrite);

enum /* Possible values for parameters 'mode' of LockFile() */
{
	LFM_READ  = 1,
	LFM_WRITE = 2
};

/* String.c */
DLLIMP void   CopyString(STRPTR destination, STRPTR source, int max);
DLLIMP STRPTR RawDoFmt(STRPTR format, va_list *);
DLLIMP Bool   FnMatch(STRPTR pattern, STRPTR string, int flags);
DLLIMP Bool   StrReplace(STRPTR src, STRPTR find, STRPTR rep, int max);
DLLIMP Bool   StrCatDup(STRPTR * buffer, int c, ...);
DLLIMP int    StrCatM(STRPTR dest, int c, int max, ...);
DLLIMP int    StrCatLen(STRPTR dst, STRPTR src, int max, int len);
DLLIMP int    StrLenM(STRPTR src, ...);
DLLIMP int    StrCount(STRPTR list, int chr);
DLLIMP int    CharInSet(STRPTR str, STRPTR set);
DLLIMP int    HexToBin(TEXT c);
DLLIMP int    FindInList(STRPTR list, STRPTR word, int len);
DLLIMP STRPTR NthWord(STRPTR str, int nth, int chr);
DLLIMP void   Unescape(STRPTR io);
DLLIMP void   AssignStrings(STRPTR * list, void * buffer, ...);
DLLIMP void   LOG_Info(STRPTR fmt, ...);
DLLIMP void   LOG_InfoA(STRPTR fmt, va_list arg);
DLLIMP void   SHA1Digest(void * buffer, int len, void * resblock);
DLLIMP STRPTR StripCRLF(STRPTR in);
DLLIMP void   ToHexStr(STRPTR str, int nb);

/* Encodings.c */
DLLIMP int    CopyStringEnc(STRPTR dest, int max, STRPTR src, int enc);
DLLIMP APTR   StrDupEnc(STRPTR str, int enc);
DLLIMP STRPTR NthChar(STRPTR str, int nth, int enc);
DLLIMP void   ConvertEnc(STRPTR dest, int max, STRPTR src, int enc);

enum /* Values for CopyStringEnc */
{
	ENC_CP1252,
	ENC_UTF8,
	ENC_UTF16    /* Internal usage only */
};

#ifndef	FNM_PATHNAME
enum /* FnMatch flags */
{
	FNM_PATHNAME    = 1,
	FNM_NOESCAPE    = 2,
	FNM_PERIOD      = 4,
	FNM_LEADING_DIR = 8,
	FNM_CASEFOLD    = 16
};
#endif

/* Some useful macros */
#define	StripExtension(p)  AddExt(p, "", 0, True)
#define	StrCat(d, s, m)    StrCatM(d, m, -1, s, NULL)
#define	IsDef(val)         ((val) && (val)[0])
#define	DIM(table)         (sizeof(table) / sizeof(table[0]))
#define	ESC_CHAR           "\t\r\n%"
#define	URL_ESC            ESC_CHAR "+<>[]^`{|}#&,;:=?'()~ "
#define	UNSAFE_CHAR        "\\/:\"<>|?*"
#define	UNSAFE_CHARHTML    "\\ / : \" ' &lt; &gt; | ? *"
#define	swap(a, b)         ((a) ^= (b), (b) ^= (a), (a) ^= (b))
#define	swap_tmp(a,b,t)    (t = a, a = b, b = t)
#define	offsetp(p,f)       ((ULONG) (&((p)0L)->f))
#define	PARAM(ud,n)        ((APTR *)ud)[n]
#ifndef	MIN
#define	MIN(a, b)          ((a) < (b) ? (a) : (b))
#endif
#ifndef	MAX
#define	MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

DLLIMP uint32_t LE32ToHost(uint32_t raw);
DLLIMP uint32_t HostToBE32(uint32_t raw);

/* CGI.c */
DLLIMP void   ParseCGI(Request * req, STRPTR request);
DLLIMP STRPTR GetValue(Request * req, STRPTR key);
DLLIMP ULONG  GetNumValue(Request * req, STRPTR key);
DLLIMP Bool   GetBoolValue(Request * req, STRPTR key);
DLLIMP STRPTR SetValue(Request * req, STRPTR key, STRPTR value);
DLLIMP STRPTR ExtractValue(Request * req, STRPTR key);
DLLIMP STRPTR GetProperty(STRPTR * list, STRPTR key);
DLLIMP int    GetNumProperty(STRPTR * list, STRPTR key);

/* DFA.c */
DLLIMP DFA  DFA_Create(void);
DLLIMP long DFA_Search(DFA dfa, STRPTR key, long insert);
DLLIMP void DFA_Free(DFA dfa);

/* Thread.c */
typedef void (*ThreadCb)(void *);

DLLIMP Mutex     MutexCreate(void);
DLLIMP void      MutexEnter(Mutex);
DLLIMP void      MutexLeave(Mutex);
DLLIMP void      MutexDestroy(Mutex);
DLLIMP Thread    ThreadCreate(ThreadCb func, APTR arg);
DLLIMP void      ThreadPause(int delay);
DLLIMP Semaphore SemInit(int count);
DLLIMP Bool      SemWait(Semaphore);
DLLIMP void      SemAdd(Semaphore, int count);
DLLIMP void      SemClose(Semaphore);


/* HashTable.c */
typedef ULONG (*HashCb)(APTR k);
typedef int   (*EqualCb)(APTR k1, APTR k2);
typedef int   (*ScanCb)(APTR k, APTR v, APTR d);

DLLIMP HashTable HashCreate(ULONG minsize, HashCb, EqualCb);
DLLIMP int       HashInsert(HashTable, APTR k, APTR v);
DLLIMP APTR      HashSearch(HashTable, APTR k);
DLLIMP APTR      HashRemove(HashTable, APTR k);
DLLIMP ULONG     HashCount(HashTable);
DLLIMP void      HashDestroy(HashTable, int free_values);
DLLIMP void      HashIterInit(HashTable, HashTableItr);
DLLIMP int       HashIterNext(HashTableItr);
DLLIMP int       HashIterRem(HashTableItr);
DLLIMP void      HashScan(HashTable, ScanCb, APTR);
DLLIMP ULONG     HashStr(APTR k);
DLLIMP int       HashStrCmp(APTR k1, APTR k2);
DLLIMP int       HashStrCaseCmp(APTR k1, APTR k2);

#define	HashIterKey(i)	 (i)->e->k
#define	HashIterValue(i) (i)->e->v

struct Entry_t
{
	APTR  k, v;
	ULONG h;
	Entry next;
};

struct HashTable_t
{
	ULONG   tablelength;
	Entry * table;
    ULONG   entrycount;
    ULONG   loadlimit;
    ULONG   primeindex;
    HashCb  hashfn;
    EqualCb eqfn;
};

struct HashTableItr_t
{
	HashTable h;
    Entry     e;
    Entry     parent;
    ULONG     index;
};

/* Stream.c */
DLLIMP File   FILE_Open(STRPTR file, int type);
DLLIMP File   FILE_OpenIP(STRPTR server, int port);
DLLIMP File   FILE_OpenURL(STRPTR url);
DLLIMP int    FILE_Write(File, STRPTR buffer, int length);
DLLIMP int    FILE_WriteStr(File io, ...);
DLLIMP int    FILE_WriteEsc(File io, STRPTR src, STRPTR escset);
DLLIMP int    FILE_Read(File);
DLLIMP int    FILE_ReadLine(File);
DLLIMP int    FILE_FlushWrite(File);
DLLIMP int    FILE_PrintF(File, STRPTR fmt, ...);
DLLIMP Bool   FILE_Close(File);

extern int FilenameEncoding;

typedef int (*io_func)(File fh, void * buffer, int size);

struct File_t
{
	STRPTR   buffer;
	STRPTR   url;
	STRPTR * header;
	void *   fd;
	int      usage, read;
	TEXT     svg, type;
	io_func  recv;
	io_func  write;
	APTR     ud;
};

enum /* Possible values for 'type' field */
{
	FILE_IO_STDIO,
	FILE_IO_WRITE,
	FILE_IO_HTTP,
	FILE_IO_IP,
	FILE_IO_DUMMY
};

#define	SZ_CHUNK       4096

/* ARGV.c */
DLLIMP void     FreeArgvList(ArgList start);
DLLIMP ArgList  MakeArgList(STRPTR id, ...);
DLLIMP ArgList  MakeArgListFromCGI(STRPTR request);
DLLIMP void     ArgvAppend(ArgList * first, ArgList item);
DLLIMP void     ArgvRemove(ArgList * first, ArgList item);
DLLIMP void     ArgvInsert(ArgList * first, ArgList insert_after, ArgList item);

#define	ARG_INT                  ((STRPTR)-1)
#define	ARG_DBL                  ((STRPTR)-2)
#define	ARG_REV                  ((void *)-3)
#define	PARAMLIST(arg)           (arg ? ((ArgList)arg)->al_Array : NULL)
#define	NEXTOBJ(arg)             ((ArgList)arg)->al_Next
#define	FREE_AND_GONEXT(arg)     { ArgList next = arg->al_Next; free(arg); arg = next; }

#define	PROP(al, prop)           GetProperty((al)->al_Array, #prop)
#define	NUM(al, prop)            GetNumProperty((al)->al_Array, #prop)


struct ArgList_t
{
	ArgList al_Next;
	int     al_Size;
	APTR    al_UserData;
	APTR    al_Extra;
	STRPTR  al_Array[0];
};

/* INI.c */
typedef struct INIFile_t *    INIFile;

DLLIMP INIFile ParseINI(STRPTR file);
DLLIMP INIFile ParseINIStr(STRPTR buf);
DLLIMP STRPTR  GetINIValue(INIFile, STRPTR section);
DLLIMP STRPTR  GetINIValueStr(INIFile ini, STRPTR key, STRPTR def);
DLLIMP int     GetINIValueInt(INIFile ini, STRPTR key, int def);
DLLIMP Bool    SetINIValue(STRPTR path, STRPTR key, STRPTR val);
DLLIMP Bool    SetINIValueInt(STRPTR path, STRPTR key, int val);
DLLIMP Bool    IterINI(INIFile, STRPTR * key, STRPTR * value);
DLLIMP void    IterINIReset(INIFile ini);
DLLIMP void    FreeINI(INIFile);

/* Lang.c */
DLLIMP Lang   LangAdd(STRPTR name, STRPTR path);
DLLIMP STRPTR LangStr(Lang lang, STRPTR msg, int max);
DLLIMP STRPTR LangStrPlural(Lang lang, int nb, STRPTR sing, STRPTR plur);
DLLIMP void   LangFree(Lang lang);
DLLIMP Lang   LangSet(STRPTR root, STRPTR name);
DLLIMP Lang   LangCheck(STRPTR root, Lang);

#define	_(str)              LangStr(NULL, str, 0)
#define	_N(n, plur, sing)   LangStrPlural(NULL, n, plur, sing)
#define	D_(str)             str

/* Calc.c */
typedef void (*ParseExpCb)(STRPTR, Variant, int store, APTR data);

int ParseExpression(STRPTR exp, ParseExpCb cb, APTR data);

enum /* Return codes of ParseExpression */
{
	PERR_SyntaxError = 1,
	PERR_DivisionByZero,
	PERR_LValueNotModifiable,
	PERR_TooManyClosingParens,
	PERR_MissingOperand
};


struct Variant_t
{
	int type;
	union {
		long   z;
		double r;
	} v;
};

enum /* Possible values for 'type' field */
{
	TYPE_INT,
	TYPE_IDF,
	TYPE_DBL,
	TYPE_OPE
};

#endif

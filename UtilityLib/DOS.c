/*
 * DOS.c : Disk operating related functions. Handle pathname management,
 * directory listing, etc ...
 *
 * Written by T.Pierron, feb 15, 2006.
 */

#include "SWL.h"
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <windows.h>
#include <sys/stat.h>
#include <dirent.h>
#include <wchar.h>

/**h* SWL/DOS
 * INTRODUCTION
 *	This module completes some lack of the libc concerning path
 *	management and especially directory scanning, which is available through
 *	Win32 or POSIX calls.
 *
 *	Examining, reading, writing, deleting or renaming files/directories can
 *	be accomplished using standard C library functions (usually located in
 *	stdio), check manual page for complete documentation of :
 *
 *	- int stat(const char * path, struct stat * stbuf);
 *	  This is a POSIX function available in stdlib.h, though most systems
 *	  usually provide an implementation. All field may not be relevant, but
 *	  it is worth to get detailed information on a file/directory.
 *
 *	- int rename(char * from, char * to);
 *	  Use it to rename/move a file/directory.
 *
 *	- int remove(char * path);
 *	  Can be use to delete a file or an empty directory.
 *
 *	- stdio fopen(), fread(), fwrite(), fprintf(), fscanf(), ....
 *	  These functions are quite handy to process content of a file and would
 *	  be overkill to provide replacement for them.
 *
 *	Note that Windows accepts both forward and backward slash as filename
 *	separator. This modules will use backward slash whenever possible,
 *	unless forward is already present in filename (or forced using dedicated
 *	global variable).
 */

static void AddPartW(LPWSTR path, LPWSTR file, int max);

/**v* DOS/ForceForwardSlash
 * NAME
 *	ForceForwardSlash - a variable controlling use of slash in pathname
 *
 * SYNOPSIS
 *	Bool ForceForwardSlash = False;
 *
 * FUNCTION
 *	This global variable controls what kind of slash should be used by default
 *	by all functions that manipulate path. Okay, this is a quick hack intended
 *	for those, like me, who hates that damn backward slash. Absolutly not thread
 *	safe, set it up when starting your program, or know what your are doing.
 */
DLLIMP Bool ForceForwardSlash = False;

/**v* DOS/FilenameEncoding
 * NAME
 *	FilenameEncoding - control encoding of filename.
 *
 * SYNOPSIS
 *	int FilenameEncoding = ENC_CP1252;
 *
 * FUNCTION
 *	Control encoding of filename given as argument or returned by all DOS related
 *	functions. Possible values are:
 *		ENC_CP1252 - Windows-1252 charset. Superset of iso-8859-1.
 *		ENC_UTF8   - 8bit unicode with variable-length characters.
 *
 *	This is a global intended to be initialized at startup. For sanity, avoid
 *	using many encoding within your program, convert everything if it does not
 *	conforms to what expected.
 */
DLLIMP int FilenameEncoding = ENC_CP1252;


/**f* DOS/ScanDir
 * NAME
 *	ScanDir - Scan a directory content
 *
 * SYNOPSIS
 *	int ScanDir(STRPTR dir, DirScanFunc cb, APTR data);
 *
 * FUNCTION
 *	Scan each entries of the dir (. and .. will be filtered - you can
 *	assume they always exists). For each entries the user callback will be
 *	applied on them. Expected prototype for this callback is :
 *
 *		int DirScanFunc(STRPTR dir, STRPTR file, APTR data);
 *
 *	Each parameters have following meaning :
 *
 *	- dir : this the same value than the one passed to ScanDir().
 *	- file : this is a file or dir name found in the directory, which does
 *	         not include the directory part. No particular order should be
 *	         assumed : entries are listed as they appear in the directory.
 *	- data : the user data supplied to ScanDir().
 *
 *	Callback should return a positive or 0 value if directory scanning must
 *	continue or negative if it must stop immediatly.
 *
 * RESULT
 *	The *sum* of all return codes of your callback.
 *
 * SEE ALSO
 *	stat(3), AddPart
 */
DLLIMP int ScanDir(STRPTR dirname, DirScanFunc cb, APTR data)
{
	WIN32_FIND_DATAW fdat;
	HANDLE           fd;
	LPWSTR           dir = StrDupEnc(dirname, FilenameEncoding);
	LPWSTR           end = wcschr(dir, 0);
	int              ret = 0;

	AddPartW(dir, L"*", wcslen(dir) + 5);
	fd = FindFirstFileW(dir, &fdat);

	if (fd != INVALID_HANDLE_VALUE)
	{
		end[0] = 0;

		do
		{
			if (wcscmp(fdat.cFileName, L"..") && wcscmp(fdat.cFileName, L"."))
			{
				int rc;

				if (FilenameEncoding != ENC_UTF16)
				{
					TEXT buffer[MAX_PATH * 3 / 2];

					ConvertEnc(buffer, sizeof buffer, (STRPTR) fdat.cFileName, FilenameEncoding);

					rc = cb(dirname, buffer, data);
				}
				else rc = cb(dirname, (STRPTR) fdat.cFileName, data);

				if (rc < 0) break;
			}
		}
		while (FindNextFileW(fd, &fdat));

		FindClose(fd);
	}
	free(dir);
	return ret;
}

static int ScanDirW(LPWSTR dirname, DirScanFunc cb, APTR data)
{
	WIN32_FIND_DATAW fdat;
	HANDLE           fd;
	LPWSTR           end = wcschr(dirname, 0);
	int              ret = 0;

	AddPartW(dirname, L"*", wcslen(dirname) + 5);
	fd = FindFirstFileW(dirname, &fdat);

	if (fd != INVALID_HANDLE_VALUE)
	{
		end[0] = 0;

		do
		{
			if (wcscmp(fdat.cFileName, L"..") && wcscmp(fdat.cFileName, L"."))
			{
				int rc = cb((STRPTR) dirname, (STRPTR) fdat.cFileName, data);

				if (rc < 0) break;
			}
		}
		while (FindNextFileW(fd, &fdat));

		FindClose(fd);
	}
	else end[0] = 0;
	return ret;
}

/**f* DOS/AddPart
 * NAME
 *	AddPart - Appends a file/dir to the end of a path
 *
 * SYNOPSIS
 *	Bool AddPart(STRPTR path, STRPTR file, int max);
 *
 * FUNCTION
 *	This function adds a file, directory, or subpath name to a directory
 *	path name taking into account any required separator characters. If
 *	"file" is a fully-qualified path it will totally replace the current
 *	value of "path".
 *
 * INPUT
 *	path - the path to add a file/directory name to.
 *	file - the filename or directory name to add. May be a relative
 *	       pathname from the current directory (example: foo\bar).
 *	       Can deal with '..', to get parent directory.
 *	max  - Size in bytes of the space allocated for dirname.
 *
 * RESULT
 *	True if file part was added successfully or False if the buffer would
 *	have overflowed (path might be partialy modified). Validity of path will
 *	not be verified.
 *
 * EXAMPLE
 *	AddPart("C:/Program files", "Somewhere");
 *		=> "C:/Program files/Somewhere"
 *	AddPart("C:/Temp", "../Windows/System32");
 *		=> "C:/Windows/System32"
 *		   More than one component can be given for 'file', '..' is processed
 *		   to get a somewhat canonical name).
 *	AddPart("C:/Temp", "/Windows/System32");
 *		=> "C:/Windows/System32"
 *		   A starting / or \ in the 'file' path indicates root of disk.
 *	AddPart("C:/Temp", "D:/My documents");
 *		=> "D:/My documents"
 *		   Fully qualified path completly overrides source path.
 *	AddPart("Folder", "Sub folder");
 *		=> "Folder\\Sub folder"
 *		   If kind of slash cannot be determined, backward will be used as
 *		   default...
 *	AddPart("Folder", "Sub folder/Even deeper");
 *		=> "Folder/Sub folder/Even deeper"
 *		   ...unless forward slash is used in 'file'.
 *
 * SEE ALSO
 *	ParentDir, CatPath, ForceForwardSlash
 */
static Bool IsParent(STRPTR path)
{
	return strncmp(path, "..", 2) == 0 && strchr("/\\", path[2]);
}

DLLIMP Bool AddPart(STRPTR path, STRPTR src, int max)
{
	STRPTR p    = strchr(path, 0);
	STRPTR file = src;
	int    n    = 0;
	TEXT   sep;

	/* Check for separator used in source path */
	while (p > path && *p != '/' && *p != '\\') p --;
	if (p == path)
	{
		for (p = src; *p && *p != '/' && *p != '\\'; p ++);
		sep = (*p ? *p : ForceForwardSlash ? '/' : '\\');
	}
	else sep = *p;
	p = strchr(path, 0);

	/* Is file a fully qualified path ? */
	if (isalpha(file[0]) && file[1] == ':')
	{
		if (strlen(file) >= max) return False;

		strcpy(path, file);
	}
	else if (file[0] && strchr("\\/", file[0]))
	{
		/* Absolute path name */
		p = path;
		if (isalpha(p[0]) && p[1] == ':') p += 2, max -= 2;
		if (strlen(file) >= max) return False;
		strcpy(p, file);
	}
	else for (n = p - path; *file && n < max; file ++) /* Append file to path */
	{
		if (IsParent(file) && p > path && ! IsParent(p - 3))
		{
			/* Get directly parent directory of path */
			ParentDir(path);
			file += 2;
			n = strlen(path);
			p = path + n;
			if (*file == 0) file --;
		}
		else /* Append part */
		{
			if (p > path && p[-1] != sep && n < max)
				*p ++ = sep, n ++;

			for (; n < max && *file && *file != '/' && *file != '\\'; file ++, n ++)
				*p ++ = *file;

			if (n < max) p[ 0] = 0;
			else         p[-1] = 0;
			if (*file == 0) file --;
		}
	}
	return True;
}

/**f* DOS/ParentDir
 * NAME
 *	ParentDir - Removes the last part of a filename
 *
 * SYNOPSIS
 *	Bool ParentDir(STRPTR dir);
 *
 * FUNCTION
 *	This function simply *modify* its argument to point to its parent
 *	directory. Note that validity of path will NOT be verified. It is
 *	somewhat like AddPart(dir, "..").
 *
 * RESULT
 *	True if path contained a removable part, False is there was no more
 *	part to remove.
 *
 * EXAMPLE
 *	ParentDir("C:/Program files/Common files/");   => "C:/Program files"
 *	ParentDir("C:\\Program files");                => "C:"
 *	ParentDir("C:\\");                             => returns False.
 *	ParentDir("Folder/Sub")                        => "Folder"
 *	ParentDir("Folder")                            => ""
 *
 * SEE ALSO
 *	AddPart, CatPath, CreatePath
 */
DLLIMP Bool ParentDir(STRPTR dir)
{
	STRPTR p = strchr(dir, 0) - 1;

	if (! IsDef(dir)) return False;

	/* Ignore final slash */
	if (p > dir && strchr("\\/", *p)) p --;

	while (p > dir && *p != '/' && *p != '\\') p --;

	if (p >= dir && isalpha(p[0]) && p[1] == ':') return False;

	*p = 0;

	return True;
}

/**f* DOS/SafePath
 * NAME
 *	SafePath - sanitize path to avoid bad surprise.
 *
 * SYNOPSIS
 *	void SafePath(STRPTR path);
 *
 * FUNCTION
 *	Not every characters in file name are allowed. Especially on Windows,
 *	where the system silently removes characters from filename, giving some
 *	surprise when attempting to find it by scanning directory.
 *
 *	Forbidden characters includes : \ / : * \ " < > | ?
 *	Those characters will be replaced by _
 *
 *	Moreover, it is not possible to have one or several dots/spaces/
 *	underscores at the end of filename, at least since Windows 2000. Thus
 *	those characters will be trimmed from string.
 *
 *	The function modifies its argument (result will always contains same or
 *	less characters than original), do not pass constant strings.
 */
DLLIMP void SafePath(STRPTR path)
{
	STRPTR p;

	for (p = path; *p; p ++)
	{
		if (strchr(UNSAFE_CHAR, *p))
			*p = '_';
		if (strchr("\n\r", *p))
			*p-- = 0;
	}

	/* Remove starting underscores */
	for (p = path; *p == '_' || *p == ' '; p ++);
	if (p > path) strcpy(path, p);

	/* Remove trailing spaces, underscores and dots */
	for (p = strchr(path, 0); p > path; p --)
	{
		if (strchr(" _.", *p)) *p = 0;
		else break;
	}
}


/**f* DOS/CatPath
 * NAME
 *	CatPath - Catenate directory components into a single path
 *
 * SYNOPSIS
 *	STRPTR CatPath(STRPTR path, ...);
 *
 * FUNCTION
 *	Catenate each path passed as arguments list (which MUST be NULL
 *	terminated). Path can be relative or absolute. Validity of the path
 *	will NOT be checked. This function is based on AddPart, same rules
 *	apply to this function.
 *
 * INPUT
 *	path  - first component of path to catenate.
 *	...   - list of other components that must be NULL-terminated.
 *
 * RESULT
 *	Catenated path in a malloc()'ed buffer, that you must free() yourself.
 *
 * SEE ALSO
 *	AddPart
 */
DLLIMP STRPTR CatPath(STRPTR fmt, ...)
{
	va_list args;
	STRPTR  res;
	int     max;

	va_start(args, fmt);
	for (max = strlen(fmt) + 1; (res = va_arg(args, STRPTR)); max += strlen(res) + 1);
	va_end(args);

	res = malloc(max);
	res[0] = 0;
	va_start(args, fmt);
	do {
		AddPart(res, fmt, max);
		fmt = va_arg(args, STRPTR);
	} while (fmt);
	va_end(args);

	return res;
}

/**f* DOS/CreatePath
 * NAME
 *	CreatePath - Create missing directories from specified path
 *
 * SYNOPSIS
 *	STRPTR CreatePath(STRPTR path, Bool not_last);
 *
 * FUNCTION
 *	Creates each missing part of the specified path. Stop at the first
 *	component that cannot be created and returns its reference.
 *
 * INPUT
 *	path     - path where directory will be created
 *	not_last - avoid creating a directory for last component of path. If
 *	           path is supposed to point to a file, you can call this
 *	           function with 'not_last' set to True, to create possibly
 *	           missing directory, without modifying the string. Thus can
 *	           use it just afterward to create your file.
 *
 * RESULT
 *	NULL if everything was fine or pointer to part that causes the function
 *	to fail.
 *
 * SEE ALSO
 *	CatPath, AddPart
 */
DLLIMP STRPTR CreatePath(STRPTR path, Bool not_last)
{
	LPWSTR dup = StrDupEnc(path, FilenameEncoding);
	LPWSTR p, next;
	STRPTR ret = NULL;
	DWORD  attr;
	int    unc = -1;

	if (dup == NULL) return False;

	for (p = dup; p && *p; p = next)
	{
		for (next = p; *next && *next != '/' && *next != '\\'; next ++);

		if (p == next || (unc > 0 && unc < 3)) { unc ++; next ++; continue; } /* Absolute or UNC path */
		if (next[0] == 0 && not_last) break;
		if (next[0] == 0) next = NULL;
		else              next[0] = 0;

		/* Check if dir exists */
		attr = GetFileAttributesW(dup);
		if (attr == 0xFFFFFFFF || (attr & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if (CreateDirectoryW(dup, NULL) == 0)
			{
				for (p = wcschr(dup, 0); p > dup && *p != '/' && *p != '\\'; p --);
				ret = NthChar(path, p - dup + 1, FilenameEncoding);
				break;
			}
		}
		if (next) *next++ = '/';
	}
	free(dup);
	return ret;
}

/**f* DOS/BaseName
 * NAME
 *	BaseName - Retrieve file part of given path
 *
 * SYNOPSIS
 *	STRPTR BaseName(STRPTR path);
 *
 * FUNCTION
 *	Get last component of a path.
 *
 * RESULT
 *	Pointer will reference memory inside path, nothing will be allocated.
 *
 * EXAMPLES
 *	BaseName("C:\\Program files\\XYZ corp")
 *		=> Pointer to "XYZ corp"
 *	BaseName("Test file.txt")
 *		=> Pointer to "Test file.txt"
 *	BaseName("C:\\Temp\\")
 *		=> Pointer to end of string
 *
 * SEE ALSO
 *	CatPath, AddPart
 */
DLLIMP STRPTR BaseName(STRPTR path)
{
	if (path == NULL) return "";

	STRPTR base  = strrchr(path, '\\');
	STRPTR base2 = strrchr(path, '/');

	if (base < base2)
		base = base2;

	return base ? base + 1 : path;
}


/**f* DOS/IsDir
 * NAME
 *	IsDir - Check if a path points to a directory.
 *
 * SYNOPSIS
 *	Bool IsDir(STRPTR path)
 *
 */
DLLIMP Bool IsDir(STRPTR path)
{
#ifdef	WIN32
	LPWSTR dir  = StrDupEnc(path, FilenameEncoding);
	DWORD  attr = GetFileAttributesW(dir);
	BOOL   ret  = (attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_DIRECTORY));

	free(dir);

	return ret;
#else
	struct stat stbuf;

	return ret = stat(path, &stbuf) == 0 && S_ISDIR(stbuf.st_mode);
#endif
}

static Bool IsDirW(LPWSTR path)
{
	DWORD attr = GetFileAttributesW(path);

	return attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

/**f* DOS/Unix2DOS, DOS/DOS2Unix
 * NAME
 *	Unix2DOS - Convert a /-separated path into a \-separated
 *	DOS2Unix - Convert a \-separated path into a /-separated
 *
 * SYNOPSIS
 *	STRPTR Unix2DOS(STRPTR path);
 *	STRPTR DOS2Unix(STRPTR path);
 *
 * INPUT
 *	path - pointer to string where path will be modified. Do not pass
 *	       static strings.
 *
 * RESULT
 *	Pointer to path. Function never fails.
 */
DLLIMP STRPTR Unix2DOS(STRPTR inout)
{
	STRPTR p;

	for (p = inout; *p; p ++)
		if (*p == '/') *p = '\\';

	return inout;
}

DLLIMP STRPTR DOS2Unix(STRPTR inout)
{
	STRPTR p;

	for (p = inout; *p; p ++)
		if (*p == '\\') *p = '/';

	return inout;
}


/**f* DOS/GetExtension
 * NAME
 *	GetExtension - Give a pointer to last dot
 *
 * SYNOPSIS
 *	STRPTR GetExtension(STRPTR path);
 *
 * RESULT
 *	Pointer to character following last dot in the path.
 *	If no dot is found before the path separator, a pointer to the end
 *	of the string will be returned.
 *
 * SEE ALSO
 *	BaseName
 */
DLLIMP STRPTR GetExtension(STRPTR file)
{
	STRPTR dot = strchr(file, 0);
	STRPTR end = dot;

	while (dot > file && *dot != '/' && *dot != '\\' && *dot != '.') dot --;

	return (*dot == '.' ? dot + 1 : end);
}

/**f* DOS/AddExt, DOS/StripExtension
 * NAME
 *	AddExt - replace or add a file extension to given path.
 *	StripExtension - remove extension of path, if any.
 *
 * SYNOPSIS
 *	void AddExt(STRPTR path, STRPTR ext, int max, Bool replace);
 *	void StripExtension(STRPTR path);
 *
 * FUNCTION
 *	AddExt can be used to add, replace or remove an extension from the last
 *	component of a path.
 *
 *	StripExtension is a simple macro that expends to : AddExt(path, "", 0, True).
 *
 * INPUT
 *	- path    : buffer to modify.
 *	- ext     : extension to add or replace. Initial dot will be ignored.
 *	- max     : size of buffer in bytes.
 *	- replace : replace extension if any, otherwise append it.
 *
 * EXAMPLES
 *	AddExt("C:/Myfile.txt", ".tif", 100, True);   => "C:/MyFile.tif"
 *	AddExt("C:/Myfile", "tif", 100, True);        => "C:/MyFile.tif"
 *	AddExt("C:/Myfile", "tif", 12, True);         => "C:/MyFile.t"
 *	AddExt("C:/Myfile.tif", "txt", 4, True);      => "C:/MyFile.tif"
 *	AddExt("C:/Myfile.tif", "txt", 100, False);   => "C:/MyFile.tif.txt"
 *	AddExt("C:/Myfile.tif", "", 100, True);       => "C:/MyFile"
 *	AddExt("C:/Myfile.tif", "", 100, False);      => "C:/MyFile.tif"
 *
 * SEE ALSO
 *	GetExtension
 */
DLLIMP void AddExt(STRPTR path, STRPTR ext, int max, Bool replace)
{
	STRPTR dext;

	/* Only take care of last component of path */
	dext = BaseName(path);
	max -= dext - path;
	path = dext;
	dext = strrchr(path, replace ? '.' : 0);

	if (*ext == '.') ext ++;
	if (dext == NULL) dext = strchr(path, 0);
	else if (replace) dext ++;
	if (*ext)
	{
		max -= dext - path;
		if (max > 0 && (dext == path || dext[-1] != '.')) *dext ++ = '.', max --;
		while (max > 0 && *ext) *dext++ = *ext++, max --;
		if (max == 0) dext[-1] = 0; else
		if (max >  0) dext[ 0] = 0;
	}
	else if (replace && dext > path && dext[-1] == '.')
	{
		dext[-1] = 0;
	}
}


/**f* DOS/TimeStamp
 * NAME
 *	TimeStamp - Get timestamp type of given file
 *
 * SYNOPSIS
 *	ULONG TimeStamp(STRPTR path, int type);
 *
 * INPUT
 *	- path : path of file to retrieve given timestamp type
 *	- type : if 0 get creation time,
 *	         if 1 get access time (not always accurate).
 *	         if 2 get modification time.
 *
 * RESULT
 *	Timestamp in seconds relative to January 1st, 1970 or 0 if timestamp cannot
 *	be retrieved.
 */
DLLIMP ULONG TimeStamp(STRPTR path, int type)
{
	struct _stat stbuf;
	LPWSTR dir;
	time_t ret;

	if (path == NULL) return 0;
	dir = StrDupEnc(path, FilenameEncoding);
	ret = 0;

	if (_wstat(dir, &stbuf) == 0)
	{
		switch (type) {
		case 0: ret = stbuf.st_ctime;
		case 1: ret = stbuf.st_atime;
		case 2: ret = stbuf.st_mtime;
		}
	}
	free(dir);
	return ret;
}

/**f* DOS/FileSize
 * NAME
 *	FileSize - Get file size in bytes.
 *
 * SYNOPSIS
 *	ULONG FileSize(STRPTR path);
 *
 * RESULT
 *	File size in bytes or 0 if it cannot be retrieved.
 */
ULONG FileSize(STRPTR file)
{
	LPWSTR path  = StrDupEnc(file, FilenameEncoding);
	HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);

	free(path);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD sz = GetFileSize(hFile, NULL);

		CloseHandle(hFile);
		if (sz != 0xFFFFFFFF) return sz;
	}
	return 0;
}


/**f* DOS/Execute
 * NAME
 *	Execute - Execute a DOS command and wait until it completes.
 *
 * SYNOPSIS
 *	int Execute(STRPTR cmd);
 *
 * FUNCTION
 *	command to execute followed by its possible arguments. Input and output
 *	will be shutdown and possible window (including DOS) will be minimized at
 *	startup.
 *
 * RESULT
 *	Return value of process or -1 if it has not been started.
 */
DLLIMP int Execute(STRPTR cmd)
{
#ifdef	WIN32
	/* Quoted from MinGW runtime library */
	STARTUPINFO sa;
	PROCESS_INFORMATION pi;
	DWORD ec = 1;

	memset(&sa, 0, sizeof(sa));
	memset(&pi, 0, sizeof(pi));
	if (!CreateProcess(0, cmd, 0, 0, 1, 0, 0, 0, &sa, &pi))
	{
		return 0xdeadbeef;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	GetExitCodeProcess(pi.hProcess, &ec);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return ec;
#else /* Posix way */
	int rc = system(cmd);

	return (WIFEXITED(rc) ? WEXITSTATUS(rc) : -1);
#endif
}


/**f* DOS/GetSysError, DOS/GetError
 * NAME
 *	GetSysError - Get error message of last function that indicated a failure code.
 *	GetError    - Get error message in a STATIC buffer.
 *
 * SYNOPSIS
 *	void GetSysError(STRPTR header, STRPTR buffer, int max);
 *	STRPTR GetError(void);
 *
 * FUNCTION
 *	GetSysError will copy into 'buffer' a message suitable to be displayed to user,
 *	that will describe the cause of the last error encountered. Format will be
 *	<header: msg>.
 *
 *	GetError will return the error message in a static buffer and is therefore not
 *	thread safe.
 */
DLLIMP void GetSysError(STRPTR header, STRPTR buffer, int max)
{
	STRPTR ptr;
	int    errcode = GetLastError();

	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL,
	    errcode,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &ptr,
	    0,
	    NULL
	);

	snprintf(buffer, max, "%s: %s", header, ptr);

	LocalFree(ptr);
}

DLLIMP STRPTR GetError(void)
{
	static TEXT buffer[128];

	STRPTR ptr;
	int    errcode = GetLastError();

	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL,
	    errcode,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &ptr,
	    0,
	    NULL
	);

	CopyString(buffer, ptr, sizeof buffer);

	LocalFree(ptr);

	return buffer;
}

/**f* DOS/IsSameFile
 * NAME
 *	IsSameFile - check if two paths point to same physical file.
 *
 * SYNOPSIS
 *	Bool IsSameFile(STRPTR file1, STRPTR file2);
 */
DLLIMP Bool IsSameFile(STRPTR file1, STRPTR file2)
{
	LPWSTR path1 = StrDupEnc(file1, FilenameEncoding);
	LPWSTR path2 = StrDupEnc(file2, FilenameEncoding);
	HANDLE fh1   = CreateFileW(path1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	HANDLE fh2   = CreateFileW(path2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	Bool   ret   = False;

	if (fh1 != INVALID_HANDLE_VALUE && fh2 != INVALID_HANDLE_VALUE)
	{
		BY_HANDLE_FILE_INFORMATION info1, info2;

		if (GetFileInformationByHandle(fh1, &info1) &&
		    GetFileInformationByHandle(fh2, &info2) )
		{
			ret = info1.dwVolumeSerialNumber == info2.dwVolumeSerialNumber &&
			      info1.nFileIndexHigh       == info2.nFileIndexHigh       &&
			      info1.nFileIndexLow        == info2.nFileIndexLow;
		}
		CloseHandle(fh2);
	}
	if (fh1 != INVALID_HANDLE_VALUE) CloseHandle(fh1);
	free(path1);
	free(path2);
	return ret;
}

/**f* DOS/DeleteDOS, DOS/DeletePath
 * NAME
 *	DeleteDOS  - Delete one DOS object from disk.
 *	DeletePath - Delete recursively directory.
 *
 * SYNOPSIS
 *	Bool DeleteDOS(STRPTR path);
 *	Bool DeletePath(STRPTR path);
 *
 * FUNCTION
 *	Attempt to delete specified file/dir from disk. Must have acess right and in
 *	case of a directory, it must be empty.
 *
 *	If you want to delete whatever object, including non-empty directory use
 *	DeletePath.
 */
DLLIMP Bool DeleteDOS(STRPTR path)
{
	LPWSTR dir  = StrDupEnc(path, FilenameEncoding);
	DWORD  attr = GetFileAttributesW(dir);
	BOOL   ret  = (attr != 0xFFFFFFFF && (attr & FILE_ATTRIBUTE_DIRECTORY));

	if (ret)
		ret = RemoveDirectoryW(dir);
	else
		ret = DeleteFileW(dir);

	return ret;
}

static void AddPartW(LPWSTR path, LPWSTR file, int max)
{
	LPWSTR end = wcschr(path, 0);

	if (strchr("/\\", end[-1]) == NULL)
	{
		*end++ = '/';
		max -= end - path;
	}
	CopyStringEnc((STRPTR) end, max, (STRPTR) file, ENC_UTF16);
}

static int DeleteSub(STRPTR path, STRPTR file, APTR data)
{
	LPWSTR dir = (LPWSTR) path;
	int    len = wcslen(dir);
	Bool * ret = data;

	AddPartW(dir, (LPWSTR) file, 1024);

	if (IsDirW(dir))
	{
		ScanDirW(dir, DeleteSub, data);
		if (! *ret || ! RemoveDirectoryW(dir)) { *ret = False; return -1; }
	}
	else if (! DeleteFileW(dir))
	{
		*ret = False;
		return -1;
	}
	dir[len] = 0;
	return 1;
}

DLLIMP Bool DeletePath(STRPTR path)
{
	wchar_t buffer[1024];
	BOOL    ret;

	CopyStringEnc((STRPTR) buffer, DIM(buffer), path, FilenameEncoding);

	if (IsDirW(buffer))
	{
		ret = True;
		ScanDirW(buffer, DeleteSub, &ret);
		if (ret) ret = RemoveDirectoryW(buffer);
	}
	else ret = DeleteFileW(buffer);

	return ret;
}

/**f* DOS/FileCopy
 * NAME
 *	FileCopy - make a copy as close as possible of given file.
 *
 * SYNOPSIS
 *	Bool FileCopy(STRPTR src, STRPTR dest, Bool overwrite);
 *
 * FUNCTION
 *	This function will make a copy as close as possible of original file, including
 *	timestamp and extended attributes (if any).
 *
 * INPUT
 *	src       - Source file to copy.
 *	dest      - Destination file, path must exist.
 *	overwrite - True if overwriting existing file is allowed, False otherwise.
 *
 * SEE ALSO
 *	FileRename, DeleteDOS
 */
DLLIMP Bool FileCopy(STRPTR from, STRPTR to, Bool overwrite)
{
	LPWSTR fromw = StrDupEnc(from, FilenameEncoding);
	LPWSTR tow   = StrDupEnc(to,   FilenameEncoding);

	BOOL ret = CopyFileW(fromw, tow, ! overwrite);

	free(fromw);
	free(tow);

	return ret;
}

/**f* DOS/LockFileMode
 * NAME
 *	LockFileMode - Prevent certain operation on a file.
 *
 * SYNOPSIS
 *	APTR LockFileMode(STRPTR path, int mode);
 *
 * FUNCTION
 *	Prevent other process to perform some operation on a file.
 *
 * INPUT
 *	- path : path of the file to lock.
 *	- mode : bitfield that indicates operations that will be allowed on file after
 *	         it has been successfully locked:
 *	         - LFM_READ  : read allowed
 *	         - LFM_WRITE : modification allowed (including truncating but not delete)
 *
 *	         0 indicates an exclusive lock. No process (including caller) will be
 *	         able to open the file.
 *
 * RESULT
 *	Lock handle or NULL if there was a problem.
 *
 * SEE ALSO
 *	UnlockFileMode
 */
DLLIMP APTR LockFileMode(STRPTR file, int mode)
{
	DWORD  shareMode = 0;
	LPWSTR path = StrDupEnc(file, FilenameEncoding);
	HANDLE h;

	if (mode & LFM_READ)  shareMode |= FILE_SHARE_READ;
	if (mode & LFM_WRITE) shareMode |= FILE_SHARE_WRITE;

	h = CreateFileW(path, GENERIC_READ, shareMode, NULL, OPEN_EXISTING, 0, NULL);
	free(path);

	if (h == INVALID_HANDLE_VALUE) return NULL;

	if (h == NULL)
	{
		LOG_Info("NULL handle returned.\n");
	}
	return (APTR) h;
}

/**f* DOS/UnlockFileMode
 * NAME
 *	UnlockFileMode - release previously locked file.
 *
 * SYNOPSIS
 *	void UnlockFileMode(APTR lock);
 *
 * FUNCTION
 *	Release lock held on file.
 *
 * SEE ALSO
 *	LockFileMode
 */
DLLIMP void UnlockFileMode(APTR lock)
{
	if (lock)
	{
		CloseHandle((HANDLE)lock);
	}
}

/**f* DOS/FileExists
 * NAME
 *	FileExists - check if a file (or directory) exists.
 *
 * SYNOPSIS
 *	Bool FileExists(STRPTR file);
 *
 * SEE ALSO
 *	FileVisible
 */
DLLIMP Bool FileExists(STRPTR file)
{
	LPWSTR path = StrDupEnc(file, FilenameEncoding);
	DWORD  ret = GetFileAttributesW(path);

	free(path);

	return ret != (DWORD) -1;
}

/**f* DOS/FileVisible
 * NAME
 *	FileVisible - check if a file (or directory) is not normally hidden from OS.
 *
 * SYNOPSIS
 *	Bool FileVisible(STRPTR file);
 *
 * SEE ALSO
 *	FileExists
 */
DLLIMP Bool FileVisible(STRPTR file)
{
	LPWSTR path = StrDupEnc(file, FilenameEncoding);
	DWORD  ret = GetFileAttributesW(path);

	free(path);

	return ret != (DWORD) -1 && ! (ret & FILE_ATTRIBUTE_HIDDEN);
}

/**f* DOS/FileRename
 * NAME
 *	FileRename - rename and/or move a file/directory
 *
 * SYNOPSIS
 *	Bool FileRename(STRPTR from, STRPTR to, Bool overwrite);
 *
 * FUNCTION
 *	Will rename and/or move the given file/directory to the new name/location.
 *	If new location is on a different partition, file will be copied instead
 *	(note : if it is a directory, the operation will fail). If overwrite is
 *	True, possible destination file will be overwritten if it exists.
 *
 * SEE ALSO
 *	DeletePath, FileCopy
 */
DLLIMP Bool FileRename(STRPTR from, STRPTR to, Bool overwrite)
{
	LPWSTR fromw = StrDupEnc(from, FilenameEncoding);
	LPWSTR tow   = StrDupEnc(to,   FilenameEncoding);
	DWORD  flags = overwrite ? MOVEFILE_REPLACE_EXISTING : 0;

	BOOL ret = MoveFileExW(fromw, tow, MOVEFILE_COPY_ALLOWED | flags);

	free(fromw);
	free(tow);

	return ret;
}

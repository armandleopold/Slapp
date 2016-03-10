/*
 * Thread.c : simple wrapper for thread related functions
 *
 * Written by T.Pierron, Oct 2005.
 */

#include "SWL.h"
#include <windows.h>
#include <process.h>

/**h* SWL/Thread
 * INTRODUCTION
 *	Simple module to create thread and usual synchronization objects (Mutex &
 *	semaphore).
 *
 *	As usual, no safeguards are provided. Threads are as easy to start as it is
 *	to shoot yourself in your foot with them.
 */

/**f* Thread/MutexCreate
 * NAME
 *	MutexCreate - critical section safe guard
 *
 * SYNOPSIS
 *	Mutex MutexCreate(void);
 *
 * FUNCTION
 *	Create a synchronization object that will allow you to run atomic code.
 *	Like usual beware of deadlock, eitheir by forgetting to release the mutex,
 *	or by locking a set of mutexes in different order in different threads.
 *
 * SEE ALSO
 *	MutexEnter, MutexLeave, MutexTryEnter, MutexDestroy
 */
DLLIMP Mutex MutexCreate(void)
{
	LPCRITICAL_SECTION cs = malloc(sizeof *cs);

	if (cs) InitializeCriticalSection(cs);

	return cs;
}

/**f* Thread/MutexEnter, Thread/MutexLeave, Thread/MutexTryEnter
 * NAME
 *	MutexEnter    - Ask for access to the lock. Blocking.
 *	MutexLeave    - Permit other thread to have access to the lock.
 *	MutexTryEnter - Ask for access to the lock. Non-blocking.
 *
 * SYNOPSIS
 *	void MutexEnter(Mutex lock);
 *	void MutexLeave(Mutex lock);
 *	Bool MutexTryEnter(Mutex lock);
 *
 * FUNCTION
 *	MutexEnter will wait until the calling thread can access the lock. Once you
 *	gain access, all other threads will block by calling MutexEnter on the mutex,
 *	and until you call MutexLeave.
 *
 *	You can safely call multiple times MutexEnter on the same mutex, within the
 *	same thread, but you must have the same number of MutexLeave call to release
 *	the lock held on the mutex by your Thread.
 *
 *	MutexTryEnter will ask for access to the lock. If it can obtain it
 *	immediately, the mutex will be locked by your thread and the function will
 *	return True. Otherwise mutex ownership won't be changed and False will be
 *	returned.
 *
 *	If mutiple threads are waiting on locking a given mutex, there is no
 *	guarantee on which one will get access. It might very well cause the well
 *	known effect of starvation for one of them.
 *
 *	Functions are NULL-safe.
 *
 * RESULT
 *	MutexTryEnter will return True if lock has been acquired on the mutex. False
 *	otherwise.
 *
 * SEE ALSO
 *	MutexCreate
 */
DLLIMP void MutexEnter(Mutex lock)
{
	if (lock) EnterCriticalSection(lock);
}

DLLIMP void MutexLeave(Mutex lock)
{
	if (lock) LeaveCriticalSection(lock);
}

DLLIMP Bool MutexTryEnter(Mutex lock)
{
	if (lock)
	{
		return TryEnterCriticalSection(lock);
	}
	return False;
}

/**f* Thread/MutexDestroy
 * NAME
 *	MutexDestroy - release memory held by lock
 *
 * SYNOPSIS
 *	void MutexDestroy(Mutex lock);
 */
DLLIMP void MutexDestroy(Mutex lock)
{
	if (lock)
	{
		DeleteCriticalSection(lock);
		free(lock);
	}
}

/**f* Thread/ThreadCreate
 * NAME
 *	ThreadCreate - start an asynchronous program flow.
 *
 * SYNOPSIS
 *	Thread ThreadCreate(ThreadCb func, APTR arg);
 *
 * FUNCTION
 *	Starts a new execution flow, independant of the caller thread.
 *
 *	Be sure to link your program with a thread safe standard C library, or at
 *	least avoiding non-thread safe functions in your thread (which might go as
 *	far as not using malloc if linked to the wrong library).
 *
 * RESULT
 *	An opaque ID with which you cannot do much, except checking if thread
 *	creation has succeeded or not. You cannot cancel thread execution, nor send
 *	a signal to it. You cannot know whan thread exited, you have to use
 *	synchronization objects.
 *
 * SEE ALSO
 *	MutexCreate, SemInit
 */
DLLIMP Thread ThreadCreate(ThreadCb func, APTR arg)
{
	uintptr_t tid = _beginthread(func, 0, arg);

	return tid;
}

/**f* Thread/SemInit
 * NAME
 *	SemInit - Create a semaphore object.
 *
 * SYNOPSIS
 *	Semaphore SemInit(int count);
 *
 * FUNCTION
 *	Create a semaphore with the specified amount as starting value. Semaphore
 *	are a bit more generic than Mutex, though the former can be emulated using
 *	an integer and two mutexes.
 *
 * SEE ALSO
 *	SemWait, SemAdd
 */
DLLIMP Semaphore SemInit(int count)
{
	return (Semaphore) CreateSemaphore(NULL, count, 32767, NULL);
}

/**f* Thread/SemWait
 * NAME
 *	SemWait - Remove one unit in the semaphore or wait until one arrives if count is 0.
 *
 * SYNOPSIS
 *	Bool SemWait(Semaphore s);
 *
 * FUNCTION
 *	This will decrease the count of the integer contained in the semaphore. If
 *	the current count is 0, it will wait infinately until it reach one. If
 *	multiple threads are waiting for a unit to be released, there will be no
 *	guarantee on which thread will be awaken.
 *
 * SEE ALSO
 *	SemAdd
 */
DLLIMP Bool SemWait(Semaphore s)
{
	return WaitForSingleObject((HANDLE) s, INFINITE) == WAIT_OBJECT_0;
}

/**f* Thread/SemAdd
 * NAME
 *	SemAdd - Add amount to currrent value in semaphore.
 *
 * SYNOPSIS
 *	void SemAdd(Semaphore s, int count);
 *
 * FUNCTION
 *	Increase the value of the semaphore to the specified amount, releasing
 *	potential thread waiting for it.
 *
 * SEE ALSO
 *	SemWait
 */
DLLIMP void SemAdd(Semaphore s, int count)
{
	ReleaseSemaphore((HANDLE) s, count, NULL);
}

/**f* Thread/SemClose
 * NAME
 *	SemClose - release memory held by semaphore.
 *
 * SYNOPSIS
 *	void SemClose(Semaphore s);
 */
DLLIMP void SemClose(Semaphore s)
{
	CloseHandle(s);
}

/**f* Thread/ThreadPause
 * NAME
 *	ThreadPause - Pause current thread.
 *
 * SYNOPSIS
 *	void ThreadPause(int delay);
 *
 * FUNCTION
 *	Pause the current thread of the specified amount of miliseconds. This
 *	delay is uninterruptible.
 */
DLLIMP void ThreadPause(int delay)
{
	Sleep(delay);
}

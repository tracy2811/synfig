/* === S I N F G =========================================================== */
/*!	\file mutex.cpp
**	\brief Template File
**
**	$Id: mutex.cpp,v 1.3 2005/01/12 00:55:46 darco Exp $
**
**	\legal
**	Copyright (c) 2002 Robert B. Quattlebaum Jr.
**
**	This software and associated documentation
**	are CONFIDENTIAL and PROPRIETARY property of
**	the above-mentioned copyright holder.
**
**	You may not copy, print, publish, or in any
**	other way distribute this software without
**	a prior written agreement with
**	the copyright holder.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "mutex.h"

#ifdef HAVE_LIBPTHREAD
#define USING_PTHREADS 1
#else
#ifdef _WIN32
#define USING_WIN32_THREADS 1
#endif
#endif

#ifdef USING_WIN32_THREADS
#include <windows.h>
#endif

#ifdef USING_PTHREADS
#include <pthread.h>
#endif

#endif

/* === U S I N G =========================================================== */

//using namespace std;
//using namespace etl;
using namespace sinfg;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */






bool
Mutex::is_locked()
{
	if(try_lock())
	{
		unlock();
		return false;
	}
	return true;
}

void
RecMutex::unlock_all()
{
	while(is_locked()) unlock();
}

#ifdef USING_PTHREADS
Mutex::Mutex()
{
	pthread_mutex_t*const mtx_ptr(new pthread_mutex_t);

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);

	//#ifdef PTHREAD_PRIO_INHERIT
	//pthread_mutexattr_setprioceiling(&attr,PTHREAD_PRIO_INHERIT);
	//#endif

	//#ifdef PTHREAD_MUTEX_RECURSIVE
	//pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
	//#endif

	pthread_mutex_init(mtx_ptr,&attr);
	pthread_mutexattr_destroy(&attr);
	
	blackbox=mtx_ptr;
}

Mutex::~Mutex()
{
	pthread_mutex_t*const mtx_ptr(static_cast<pthread_mutex_t*>(blackbox));
	pthread_mutex_destroy(mtx_ptr);
	delete mtx_ptr;
}

void
Mutex::lock()
{
	pthread_mutex_t*const mtx_ptr(static_cast<pthread_mutex_t*>(blackbox));
	pthread_mutex_lock(mtx_ptr);
}

void
Mutex::unlock()
{
	pthread_mutex_t*const mtx_ptr(static_cast<pthread_mutex_t*>(blackbox));
	pthread_mutex_unlock(mtx_ptr);
}

bool
Mutex::try_lock()
{
	pthread_mutex_t*const mtx_ptr(static_cast<pthread_mutex_t*>(blackbox));
	return !(bool) pthread_mutex_trylock(mtx_ptr);
}


RecMutex::RecMutex()
{
	pthread_mutex_t*const mtx_ptr(static_cast<pthread_mutex_t*>(blackbox));
	pthread_mutexattr_t attr;

	// Backtrack and get rid of the non-recursive mutex
	pthread_mutex_destroy(mtx_ptr);

	pthread_mutexattr_init(&attr);

	pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(mtx_ptr,&attr);
	pthread_mutexattr_destroy(&attr);
}



RWLock::RWLock()
{
	pthread_rwlock_t*const rwlock_ptr(new pthread_rwlock_t);
	
	pthread_rwlock_init(rwlock_ptr, NULL);
	
	blackbox=rwlock_ptr;
}

RWLock::~RWLock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	pthread_rwlock_destroy(rwlock_ptr);
	
	delete rwlock_ptr;
}

void
RWLock::reader_lock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	pthread_rwlock_rdlock(rwlock_ptr);
}

void
RWLock::reader_unlock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	pthread_rwlock_unlock(rwlock_ptr);
}

bool
RWLock::reader_trylock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	return !pthread_rwlock_tryrdlock(rwlock_ptr);
}

void
RWLock::writer_lock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	pthread_rwlock_wrlock(rwlock_ptr);
}

void
RWLock::writer_unlock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	pthread_rwlock_unlock(rwlock_ptr);
}

bool
RWLock::writer_trylock()
{
	pthread_rwlock_t*const rwlock_ptr(static_cast<pthread_rwlock_t*>(blackbox));

	return !pthread_rwlock_trywrlock(rwlock_ptr);
}

#endif

#ifdef USING_WIN32_THREADS
Mutex::Mutex()
{
	HANDLE& mtx(*reinterpret_cast<HANDLE*>(&blackbox));
	mtx=CreateMutex(NULL, FALSE, NULL);
}

Mutex::~Mutex()
{
	HANDLE mtx(reinterpret_cast<HANDLE>(blackbox));
	CloseHandle(mtx);
}

void
Mutex::lock()
{
	HANDLE mtx(reinterpret_cast<HANDLE>(blackbox));
	WaitForSingleObject(mtx, INFINITE);
}

void
Mutex::unlock()
{
	HANDLE mtx(reinterpret_cast<HANDLE>(blackbox));
	ReleaseMutex(mtx);
}

bool
Mutex::try_lock()
{
	HANDLE mtx(reinterpret_cast<HANDLE>(blackbox));
	return WaitForSingleObject(mtx, 0)==WAIT_FAILED;
}


RecMutex::RecMutex()
{
	// Win32 mutexes are recursive by default.
}


RWLock::RWLock()
{
}

RWLock::~RWLock()
{
}

void
RWLock::reader_lock()
{
}

void
RWLock::reader_unlock()
{
}

bool
RWLock::reader_trylock()
{
}

void
RWLock::writer_lock()
{
}

void
RWLock::writer_unlock()
{
}

bool
RWLock::writer_trylock()
{
}

#endif

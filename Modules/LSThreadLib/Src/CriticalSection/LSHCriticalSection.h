/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A wrapper for critical sections that works on all platforms (give or take).
 *	Always use CLocker() to lock critical sections unless there is a good reason not to do so, such as
 *	when you want to keep an object locked after a function call has ended.
 * Alternatively, for critical sections that only needs to be locked in multi-threaded builds, use
 *	CLockerS.
 * The reason both exist is because it is possible for a game to turn off multi-threading but still use a
 *	second special-case thread.  For example, the whole engine may itself be single-threaded, but run on
 *	a second thread while the main thread receives OS events.  The exhange of events between the two threads
 *	always needs to be multi-threaded and must use CLocker.  The rest of the engine, however, can be set to
 *	single-threaded use and can use CLockerS.
 * Most cases should use CLockerS.  Use CLocker only when there are special-case manually-handled threads
 *	that are always created even when the engine itself is set to run in single-threaded mode.
 */


#ifndef __LSH_CRITICALSECTION_H__
#define __LSH_CRITICALSECTION_H__

#include "../LSHThreadLib.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * Some types of locks can be removed in single-threaded applications.  This macro changes the
 *	CLockerS class so that it does not lock the target critical section.
 */
//#define LSH_SINGLE_THREADED


namespace lsh {

	/**
	 * Class CCriticalSection
	 *
	 * Description: Cross-platform critical sections.
	 */
	class CCriticalSection {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						CCriticalSection();
		LSE_INLINE LSE_CALLCTOR						~CCriticalSection();


		// == Types.
		/**
		 * A helper class that reliably locks and unlocks a critical section.
		 */
		class CLocker {
		public :
			// == Various constructors.
			LSE_INLINE LSE_CALLCTOR					CLocker( CCriticalSection &_csLockMe ) {
				m_pcsSection = &_csLockMe;
				m_pcsSection->Lock();
			}
			LSE_INLINE LSE_CALLCTOR					~CLocker() {
				m_pcsSection->UnLock();
			}

		private :
			// == Members.
			/**
			 * The section we lock and unlock.
			 */
			CCriticalSection *						m_pcsSection;
		};

		/**
		 * Another helper which changes behavior depending on the LSH_SINGLE_THREADED
		 *	macro.
		 */
		class CLockerS {
		public :
			// == Various constructors.
			LSE_INLINE LSE_CALLCTOR					CLockerS( CCriticalSection &_csLockMe ) {
#ifndef LSH_SINGLE_THREADED
				m_pcsSection = &_csLockMe;
				m_pcsSection->Lock();
#else	// #ifndef LSH_SINGLE_THREADED
				// Remove warning C4100.
				static_cast<CCriticalSection &>(_csLockMe);
#endif	// #ifndef LSH_SINGLE_THREADED
			}
			LSE_INLINE LSE_CALLCTOR					~CLockerS() {
#ifndef LSH_SINGLE_THREADED
				m_pcsSection->UnLock();
#endif	// #ifndef LSH_SINGLE_THREADED
			}

		private :
#ifndef LSH_SINGLE_THREADED
			// == Members.
			// The section we lock and unlock.
			CCriticalSection *						m_pcsSection;
#endif	// #ifndef LSH_SINGLE_THREADED
		};


		// == Functions.
		// Lock the critical section.
		LSE_INLINE void LSE_CALL					Lock();

		// Unlock the critical section.
		LSE_INLINE void LSE_CALL					UnLock();

	protected :
		// == Members.
#ifdef LSE_WIN32
		CRITICAL_SECTION							m_csCrit;
#elif defined( LSE_MAC )
		pthread_mutex_t								m_ptmCrit;
#endif	// #ifdef LSE_WIN32
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CCriticalSection::CCriticalSection() {
#ifdef LSE_WIN32
		::InitializeCriticalSection( &m_csCrit );
#elif defined( LSE_MAC )
		pthread_mutexattr_t tmaAtt;
		::pthread_mutexattr_init( &tmaAtt );
		::pthread_mutexattr_settype( &tmaAtt, PTHREAD_MUTEX_RECURSIVE );
		::pthread_mutex_init( &m_ptmCrit, &tmaAtt );
		::pthread_mutexattr_destroy( &tmaAtt );
#endif	// #ifdef LSE_WIN32
	}
	LSE_INLINE LSE_CALLCTOR CCriticalSection::~CCriticalSection() {
#ifdef LSE_WIN32
		::DeleteCriticalSection( &m_csCrit );
#elif defined( LSE_MAC )
		::pthread_mutex_destroy( &m_ptmCrit );
#endif	// #ifdef LSE_WIN32
	}

	// == Functions.
	// Lock the critical section.
	LSE_INLINE void LSE_CALL CCriticalSection::Lock() {
#ifdef LSE_WIN32
		::EnterCriticalSection( &m_csCrit );
#elif defined( LSE_MAC )
		::pthread_mutex_lock( &m_ptmCrit );
#endif	// #ifdef LSE_WIN32
	}

	// Unlock the critical section.
	LSE_INLINE void LSE_CALL CCriticalSection::UnLock() {
#ifdef LSE_WIN32
		::LeaveCriticalSection( &m_csCrit );
#elif defined( LSE_MAC )
		::pthread_mutex_unlock( &m_ptmCrit );
#endif	// #ifdef LSE_WIN32
	}

}	// namespace lsh

#endif	// __LSH_CRITICALSECTION_H__

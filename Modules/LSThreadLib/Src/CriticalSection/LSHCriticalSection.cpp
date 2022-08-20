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

#include "LSHCriticalSection.h"


namespace lsh {

}	// namespace lsh

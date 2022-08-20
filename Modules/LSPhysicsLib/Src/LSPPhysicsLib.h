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
 * Description: The main header for the physics library.  Includes the main headers for the other libraries on
 *	which the physics library depends.
 */


#ifndef __LSP_PHYSICSLIB_H__
#define __LSP_PHYSICSLIB_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the math library.
#include "LSMMathLib.h"
using namespace ::lsm;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the thread library.
#include "LSHThreadLib.h"
using namespace ::lsh;

// Include the standard library.
#include "LSTLib.h"
using namespace ::lstl;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * The performance desired.  Certain calculations
 *	are modified depending on this, losing precision
 *	in favor of speed or vice-versa.
 * Values from 0 to 4 are allowed.
 *	0 = Fastest, but less accuracy.
 *	4 =	Slowest, but more accurate.
 */
#define LSP_PERFORMANCE					2

namespace lsp {

	/**
	 * Class CPhysicsLib
	 * \brief The main header for the physics library.
	 *
	 * Description: The main header for the physics library.  Includes the main headers for the other libraries on
	 *	which the physics library depends.
	 */
	class CPhysicsLib {
	public :


		// == Functions.


	protected :
		// == Members.
	};

}	// namespace lsp

#endif	// __LSP_PHYSICSLIB_H__

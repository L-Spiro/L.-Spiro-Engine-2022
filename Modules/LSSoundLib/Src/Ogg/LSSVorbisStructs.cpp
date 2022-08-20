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
 * Description: The Vorbis and Ogg libraries are extremely messy.  The locations of certain structures is
 *	fairly illogical, functions that are declared in a single header are defined in multiple translation units,
 *	and #include's are thrown around everywhere, not just at the tops of files.
 * Circular dependencies are created all over the place when the code is restructured properly.  To eliminate this,
 *	we place some structures that do not rely on any other structures in this class/file.
 */

#include "LSSVorbisStructs.h"

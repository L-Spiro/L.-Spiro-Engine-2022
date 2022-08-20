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
 * Description: A large-integer template class.  Represents a number containing at most _uMaxDigits digits in
 *	radix 10.  Provides standard operators synthesized in software mode.  This class is not intended for use
 *	in any high-performance situation.  Use this class when precision is more important than speed.
 *
 * Multiple instances of this class can cause a heavy hit to the stack unless allocated to the heap.
 */

#include "LSSTDLargeInteger.h"

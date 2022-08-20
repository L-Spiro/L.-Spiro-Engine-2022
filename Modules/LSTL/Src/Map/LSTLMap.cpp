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
 * Description: An associative container pairing a unique key with a mapped value.  The key and value do not
 *	need to be of the same type.
 *
 * Values are accessed via their keys, so each key is unique.  Mapped values do not need to be unique.  Mapped
 *	values are sorted, and the key type must implement the == and < operators.
 */

#include "LSTLLinearMap.h"

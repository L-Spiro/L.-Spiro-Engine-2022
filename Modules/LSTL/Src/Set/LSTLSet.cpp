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
 * Description: An associate container such that each element is the key itself.  CLinearMap's associate keys with
 *	values, whereas CSet simply keeps a list of keys.  Like with CLinearMap, each key is unique in the list, and
 *	sorted by key value.
 * Because keys are sorted, the key type must implement the == and < operators.
 */

#include "LSTLSet.h"



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
 * Description: A 3D plane stored in constant-normal form (normal + distance from origin).
 *	Note that the plane is stored such that ax + by + cx - d = 0 instead of ax + by + cx + d = 0
 *	to avoid the superfluous negation that frequently occurs in math related to planes.
 */

#include "LSMPlane3.h"

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
 * Description: Add more streamer types, because the core sound engine only defines the base
 *	streaming type which cannot even be instanciated.
 */


#ifndef __LSS_STREAMERTYPES_H__
#define __LSS_STREAMERTYPES_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Adds more streamer types, because the core sound engine only defines the base
	 *	streaming type which cannot even be instanciated.
	 */
	enum LSS_STREAMERTYPES {
		LSS_ST_WAVEFILE					= (1 << 1),
		LSS_ST_OGGFILE					= (1 << 2),
	};

}	// namespace lss

#endif	// __LSS_STREAMERTYPES_H__

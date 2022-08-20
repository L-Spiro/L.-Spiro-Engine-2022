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
 * Description: Wrappers for the OS-dependent heap functions.
 */

#include "LSAOsHeap.h"
#include <Foundation/Foundation.h>


namespace lsa {

#ifdef LSE_IPHONE
	/**
	 * Creates a path to a file that is to be used for memory mapping.
	 *
	 * \param _ui32Index Index of the file to be created.  Used as part of the file name.
	 * \param _pcReturn The character array to be filled with the returned path.
	 */
	LSVOID LSE_CALL COsHeap::MakeMemMapPath( LSUINT32 _ui32Index, LSCHAR * _pcReturn ) {
		(*_pcReturn) = '\0';
		NSArray * paPaths = NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES );
		if ( paPaths.count ) {
			NSString * psString = [paPaths objectAtIndex:0];
			NSString * psFinal = [NSString stringWithFormat:@"%@/mmap%u", psString, _ui32Index];
			LSUINT32 ui32Len = CStd::StrLen( [psFinal UTF8String] );
			CStd::MemCpy( _pcReturn, [psFinal UTF8String], ui32Len + 1 );
		}
	}
#endif	// #ifdef LSE_IPHONE

}	// namespace lsa

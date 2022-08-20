/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: registry for time, floor, res backends and channel mappings
 last mod: $Id: registry.c 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISREGISTRY_H__
#define __LSS_VORBISREGISTRY_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisBackends.h"

namespace lss {

	/**
	 * Class CVorbisRegistry
	 * \brief Registry for time, floor, res backends and channel mappings.
	 *
	 * Description: Registry for time, floor, res backends and channel mappings.
	 */
	class CVorbisRegistry {
	public :

		// == Members.
		/**
		 * Undocumented Vorbis library data.
		 */
		static const CVorbisBackends::vorbis_func_floor * const				_floor_P[];

		/**
		 * Undocumented Vorbis library data.
		 */
		static const CVorbisBackends::vorbis_func_residue * const			_residue_P[];

		/**
		 * Undocumented Vorbis library data.
		 */
		static const CVorbisBackends::vorbis_func_mapping * const			_mapping_P[];
	};

}	// namespace lss

#endif	// __LSS_VORBISREGISTRY_H__


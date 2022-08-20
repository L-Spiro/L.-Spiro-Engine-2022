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

#include "LSSVorbisRegistry.h"
#include "LSSVorbisFloor0.h"
#include "LSSVorbisFloor1.h"
#include "LSSVorbisMapping0.h"
#include "LSSVorbisRes0.h"


namespace lss {

	// == Members.
	/**
	 * Undocumented Vorbis library data.
	 */
	const CVorbisBackends::vorbis_func_floor * const CVorbisRegistry::_floor_P[] = {
		&CVorbisFloor0::floor0_exportbundle,
		&CVorbisFloor1::floor1_exportbundle,
	};

	/**
	 * Undocumented Vorbis library data.
	 */
	const CVorbisBackends::vorbis_func_residue * const CVorbisRegistry::_residue_P[] = {
		&CVorbisRes0::residue0_exportbundle,
		&CVorbisRes0::residue1_exportbundle,
		&CVorbisRes0::residue2_exportbundle,
	};

	/**
	 * Undocumented Vorbis library data.
	 */
	const CVorbisBackends::vorbis_func_mapping * const CVorbisRegistry::_mapping_P[] = {
		&CVorbisMapping0::mapping0_exportbundle
	};

}	// namespace lss

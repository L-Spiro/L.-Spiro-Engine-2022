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


#ifndef __LSS_VORBISSTRUCTS_H__
#define __LSS_VORBISSTRUCTS_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisConstants.h"

namespace lss {

	/**
	 * Class CVorbisStructs
	 * \brief Various structures from the Vorbis library that can't be placed anywhere else.
	 *
	 * Description: The Vorbis and Ogg libraries are extremely messy.  The locations of certain structures is
	 *	fairly illogical, functions that are declared in a single header are defined in multiple translation units,
	 *	and #include's are thrown around everywhere, not just at the tops of files.
	 * Circular dependencies are created all over the place when the code is restructured properly.  To eliminate this,
	 *	we place some structures that do not rely on any other structures in this class/file.
	 */
	class CVorbisStructs {
	public :
		// == Types.
		/**
		 * A Vorbis type.
		 */
		typedef void							vorbis_info_floor;

		/**
		 * A Vorbis type.
		 */
		typedef void							vorbis_info_residue;

		/**
		 * A Vorbis type.
		 */
		typedef void							vorbis_info_mapping;

		/**
		 * A Vorbis type.
		 */
		typedef void							vorbis_look_floor;

		/**
		 * A Vorbis type.
		 */
		typedef void							vorbis_look_residue;

		/**
		 * A Vorbis type.
		 */
		typedef void							vorbis_look_transform;
		
		/**
		 * Vorbis info mode.
		 */
		typedef struct vorbis_info_mode {
			int32_t								blockflag;
			int32_t								windowtype;
			int32_t								transformtype;
			int32_t								mapping;
		} * LPvorbis_info_mode, * const LPCvorbis_info_mode;

		/**
		 * A Vorbis structure.
		 */
		typedef struct vorbis_info_mapping0 {
			int32_t								submaps;  /* <= 16 */
			int32_t								chmuxlist[256];   /* up to 256 channels in a Vorbis stream */

			int32_t								floorsubmap[16];   /* [mux] submap to floors */
			int32_t								residuesubmap[16]; /* [mux] submap to residue */

			int32_t								coupling_steps;
			int32_t								coupling_mag[256];
			int32_t								coupling_ang[256];

		} * LPvorbis_info_mapping0, * const LPCvorbis_info_mapping0;

		/**
		 * A Vorbis floor-related structure.
		 */
		typedef struct lsfit_acc {
			int32_t								x0;
			int32_t								x1;

			int32_t								xa;
			int32_t								ya;
			int32_t								x2a;
			int32_t								y2a;
			int32_t								xya;
			int32_t								an;

			int32_t								xb;
			int32_t								yb;
			int32_t								x2b;
			int32_t								y2b;
			int32_t								xyb;
			int32_t								bn;
		} * LPlsfit_acc, * const LPClsfit_acc;

	};

}	// namespace lse

#endif	// __LSS_VORBISSTRUCTS_H__


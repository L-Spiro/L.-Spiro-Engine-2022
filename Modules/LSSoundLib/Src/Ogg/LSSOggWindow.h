/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: window functions
 last mod: $Id: window.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/


#ifndef __LSS_OGGWINDOW_H__
#define __LSS_OGGWINDOW_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class COggWindow
	 * \brief Ogg window functions.
	 *
	 * Description: Ogg window functions.
	 */
	class COggWindow {
	public :

		// == Functions.
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _i32N Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static float *		_vorbis_window_get( int32_t _i32N );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pfD Undocumented parameter from the Ogg library.
		 * \param _pi32WinNo Undocumented parameter from the Ogg library.
		 * \param _pi32BlockSizes Undocumented parameter from the Ogg library.
		 * \param _i32Lw Undocumented parameter from the Ogg library.
		 * \param _i32W Undocumented parameter from the Ogg library.
		 * \param _i32Nw Undocumented parameter from the Ogg library.
		 */
		static void				_vorbis_apply_window( float * _pfD, int32_t * _pi32WinNo, int32_t * _pi32BlockSizes,
			int32_t _i32Lw, int32_t _i32W, int32_t _i32Nw );

	protected :
		// == Members.
		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin64[32];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin128[64];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin256[128];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin512[256];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin1024[512];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin2048[1024];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin4096[2048];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float			m_fWin8192[4096];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static float *		m_pfWin[8];
		
	};

}	// namespace lss

#endif	// __LSS_OGGWINDOW_H__


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

  function: LSP (also called LSF) conversion routines
  last mod: $Id: lsp.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISLSP_H__
#define __LSS_VORBISLSP_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class CVorbisLsp
	 * \brief LSP (also called LSF) conversion routines.
	 *
	 * Description: LSP (also called LSF) conversion routines.
	 */
	class CVorbisLsp {
	public :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfLpc Undocumented parameter from the Vorbis library.
		 * \param _pfLsp Undocumented parameter from the Vorbis library.
		 * \param _i32M Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t						vorbis_lpc_to_lsp( float * _pfLpc, float * _pfLsp, int32_t _i32M );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfCurve Undocumented parameter from the Vorbis library.
		 * \param _pi32Map Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _i32Ln Undocumented parameter from the Vorbis library.
		 * \param _pfLsp Undocumented parameter from the Vorbis library.
		 * \param _i32M Undocumented parameter from the Vorbis library.
		 * \param _fAmp Undocumented parameter from the Vorbis library.
		 * \param _fAmpOffset Undocumented parameter from the Vorbis library.
		 */
		static void						vorbis_lsp_to_curve( float * _pfCurve, int32_t * _pi32Map, int32_t _i32N, int32_t _i32Ln, 
			float * _pfLsp, int32_t _i32M, 
			float _fAmp, float _fAmpOffset );


	protected :
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfG Undocumented parameter from the Vorbis library.
		 * \param _i32Ord Undocumented parameter from the Vorbis library.
		 */
		static void						cheby( float * _pfG, int32_t _i32Ord );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param _i32Ord Undocumented parameter from the Vorbis library.
		 * \param _pfR Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t						Laguerre_With_Deflation( float * _pfA, int32_t _i32Ord, float * _pfR );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param _i32Ord Undocumented parameter from the Vorbis library.
		 * \param _pfR Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t						Newton_Raphson( float * _pfA, int32_t _i32Ord, float * _pfR );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvContext Unused.
		 * \param _pvA Undocumented parameter from the Vorbis library.
		 * \param _pvB Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t LSE_CCALL			comp( void * _pvContext, const void * _pvA, const void * _pvB );
	};

}	// namespace lss

#endif	// __LSS_VORBISLSP_H__


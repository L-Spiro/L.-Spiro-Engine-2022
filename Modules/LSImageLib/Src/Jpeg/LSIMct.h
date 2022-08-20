/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LSI_MCT_H__
#define __LSI_MCT_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CMct
	 * \brief Implementation of a multi-component transforms (MCT).
	 *
	 * Description: Implementation of a multi-component transforms (MCT).
	 */
	class CMct {
	public :
		// == Functions.
		/**
		 * Apply a reversible multi-component transform to an image.
		 *
		 * \param _pi32C0 Samples for red component
		 * \param _pi32C1 Samples for green component
		 * \param _pi32C2 Samples blue component
		 * \param _i32N Number of samples for each component
		 */
		static void LSE_CALL					mct_encode( int32_t * _pi32C0, int32_t * _pi32C1, int32_t * _pi32C2, int32_t _i32N );
		
		/**
		 * Apply a reversible multi-component inverse transform to an image.
		 *
		 * \param _pi32C0 Samples for luminance component.
		 * \param _pi32C1 Samples for red chrominance component.
		 * \param _pi32C2 Samples for blue chrominance component.
		 * \param _i32N Number of samples for each component.
		 */
		static void LSE_CALL					mct_decode( int32_t * _pi32C0, int32_t * _pi32C1, int32_t * _pi32C2, int32_t _i32N );
		
		/**
		 * Gets norm of the basis function used for the reversible multi-component transform.
		 *
		 * \param _i32CompNo Number of the component (0->Y, 1->U, 2->V).
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static double LSE_CALL				mct_getnorm( int32_t _i32CompNo );

		/**
		 * Apply an irreversible multi-component transform to an image.
		 *
		 * \param _pi32C0 Samples for red component.
		 * \param _pi32C1 Samples for green component.
		 * \param _pi32C2 Samples blue component.
		 * \param _i32N Number of samples for each component.
		 */
		static void LSE_CALL					mct_encode_real( int32_t * _pi32C0, int32_t * _pi32C1, int32_t * _pi32C2, int32_t _i32N );
		
		/**
		 * Apply an irreversible multi-component inverse transform to an image.
		 *
		 * \param _pfC0 Samples for luminance component.
		 * \param _pfC1 Samples for red chrominance component.
		 * \param _pfC2 Samples for blue chrominance component.
		 * \param _i32N Number of samples for each component.
		 */
		static void LSE_CALL					mct_decode_real( float * _pfC0, float * _pfC1, float * _pfC2, int32_t _i32N );
		
		/**
		 * Gets norm of the basis function used for the irreversible multi-component transform.
		 *
		 * \param _i32CompNo Number of the component (0->Y, 1->U, 2->V).
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static double LSE_CALL				mct_getnorm_real( int32_t _i32CompNo );


	protected :
		// == Members.
		/**
		 * This table contains the normals of the basis function of the reversible MCT.
		 */
		static const double					m_dNorms[3];

		/**
		 * This table contains the norms of the basis function of the irreversible MCT.
		 */
		static const double					m_dNormsReal[3];
	};

}	// namespace lsi

#endif	// __LSI_MCT_H__

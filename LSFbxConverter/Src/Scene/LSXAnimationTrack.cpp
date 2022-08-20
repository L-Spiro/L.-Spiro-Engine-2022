/**
 * Copyright L. Spiro 2012
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: The FBX SDK calls these curves.  A track/curve is a collection of values at keyframes, nothing more.
 */

#include "LSXAnimationTrack.h"


namespace lsx {

	// == Operators.
	/**
	 * Less-than operator.
	 *
	 * \param _kfOther The keyframe against which to compare.
	 * \return Returns true if the time of this keyframe is less than the time of the given
	 *	keyframe.
	 */
	LSBOOL LSE_CALL CAnimationTrack::LSX_KEY_FRAME::operator < ( const LSX_KEY_FRAME &_kfOther ) const {
		return tTime < _kfOther.tTime;
	}

	/**
	 * Equality operator.
	 *
	 * \param _kfOther The keyframe against which to compare.
	 * \return Returns true if the time of this keyframe is equal to the time of the given
	 *	keyframe.
	 */
	LSBOOL LSE_CALL CAnimationTrack::LSX_KEY_FRAME::operator == ( const LSX_KEY_FRAME &_kfOther ) const {
		return tTime == _kfOther.tTime;
	}

	// == Various constructors.
	LSE_CALLCTOR CAnimationTrack::CAnimationTrack() :
		m_pfnNode( NULL ),
		m_ui32Attribute( 0UL ) {
	}
	LSE_CALLCTOR CAnimationTrack::~CAnimationTrack() {
	}

	// == Functions.
	/**
	 * Loads data from an FBX animation curve.
	 *
	 * \param _pfacCurve The curve from which to load keyframes.
	 * \param _pfnNode The node effected by this track.
	 * \param _ui32Attribute The attribute of that node affected by this track.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CAnimationTrack::Load( FbxAnimCurve * _pfacCurve, FbxNode * _pfnNode, LSUINT32 _ui32Attribute ) {
		SetAttributes( _pfnNode, _ui32Attribute );

		LSUINT32 ui32Total = _pfacCurve->KeyGetCount();
		static const FbxTime::EMode emModes[] = {
			FbxTime::eFrames96,
			FbxTime::eFrames60,
			FbxTime::eFrames48,
			FbxTime::eFrames30,
			FbxTime::eFrames24,
		};
		LSUINT32 ui32FrameMode = 0UL;
		if ( ui32Total ) {
			while ( ui32FrameMode < LSE_ELEMENTS( emModes ) ) {
				// Count how many entries we will add.
				FbxTime ftTotalTime = _pfacCurve->KeyGetTime( ui32Total - 1UL ) - _pfacCurve->KeyGetTime( 0UL );
				// We sample at 48 frames per second.
				FbxLongLong fllFrames = ftTotalTime.GetFrameCount( emModes[ui32FrameMode] );
				if ( ui32Total + fllFrames >= 0x0000000100000000ULL ) {
					// Too many frames!  Holy crazy!  Try to sample at the next-lower resolution.
					++ui32FrameMode;
					continue;
				}

				m_sKeyFrames.AllocateAtLeast( static_cast<LSUINT32>(ui32Total + fllFrames) );
				// Evaluate at the actual key times.
				int iIndex = 0;
				for ( LSUINT32 I = 0UL; I < ui32Total; ++I ) {
					if ( !SetKeyFrame( _pfacCurve->KeyGetTime( I ), _pfacCurve->Evaluate( _pfacCurve->KeyGetTime( I ), &iIndex ) ) ) {
						return false;
					}
				}
				// Extra evaluation between key times.
				iIndex = 0;
				FbxTime ftFrameTime;
				for ( FbxLongLong I = 0ULL; I < fllFrames; ++I ) {
					ftFrameTime.SetFrame( I, emModes[ui32FrameMode] );
					FbxTime ftCurTime = _pfacCurve->KeyGetTime( 0UL ) + ftFrameTime;
					if ( !SetKeyFrame( ftCurTime, _pfacCurve->Evaluate( ftCurTime, &iIndex ) ) ) {
						return false;
					}
				}


				// Now simplify.
				LSUINT32 ui32Eliminated = 0UL;
				for ( LSUINT32 ui32Start = 0UL; m_sKeyFrames.Length() >= 3UL && ui32Start < m_sKeyFrames.Length() - 2UL; ++ui32Start ) {
					const LSUINT32 ui32End = ui32Start + 2UL;
					while ( m_sKeyFrames.Length() >= 3UL && ui32Start < m_sKeyFrames.Length() - 2UL ) {
						// Try to remove the key between ui32Start and ui32End.
						LSDOUBLE dSpan = static_cast<LSDOUBLE>(m_sKeyFrames.GetByIndex( ui32End ).tTime.GetMilliSeconds()) - static_cast<LSDOUBLE>(m_sKeyFrames.GetByIndex( ui32Start ).tTime.GetMilliSeconds());
						LSDOUBLE dFrac = (static_cast<LSDOUBLE>(m_sKeyFrames.GetByIndex( ui32Start + 1UL ).tTime.GetMilliSeconds()) -
							static_cast<LSDOUBLE>(m_sKeyFrames.GetByIndex( ui32Start ).tTime.GetMilliSeconds())) / dSpan;
						// Interpolate by this much between the start and end keys.
						LSDOUBLE dInterp = (m_sKeyFrames.GetByIndex( ui32End ).fValue - m_sKeyFrames.GetByIndex( ui32Start ).fValue) * dFrac + m_sKeyFrames.GetByIndex( ui32Start ).fValue;
						LSDOUBLE dActual = m_sKeyFrames.GetByIndex( ui32Start + 1UL ).fValue;
						LSDOUBLE dDif = ::abs( dInterp - dActual );
						if ( dDif < 0.05 ) {
							m_sKeyFrames.RemoveByIndex( ui32Start + 1UL );
							++ui32Eliminated;
						}
						else {
							// Move on to the next key frame and repeat.
							break;
						}
					}
				}
				::printf( "\tKey frames: %u -> %u (%f).\r\n", ui32Total,
					m_sKeyFrames.Length(), m_sKeyFrames.Length() * 100.0f / static_cast<LSFLOAT>(ui32Total) );
				break;
			}
		}
		return ui32FrameMode < LSE_ELEMENTS( emModes );
	}

	/**
	 * Save the data to a buffer.
	 *
	 * \param _bBuffer The buffer to which to save our data.
	 * \return Returns true only if all data is written to the buffer.
	 */
	LSBOOL LSE_CALL CAnimationTrack::SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		// The target node's name.
		const char * pcName = m_pfnNode->GetName();
		if ( !CMisc::WriteString( pcName, _bBuffer ) ) { return false; }
		// The attribute.
		if ( !CMisc::WriteUInt( m_ui32Attribute, _bBuffer ) ) { return false; }
		// Write the number of key frames.
		if ( !CMisc::WriteUInt( m_sKeyFrames.Length(), _bBuffer ) ) { return false; }
		for ( LSUINT32 I = 0UL; I < m_sKeyFrames.Length(); ++I ) {
			if ( !CMisc::WriteUInt64( m_sKeyFrames.GetByIndex( I ).tTime.GetMilliSeconds(), _bBuffer ) ) { return false; }
			if ( !CMisc::WriteFloat( m_sKeyFrames.GetByIndex( I ).fValue, _bBuffer ) ) { return false; }
		}
		return true;
	}

}	// namespace lsx

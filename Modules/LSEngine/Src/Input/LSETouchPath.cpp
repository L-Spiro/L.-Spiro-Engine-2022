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
 * Description: A single touch path on a touch-enabled device.
 */

#include "LSETouchPath.h"


namespace lse {

	// == Members.
	/** ID counter. */
	CAtomic::ATOM CTouchPath::m_aIdCounter = 0;

	// == Various constructors.
	LSE_CALLCTOR CTouchPath::CTouchPath() :
		m_ui16Total( 0 ),
		m_ui16DeadFrames( 0 ),
		m_bJustBegan( false ),
		m_aId( CAtomic::InterlockedIncrement( m_aIdCounter ) ) {
	}
	LSE_CALLCTOR CTouchPath::~CTouchPath() {
	}
	
	// == Functions.
	/**
	 * Adds a point to the path.  May cause a past point to be removed if
	 *	the buffer is full.
	 *
	 * \param _vPos The position to add to the path.
	 * \param _ui64Time Timestamp of the point in the path.
	 */
	void LSE_CALL CTouchPath::AddPoint( const CVector2 &_vPos, uint64_t _ui64Time ) {
		if ( m_ui16Total < LSE_TPM_HISTORY_LENGTH ) {
			m_bJustBegan = m_ui16Total == 0;
			m_pPath[m_ui16Total].vPos = _vPos;
			m_pPath[m_ui16Total++].ui64Time = _ui64Time;
		}
		else {
			for ( uint32_t I = 1; I < LSE_TPM_HISTORY_LENGTH; ++I ) {
				m_pPath[I-1] = m_pPath[I];
			}
			m_pPath[LSE_TPM_HISTORY_LENGTH-1].vPos = _vPos;
			m_pPath[LSE_TPM_HISTORY_LENGTH-1].ui64Time = _ui64Time;
		}
	}
	
	/**
	 * Kills the path.  The dead counter begins counting every frame
	 *	after this.
	 */
	void LSE_CALL CTouchPath::Kill() {
		m_ui16DeadFrames++;
	}

}	// namespace lse

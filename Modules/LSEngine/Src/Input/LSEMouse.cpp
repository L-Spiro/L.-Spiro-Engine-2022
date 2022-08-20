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
 * Description: Keep track of mouse movements.
 */

#include "LSEMouse.h"


namespace lse {

	// == Functions.
	/**
	 * Update the position.  This only updates m_vTempPos.  The changes are applied only after a
	 *	call to ApplyChanges().
	 *
	 * \param _fX New X position.
	 * \param _fY New Y position.
	 * \param _ui64Time Time of the event.
	 */
	void CMouse::SetPos( LSREAL _fX, LSREAL _fY, uint64_t _ui64Time ) {
		m_vLastPos = m_vPos;
		m_vPos.x = _fX;
		m_vPos.y = _fY;
		AddToHistory( m_vPos, _ui64Time );
	}

	/**
	 * Update the position.  This only updates m_vTempPos.  The changes are applied only after a
	 *	call to ApplyChanges().
	 *
	 * \param _fX Amount to add to the X.
	 * \param _fY Amount to add to the Y.
	 * \param _ui64Time Time of the event.
	 */
	void CMouse::AddPos( LSREAL _fX, LSREAL _fY, uint64_t _ui64Time ) {
		m_vLastPos = m_vPos;
		m_vPos.x += _fX;
		m_vPos.y += _fY;
		AddToHistory( m_vPos, _ui64Time );
	}

	/**
	 * Applies changes made to the position/state of the mouse.  Supply the current time in
	 *	microseconds.  The time should be absolute, not relative.
	 *
	 * \param _ui64TimeStamp Current microseconds.
	 */
	void CMouse::ApplyChanges( uint64_t _ui64TimeStamp ) {
		m_ui64TimeNow = _ui64TimeStamp;

		for ( uint32_t I = LSE_MOUSE_BUTTONS; I--; ) {
			m_mbLastFrame[I] = m_mbCurFrame[I];
		}

		m_vLastInterpPos = m_vInterpPos;
		m_vInterpPos = GetInterpolatedPos();
	}

	/**
	 * Sets the starting position of the mouse.  Updates m_vPos, m_vTempPos, and m_vLastPos
	 *	to that position.
	 *
	 * \param _fX X position.
	 * \param _fY Y position.
	 */
	void CMouse::SetStartPos( LSREAL _fX, LSREAL _fY ) {
		m_vPos.x = _fX;
		m_vPos.y = _fY;
		m_vLastPos = m_vPos;
	}

	/**
	 * Snap the position to be within a box.
	 *
	 * \param _fX X extents.
	 * \param _fY Y extents.
	 * \param _fRight Right extents.
	 * \param _fBottom Bottom extents.
	 */
	void LSE_CALL CMouse::Snap( LSREAL _fX, LSREAL _fY, LSREAL _fRight, LSREAL _fBottom ) {
		m_vPos.x = CMathLib::Min( m_vPos.x, _fRight );
		m_vPos.x = CMathLib::Max( m_vPos.x, _fX );
		m_vPos.y = CMathLib::Min( m_vPos.y, _fBottom );
		m_vPos.y = CMathLib::Max( m_vPos.y, _fY );
	}

	/**
	 * Click any mouse button.
	 *
	 * \param _ui32Index Index of the button to click.
	 */
	void LSE_CALL CMouse::Click( uint32_t _ui32Index, uint64_t _ui64Time ) {
		assert( _ui32Index < LSE_MOUSE_BUTTONS );
		m_mbCurFrame[_ui32Index].bPressed = true;
		m_mbCurFrame[_ui32Index].ui64Time = _ui64Time;
		if ( m_mbiButtonInfo.ui32StateTotal[_ui32Index] < LSE_MOUSE_MAX_EVENTS_PER_FRAME ) {
			m_mbiButtonInfo.mbStates[_ui32Index][m_mbiButtonInfo.ui32StateTotal[_ui32Index]++] = m_mbCurFrame[_ui32Index];
		}
	}

	/**
	 * Unclick any mouse button.
	 *
	 * \param _ui32Index Index of the button to release.
	 */
	void LSE_CALL CMouse::UnClick( uint32_t _ui32Index, uint64_t _ui64Time ) {
		assert( _ui32Index < LSE_MOUSE_BUTTONS );
		m_mbCurFrame[_ui32Index].bPressed = false;
		m_mbCurFrame[_ui32Index].ui64Time = _ui64Time;
		if ( m_mbiButtonInfo.ui32StateTotal[_ui32Index] < LSE_MOUSE_MAX_EVENTS_PER_FRAME ) {
			m_mbiButtonInfo.mbStates[_ui32Index][m_mbiButtonInfo.ui32StateTotal[_ui32Index]++] = m_mbCurFrame[_ui32Index];
		}
	}

	/**
	 * Adds a mouse drag to the history.
	 *
	 * \param _vPos Position of the move.
	 * \param _ui64Time Time of the move.
	 */
	void LSE_CALL CMouse::AddToHistory( const CVector2 &_vPos, uint64_t _ui64Time ) {
		// Push the items back on the history stack.
		if ( m_ui32HistorySize < LSE_MOUSE_SMOOTH_HISTORY ) {
			m_mmpHistory[m_ui32HistorySize].ui64Time = _ui64Time;
			m_mmpHistory[m_ui32HistorySize].vPos = _vPos;
			++m_ui32HistorySize;
		}
		else {
			CStd::MemMove( &m_mmpHistory[0], &m_mmpHistory[1], sizeof( m_mmpHistory[0] ) * (LSE_MOUSE_SMOOTH_HISTORY - 1) );
			m_mmpHistory[LSE_MOUSE_SMOOTH_HISTORY-1].ui64Time = _ui64Time;
			m_mmpHistory[LSE_MOUSE_SMOOTH_HISTORY-1].vPos = _vPos;
		}
	}

}	// namespace lse

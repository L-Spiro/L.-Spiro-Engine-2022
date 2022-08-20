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
 * Description: Keep track of touch movements.
 */

#include "LSETouch.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CTouch::CTouch() :
		m_ui32Total( 0 ) {
	}
	LSE_CALLCTOR CTouch::~CTouch() {
	}
	
	// == Functions.
	/**
	 * Advances the touch input one logical tick.  Causes dead
	 *	paths to be removed.
	 */
	void LSE_CALL CTouch::Tick() {
		for ( uint32_t I = m_ui32Total; I--; ) {
			m_tpPaths[I].Tick();
			if ( m_tpPaths[I].DeadCount() > LSE_TM_DEAD_FRAMES ) {
				RemPath( I );
			}
		}
	}
	
	/**
	 * Adds a touch-down event.  If no paths are available, false is returned.
	 *
	 * \param _vPos The position of the path to start.
	 * \param _ui64Time Time of the path to start.
	 * \return Returns true if the path was added.
	 */
	LSBOOL LSE_CALL CTouch::TouchDown( const CVector2 &_vPos, uint64_t _ui64Time ) {
		uint32_t ui32Index = AddPath();
		if ( ui32Index == LSE_TM_MAX_TOUCHES ) { return false; }
		m_tpPaths[ui32Index].AddPoint( _vPos, _ui64Time );
		return true;
	}
	
	/**
	 * Adds a touch-move event to a path whose end point matches _vStart.
	 *
	 * \param _vStart The position of the path to which _vEnd will be added.
	 * \param _vEnd Position to add to an existing path whose end point matches _vStart.
	 * \param _ui64Time Time of the event to add to a path whose end point
	 *	is the same as _vStart.
	 * \return Returns true if the path was updated.
	 */
	LSBOOL LSE_CALL CTouch::TouchMove( const CVector2 &_vStart, const CVector2 &_vEnd, uint64_t _ui64Time ) {
		const CTouchPath * ptpPath = GetPath( _vStart );
		if ( !ptpPath ) {
			CStd::DebugPrintA( "TouchMove() failed.\r\n" );
			return false;
		}
		CTouchPath * ptpThisPath = const_cast<CTouchPath *>(ptpPath);
		ptpThisPath->AddPoint( _vEnd, _ui64Time );
		return true;
	}
	
	/**
	 * Adds a touch-up event.  If no paths at the given position exist, false is returned.
	 *
	 * \param _vStart The position of the path to end.
	 * \param _vEnd The position of the touch-end.
	 * \param _ui64Time Time of the path to end.
	 * \return Returns true if the path was ended.
	 */
	LSBOOL LSE_CALL CTouch::TouchEnd( const CVector2 &_vStart, const CVector2 &_vEnd, uint64_t _ui64Time ) {
		const CTouchPath * ptpPath = GetPath( _vStart );
		CVector2 vStart = _vStart;
		CVector2 vEnd = _vEnd;

		if ( !ptpPath ) {
			ptpPath = GetPath( _vEnd );
			if ( !ptpPath ) {
				CStd::DebugPrintA( "TouchEnd() failed.\r\n" );
				return false;
			}
			CStd::Swap( vStart, vEnd );
		}
		CTouchPath * ptpThisPath = const_cast<CTouchPath *>(ptpPath);
		if ( ptpThisPath->LastPos() != vEnd ) {
			ptpThisPath->AddPoint( vEnd, _ui64Time );
		}
		ptpThisPath->Kill();
		CStd::DebugPrintA( "TouchEnd() win.\r\n" );
		return true;
	}
	
	/**
	 * Adds a touch-cancel event.  If no paths at the given position exist, false is returned.
	 *
	 * \param _vStart The position of the path to cancel.
	 * \param _vEnd The position of the touch-cancel.
	 * \return Returns true if the path was cancelled.
	 */
	LSBOOL LSE_CALL CTouch::TouchCancel( const CVector2 &_vStart, const CVector2 &_vEnd, uint64_t /*_ui64Time*/ ) {
		LSBOOL bFound = false;
		for ( uint32_t I = m_ui32Total; I--; ) {
			if ( m_tpPaths[I].LastPos() == _vStart || m_tpPaths[I].LastPos() == _vEnd ) {
				RemPath( I );
				bFound = true;
			}
		}
		return bFound;
	}
	
	/**
	 * Gets a pointer to a path that ends with the given position or NULL if none exists.
	 *
	 * \param _vPos Position of the path to find.
	 * \return Returns the path that ends with _vPos or NULL if none exists.
	 */
	const CTouchPath * LSE_CALL CTouch::GetPath( const CVector2 &_vPos ) const {
		for ( uint32_t I = m_ui32Total; I--; ) {
			if ( m_tpPaths[I].LastPos() == _vPos ) { return &m_tpPaths[I]; }
		}
		return NULL;
	}
	
	/**
	 * Gets a pointer to a path that just began or NULL if none exists.  If multiple
	 *	paths have just began, only the last one is returned.
	 *
	 * \return Returns the last path to have just begun or NULL if no paths just began.
	 */
	const CTouchPath * LSE_CALL CTouch::PathThatJustBegun() const {
		for ( uint32_t I = m_ui32Total; I--; ) {
			if ( m_tpPaths[I].JustBegan() ) { return &m_tpPaths[I]; }
		}
		return NULL;
	}
	
	/**
	 * Gets a pointer to a path given its ID.  Returns NULL if no such path exists.
	 *
	 * \param _ui32Id ID of the path to find.
	 * \return Returns the path with the given ID or NULL if there is no such path.
	 */
	const CTouchPath * LSE_CALL CTouch::PathById( uint32_t _ui32Id ) const {
		for ( uint32_t I = m_ui32Total; I--; ) {
			if ( m_tpPaths[I].Id() == _ui32Id ) { return &m_tpPaths[I]; }
		}
		return NULL;
	}
	
	/**
	 * Adds a path and returns the resulting index of the path, which if
	 *	is equal to LSE_TM_MAX_TOUCHES implies that no new path was added.
	 *
	 * \return Returns the index of the added path.
	 */
	uint32_t LSE_CALL CTouch::AddPath() {
		if ( m_ui32Total == LSE_TM_MAX_TOUCHES ) { return LSE_TM_MAX_TOUCHES; }
		m_tpPaths[m_ui32Total].Reset();
		return m_ui32Total++;
	}
	
	/**
	 * Removes a path at the given index.
	 *
	 * \param _ui32Index Index of the path to remove.
	 */
	void LSE_CALL CTouch::RemPath( uint32_t _ui32Index ) {
		if ( _ui32Index < m_ui32Total ) {
			--m_ui32Total;
			for ( uint32_t I = _ui32Index; I < m_ui32Total; ++I ) {
				CStd::MemCpy( &m_tpPaths[I], &m_tpPaths[I+1], sizeof( m_tpPaths[I] ) );
			}
		}
	}

}	// namespace lse

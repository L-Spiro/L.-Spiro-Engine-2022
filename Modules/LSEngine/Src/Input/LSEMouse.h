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


#ifndef __LSE_MOUSE_H__
#define __LSE_MOUSE_H__

#include "../LSEEngine.h"
#include "Vector/LSMVector2.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** Number of buttons on a mouse.  Change it to support as many as you want (but you will
 *	also have to make a way to send clicks to the extra slots). */
#define LSE_MOUSE_BUTTONS				5

/** Number of history items in the mouse-smoothening array. */
#define LSE_MOUSE_SMOOTH_HISTORY		50

/** Maximum times in a logical update that a mouse button can be clicked and unclicked. */
#define LSE_MOUSE_MAX_EVENTS_PER_FRAME	5

namespace lse {

	/**
	 * Class CMouse
	 * \brief Track mouse movements.
	 *
	 * Description: Keep track of mouse movements.
	 */
	class CMouse {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALL				CMouse();


		// == Functions.
		/**
		 * Gets the position of the cursor.
		 *
		 * \return Returns the position of the cursor.
		 */
		const CVector2 & LSE_CALL		GetPos() const;

		/**
		 * Gets the last position of the cursor.
		 *
		 * \return Returns the last position of the cursor.
		 */
		const CVector2 & LSE_CALL		GetLastPos() const;

		/**
		 * Gets the delta since the last position.
		 *
		 * \return Returns the delta since the last position.
		 */
		CVector2 LSE_CALL				GetDelta() const;

		/**
		 * Gets only the X delta.
		 *
		 * \return Returns only the X delta.
		 */
		LSREAL LSE_CALL					GetXDelta() const;

		/**
		 * Gets only the Y delta.
		 *
		 * \return Returns only the Y delta.
		 */
		LSREAL LSE_CALL					GetYDelta() const;

		/**
		 * Gets the interpolated (smoothened) position of the cursor.
		 *
		 * \return Returns the interpolated position of the cursor.
		 */
		const CVector2 & LSE_CALL		GetInterpPos() const;

		/**
		 * Gets the last interpolated (smoothened) position of the cursor.
		 *
		 * \return Returns the last interpolated position of the cursor.
		 */
		const CVector2 & LSE_CALL		GetInterpLastPos() const;

		/**
		 * Gets the delta since the last interpolated (smoothened) position.
		 *
		 * \return Returns the delta since the last interpolated position.
		 */
		CVector2 LSE_CALL				GetInterpDelta() const;

		/**
		 * Gets only the X delta of the interpolated (smoothened) position.
		 *
		 * \return Returns only the X delta of the interpolated position.
		 */
		LSREAL LSE_CALL					GetInterpXDelta() const;

		/**
		 * Gets only the Y delta of the interpolated (smoothened) position.
		 *
		 * \return Returns only the Y delta of the interpolated position.
		 */
		LSREAL LSE_CALL					GetInterpYDelta() const;


		/**
		 * Update the position.  This only updates m_vTempPos.  The changes are applied only after a
		 *	call to ApplyChanges().
		 *
		 * \param _fX New X position.
		 * \param _fY New Y position.
		 * \param _ui64Time Time of the event.
		 */
		void LSE_CALL					SetPos( LSREAL _fX, LSREAL _fY, uint64_t _ui64Time );

		/**
		 * Update the position.  This only updates m_vTempPos.  The changes are applied only after a
		 *	call to ApplyChanges().
		 *
		 * \param _fX Amount to add to the X.
		 * \param _fY Amount to add to the Y.
		 * \param _ui64Time Time of the event.
		 */
		void LSE_CALL					AddPos( LSREAL _fX, LSREAL _fY, uint64_t _ui64Time );

		/**
		 * Applies changes made to the position/state of the mouse.  Supply the current time in
		 *	microseconds.  The time should be absolute, not relative.
		 *
		 * \param _ui64TimeStamp Current microseconds.
		 */
		void LSE_CALL					ApplyChanges( uint64_t _ui64TimeStamp );

		/**
		 * Sets the starting position of the mouse.  Updates m_vPos, m_vTempPos, and m_vLastPos
		 *	to that position.
		 *
		 * \param _fX X position.
		 * \param _fY Y position.
		 */
		void LSE_CALL					SetStartPos( LSREAL _fX, LSREAL _fY );

		/**
		 * Snap the position to be within a box.
		 *
		 * \param _fX X extents.
		 * \param _fY Y extents.
		 * \param _fRight Right extents.
		 * \param _fBottom Bottom extents.
		 */
		void LSE_CALL					Snap( LSREAL _fX, LSREAL _fY, LSREAL _fRight, LSREAL _fBottom );

		/**
		 * Click any mouse button.
		 *
		 * \param _ui32Index Index of the button to click.
		 * \param _ui64Time Time of the event.
		 */
		void LSE_CALL					Click( uint32_t _ui32Index, uint64_t _ui64Time );

		/**
		 * Unclick any mouse button.
		 *
		 * \param _ui32Index Index of the button to release.
		 * \param _ui64Time Time of the event.
		 */
		void LSE_CALL					UnClick( uint32_t _ui32Index, uint64_t _ui64Time );

		/**
		 * Is a mouse button down?
		 *
		 * \param _ui32Index Index of the mouse button to test for being down.
		 * \return Returns true if the button at the given index is down.
		 */
		LSBOOL LSE_CALL					IsMouseButtonDown( uint32_t _ui32Index ) const;

		/**
		 * Was a mouse button just pressed?
		 *
		 * \param _ui32Index Index of the mouse button to test for being pressed.
		 * \return Returns true if the button at the given index was just pressed.
		 */
		LSBOOL LSE_CALL					MouseButtonJustPressed( uint32_t _ui32Index ) const;

		/**
		 * Was a mouse button just released?
		 *
		 * \param _ui32Index Index of the mouse button to test for being released.
		 * \return Returns true if the button at the given index was just released.
		 */
		LSBOOL LSE_CALL					MouseButtonJustReleased( uint32_t _ui32Index ) const;

		/**
		 * Reset mouse clicks.  Done when the game loses focus.  Prevents buttons from being
		 *	held indefinitely.
		 */
		void LSE_CALL					ResetClicks();

		/**
		 * Resets all mouse information.
		 */
		void LSE_CALL					Reset();

		/**
		 * Gets the index map.  Used when applying a click or unclick using index mapping.
		 *
		 * \return Returns the index map.
		 */
		//const int16_t * LSE_CALL		GetIndexMap() const;


	protected :
		// == Types.
		/**
		 * For mouse smoothening, we need a small history of mouse
		 *	positions along with timestamps.  This structure brings
		 *	them together.
		 */
		struct LSE_MOUSE_MOVE_POS {
			/** Position at the given time. */
			CVector2					vPos;

			/** Time of this position. */
			uint64_t					ui64Time;
		};

		/**
		 * Information for a single button for a frame.
		 */
		typedef struct LSE_MOUSE_BUTTON {
			/** True = pressed, false = not pressed. */
			LSBOOL						bPressed;

			/** Time-stamp of the press/release. */
			uint64_t					ui64Time;
		} * LPLSE_MOUSE_BUTTON, * const LPCLSE_MOUSE_BUTTON;

		/**
		 * Information for all buttons for a single frame.
		 */
		typedef struct LSE_MOUSE_BUTTON_INFO {
			/** Button states. */
			LSE_MOUSE_BUTTON			mbStates[LSE_MOUSE_BUTTONS][LSE_MOUSE_MAX_EVENTS_PER_FRAME];

			/** Total states for each button. */
			uint16_t					ui32StateTotal[LSE_MOUSE_BUTTONS];
		} * LPLSE_MOUSE_BUTTON_INFO, * const LPCLSE_MOUSE_BUTTON_INFO;


		// == Members.
		/** Position of the cursor. */
		CVector2						m_vPos;

		/** Last position of the cursor. */
		CVector2						m_vLastPos;

		/** Current interpolated position. */
		CVector2						m_vInterpPos;

		/** Last interpolated position. */
		CVector2						m_vLastInterpPos;

		/** History of positions, used for mouse smoothening. */
		LSE_MOUSE_MOVE_POS				m_mmpHistory[LSE_MOUSE_SMOOTH_HISTORY];

		/** Size of the history buffer. */
		uint32_t						m_ui32HistorySize;

		/** How far behind does our mouse lag?  The mouse lags behind a bit so it
		 *	can interpolate between the previous 2 positions.  The lag time should
		 *	be exactly (or as close as possible to) the time between mouse updates.
		 *	That is, if Windows (for example) is updating our mouse position every
		 *	30 milliseconds, our lag time should be 30,000 microseconds, so that as
		 *	soon as it interpolates to the end of the list of moves, a new move is
		 *	added by Windows and interpolation will continue smoothly to it (over
		 *	the course of 30 milliseconds).  If the time is set too low (for example
		 *	0 microseconds) then the interpolation will be reach the end of the list
		 *	of movements before the next movement is added to the list, causing some
		 *	jerkiness.  If the time is set too high, the level of smoothness between
		 *	positions will not increase (no benefit), but there will be more noticable
		 *	lag between the player motion and the in-game response.  Defaults to
		 *	8000 microseconds. */
		uint32_t						m_ui32InterpTime;

		/** The actual time now.  Needed for interpolation. */
		uint64_t						m_ui64TimeNow;

#if 0
		/** Input mapping.  The source index is the index into this array.
		 *	the destination index is the value at that index.  Defaults
		 *	to a one-to-one mapping where the source and destination
		 *	values are the same. */
		int16_t							m_i16IndexMap[LSE_MOUSE_BUTTONS];
#endif	// #if 0
		
		/** Button information for the current frame. */
		LSE_MOUSE_BUTTON_INFO			m_mbiButtonInfo;

		/** Current frame's button statuses. */
		LSE_MOUSE_BUTTON				m_mbCurFrame[LSE_MOUSE_BUTTONS];

		/** Last frame's button statuses. */
		LSE_MOUSE_BUTTON				m_mbLastFrame[LSE_MOUSE_BUTTONS];


		// == Functions.
		/**
		 * Gets the most recent time in the history stack.
		 *
		 * \return Returns the most recent time in the history stack.
		 */
		uint64_t LSE_CALL				GetMostRecentHistoryTime() const;

		/**
		 * Gets the index of the first history item whose timestamp is over or equal to
		 *	the given timestamp.  Returns LSE_MOUSE_SMOOTH_HISTORY on failure.
		 *
		 * \param _ui64Time Time of the index of the history item to find.
		 * \return Returns the index of the first history item whose timestamp is over or equal to
		 *	the given timestamp
		 */
		uint32_t LSE_CALL				FindTimeStampByTime( uint64_t _ui64Time ) const;

		/**
		 * Gets the interpolated mouse position.  Uses m_ui32InterpTime and the history
		 *	stack.  If there is none, or the time is beyond the stack, returns m_vPos.
		 *
		 * \return Returns the interpolated mouse position.
		 */
		CVector2 LSE_CALL				GetInterpolatedPos() const;

		/**
		 * Adds a mouse drag to the history.
		 *
		 * \param _vPos Position of the move.
		 * \param _ui64Time Time of the move.
		 */
		void LSE_CALL					AddToHistory( const CVector2 &_vPos, uint64_t _ui64Time );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALL CMouse::CMouse() :
		m_ui32HistorySize( 0 ),
		m_ui32InterpTime( 8000 ),
		m_ui64TimeNow( 0ULL ) {
		/*for ( uint32_t I = 0; I < LSE_ELEMENTS( m_i32ClickCount ); ++I ) {
			m_i32ClickCountBuffer[I] = m_i32ClickCount[I] = m_i32UnClickCount[I] = m_i32LastClickState[I] = 0;
			m_i16IndexMap[I] = static_cast<int16_t>(I);
		}*/
	}

	// == Functions.
	/**
	 * Gets the position of the cursor.
	 *
	 * \return Returns the position of the cursor.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CMouse::GetPos() const {
		return m_vPos;
	}

	/**
	 * Gets the last position of the cursor.
	 *
	 * \return Returns the last position of the cursor.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CMouse::GetLastPos() const {
		return m_vLastPos;
	}

	/**
	 * Gets the delta since the last position.
	 *
	 * \return Returns the delta since the last position.
	 */
	LSE_INLINE CVector2 LSE_CALL CMouse::GetDelta() const {
		return CVector2( m_vPos.x - m_vLastPos.x, m_vPos.y - m_vLastPos.y );
	}

	/**
	 * Gets only the X delta.
	 *
	 * \return Returns only the X delta.
	 */
	LSE_INLINE LSREAL LSE_CALL CMouse::GetXDelta() const {
		return m_vPos.x - m_vLastPos.x;
	}

	/**
	 * Gets only the Y delta.
	 *
	 * \return Returns only the Y delta.
	 */
	LSE_INLINE LSREAL LSE_CALL CMouse::GetYDelta() const {
		return m_vPos.y - m_vLastPos.y;
	}

	/**
	 * Gets the interpolated (smoothened) position of the cursor.
	 *
	 * \return Returns the interpolated position of the cursor.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CMouse::GetInterpPos() const {
		return m_vInterpPos;
	}

	/**
	 * Gets the last interpolated (smoothened) position of the cursor.
	 *
	 * \return Returns the last interpolated position of the cursor.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CMouse::GetInterpLastPos() const {
		return m_vLastInterpPos;
	}

	/**
	 * Gets the delta since the last interpolated (smoothened) position.
	 *
	 * \return Returns the delta since the last interpolated position.
	 */
	LSE_INLINE CVector2 LSE_CALL CMouse::GetInterpDelta() const {
		return m_vInterpPos - m_vLastInterpPos;
	}

	/**
	 * Gets only the X delta of the interpolated (smoothened) position.
	 *
	 * \return Returns only the X delta of the interpolated position.
	 */
	LSE_INLINE LSREAL LSE_CALL CMouse::GetInterpXDelta() const {
		return m_vInterpPos.x - m_vLastInterpPos.x;
	}

	/**
	 * Gets only the Y delta of the interpolated (smoothened) position.
	 *
	 * \return Returns only the Y delta of the interpolated position.
	 */
	LSE_INLINE LSREAL LSE_CALL CMouse::GetInterpYDelta() const {
		return m_vInterpPos.y - m_vLastInterpPos.y;
	}

	/**
	 * Is a mouse button down?
	 *
	 * \param _ui32Index Index of the mouse button to test for being down.
	 * \return Returns true if the button at the given index is down.
	 */
	LSE_INLINE LSBOOL LSE_CALL CMouse::IsMouseButtonDown( uint32_t _ui32Index ) const {
		assert( _ui32Index < LSE_MOUSE_BUTTONS );
		return m_mbCurFrame[_ui32Index].bPressed;
	}

	/**
	 * Was a mouse button just pressed?
	 *
	 * \param _ui32Index Index of the mouse button to test for being pressed.
	 * \return Returns true if the button at the given index was just pressed.
	 */
	LSE_INLINE LSBOOL LSE_CALL CMouse::MouseButtonJustPressed( uint32_t _ui32Index ) const {
		assert( _ui32Index < LSE_MOUSE_BUTTONS );
		return m_mbCurFrame[_ui32Index].bPressed && !m_mbLastFrame[_ui32Index].bPressed;
	}

	/**
	 * Was a mouse button just released?
	 *
	 * \param _ui32Index Index of the mouse button to test for being released.
	 * \return Returns true if the button at the given index was just released.
	 */
	LSE_INLINE LSBOOL LSE_CALL CMouse::MouseButtonJustReleased( uint32_t _ui32Index ) const {
		assert( _ui32Index < LSE_MOUSE_BUTTONS );
		return !m_mbCurFrame[_ui32Index].bPressed && m_mbLastFrame[_ui32Index].bPressed;
	}

	/**
	 * Reset mouse clicks.  Done when the game loses focus.  Prevents buttons from being
	 *	held indefinitely.
	 */
	LSE_INLINE void LSE_CALL CMouse::ResetClicks() {
		CStd::MemSet( &m_mbiButtonInfo, 0, sizeof( m_mbiButtonInfo ) );
		CStd::MemSet( m_mbCurFrame, 0, sizeof( m_mbCurFrame ) );
		CStd::MemSet( m_mbLastFrame, 0, sizeof( m_mbLastFrame ) );
	}

	/**
	 * Resets all mouse information.
	 */
	LSE_INLINE void LSE_CALL CMouse::Reset() {
		ResetClicks();
		m_ui32HistorySize = 0;
		m_vPos = m_vLastPos = m_vInterpPos = m_vLastInterpPos = CVector2( LSM_ZERO, LSM_ZERO );
	}

	/**
	 * Gets the index map.  Used when applying a click or unclick using index mapping.
	 *
	 * \return Returns the index map.
	 */
	//LSE_INLINE const int16_t * LSE_CALL CMouse::GetIndexMap() const { return m_i16IndexMap; }

	/**
	 * Gets the most recent time in the history stack.
	 *
	 * \return Returns the most recent time in the history stack.
	 */
	LSE_INLINE uint64_t LSE_CALL CMouse::GetMostRecentHistoryTime() const {
		if ( m_ui32HistorySize == 0 ) {
			// No history.
			return 0ULL;
		}
		return m_mmpHistory[m_ui32HistorySize-1].ui64Time;
	}

	/**
	 * Gets the index of the first history item whose timestamp is over or equal to
	 *	the given timestamp.  Returns LSE_MOUSE_SMOOTH_HISTORY on failure.
	 *
	 * \param _ui64Time Time of the index of the history item to find.
	 * \return Returns the index of the first history item whose timestamp is over or equal to
	 *	the given timestamp
	 */
	LSE_INLINE uint32_t LSE_CALL CMouse::FindTimeStampByTime( uint64_t _ui64Time ) const {
		for ( uint32_t I = 0; I < m_ui32HistorySize; ++I ) {
			if ( m_mmpHistory[I].ui64Time >= _ui64Time ) {
				return I;
			}
		}
		return LSE_MOUSE_SMOOTH_HISTORY;
	}

	/**
	 * Gets the interpolated mouse position.  Uses m_ui32InterpTime and the history
	 *	stack.  If there is none, or the time is beyond the stack, returns m_vPos.
	 *
	 * \return Returns the interpolated mouse position.
	 */
	LSE_INLINE CVector2 LSE_CALL CMouse::GetInterpolatedPos() const {
		if ( m_ui32HistorySize == 0 ) {
			// No history.
			return m_vPos;
		}

		uint32_t ui32Index = FindTimeStampByTime( m_ui64TimeNow - m_ui32InterpTime );
		// If none found, we are at the end of the history.
		if ( ui32Index == LSE_MOUSE_SMOOTH_HISTORY ) {
			return m_vPos;
		}
		if ( ui32Index == 0 ) {
			// No history item prior to this for interpolating between.
			return m_mmpHistory[ui32Index].vPos;
		}

		// Now we have positions on both sides of our delayed timestamp, so
		//	we can interpolate between them.
		uint32_t ui32DistanceBetweenPos = static_cast<uint32_t>(m_mmpHistory[ui32Index].ui64Time - m_mmpHistory[ui32Index-1].ui64Time);
		uint32_t ui32DistanceBetweenCurTime = static_cast<uint32_t>((m_ui64TimeNow - m_ui32InterpTime) - m_mmpHistory[ui32Index-1].ui64Time);
		LSREAL fFrac = static_cast<LSREAL>(ui32DistanceBetweenCurTime >> 2) / static_cast<LSREAL>(ui32DistanceBetweenPos >> 2);

		CVector2 vInterp = m_mmpHistory[ui32Index].vPos - m_mmpHistory[ui32Index-1].vPos;
		return m_mmpHistory[ui32Index-1].vPos + (vInterp * fFrac);
	}

}	// namespace lse

#endif	// __LSE_MOUSE_H__

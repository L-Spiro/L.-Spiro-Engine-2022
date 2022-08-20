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


#ifndef __LSE_TOUCHPATH_H__
#define __LSE_TOUCHPATH_H__

#include "../LSEEngine.h"
#include "Atomic/LSSTDAtomic.h"
#include "Vector/LSMVector2.h"

namespace lse {

	/**
	 * Class CTouchPath
	 * \brief A single touch path on a touch-enabled device.
	 *
	 * Description: A single touch path on a touch-enabled device.
	 */
	class CTouchPath {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CTouchPath();
		LSE_CALLCTOR						~CTouchPath();
		
		
		// == Enumerations.
		/** Metrics. */
		enum LSE_TOUCH_PATH_METRICS {
			LSE_TPM_HISTORY_LENGTH			= 100,			/**< Maximum number of points in a path. */
		};
		
		
		// == Types.
		/** Each entry in the touch path. */
		typedef struct LSE_POINT {
			/** Position of the touch. */
			CVector2						vPos;
			
			/** Time of the touch. */
			uint64_t						ui64Time;
		} * LPLSE_POINT, * const LPCLSE_POINT;


		// == Functions.
		/**
		 * Adds a point to the path.  May cause a past point to be removed if
		 *	the buffer is full.
		 *
		 * \param _vPos The position to add to the path.
		 * \param _ui64Time Timestamp of the point in the path.
		 */
		void LSE_CALL						AddPoint( const CVector2 &_vPos, uint64_t _ui64Time );
		
		/**
		 * Kills the path.  The dead counter begins counting every frame
		 *	after this.
		 */
		void LSE_CALL						Kill();
		
		/**
		 * Resets the path.
		 */
		LSE_INLINE void LSE_CALL			Reset();
		
		/**
		 * Ticks the path (advances the dead counter if it is not 0).
		 */
		LSE_INLINE void LSE_CALL			Tick();
		
		/**
		 * Gets the most recent position in the path.
		 *
		 * \return Returns the last position in the path, or [-1,-1] if there is none.
		 */
		LSE_INLINE CVector2 LSE_CALL		LastPos() const;
		
		/**
		 * Gets the most recent position/timestamp pair in the path.
		 *
		 * \return Returns the last position/timestamp in the path, or {[-1,-1],-1} if there is none.
		 */
		LSE_INLINE LSE_POINT LSE_CALL		LastPosAndTime() const;
		
		/**
		 * Returns the total number of points in the path.
		 *
		 * \return Returns the total number of points in the path.
		 */
		LSE_INLINE uint32_t LSE_CALL		TotalPoints() const;
		
		/**
		 * Returns the dead counter.
		 *
		 * \return Returns the dead counter.
		 */
		LSE_INLINE uint32_t LSE_CALL		DeadCount() const;
		
		/**
		 * Returns true if the path was just ended.
		 *
		 * \return Returns true if the path was just ended.
		 */
		LSE_INLINE LSBOOL LSE_CALL			JustEnded() const;
		
		/**
		 * Returns true if the path was just began.
		 *
		 * \return Returns true if the path was just began.
		 */
		LSE_INLINE LSBOOL LSE_CALL			JustBegan() const;
		
		/**
		 * Gets the ID of the path.
		 *
		 * \return Returns the ID of the path.
		 */
		LSE_INLINE uint32_t LSE_CALL		Id() const;


	protected :
		// == Members.
		/** Our unique ID. */
		CAtomic::ATOM						m_aId;

		/** The buffer of points in the path. */
		LSE_POINT							m_pPath[LSE_TPM_HISTORY_LENGTH];
		
		/** Total points in the path. */
		uint16_t							m_ui16Total;
		
		/** Dead frames. */
		uint16_t							m_ui16DeadFrames;
		
		/** Did we just begin? */
		LSBOOL								m_bJustBegan;
		
		/** ID counter. */
		static CAtomic::ATOM				m_aIdCounter;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Resets the path.
	 */
	LSE_INLINE void LSE_CALL CTouchPath::Reset() {
		m_ui16Total = 0;
		m_ui16DeadFrames = 0;
		m_bJustBegan = false;
		
		// Necessary to increment the ID to avoid bugs in distant code.
		m_aId = CAtomic::InterlockedIncrement( m_aIdCounter );
	}
	
	/**
	 * Ticks the path (advances the dead counter if it is not 0).
	 */
	LSE_INLINE void LSE_CALL CTouchPath::Tick() {
		m_bJustBegan = false;
		if ( m_ui16DeadFrames ) {
			++m_ui16DeadFrames;
		}
	}
	
	/**
	 * Gets the most recent position in the path.
	 *
	 * \return Returns the last position in the path, or [-1,-1] if there is none.
	 */
	LSE_INLINE CVector2 LSE_CALL CTouchPath::LastPos() const {
		if ( !m_ui16Total ) { return CVector2( LSREAL( -1.0 ), LSREAL( -1.0 ) ); }
		return m_pPath[m_ui16Total-1].vPos;
	}
	
	/**
	 * Gets the most recent position/timestamp pair in the path.
	 *
	 * \return Returns the last position/timestamp in the path, or {[-1,-1],-1} if there is none.
	 */
	LSE_INLINE CTouchPath::LSE_POINT LSE_CALL CTouchPath::LastPosAndTime() const {
		static const LSE_POINT pDef = {
			CVector2( LSREAL( -1.0 ), LSREAL( -1.0 ) ),
			LSE_MAXU64
		};
		if ( !m_ui16Total ) { return pDef; }
		return m_pPath[m_ui16Total-1];
	}
	
	/**
	 * Returns the total number of points in the path.
	 *
	 * \return Returns the total number of points in the path.
	 */
	LSE_INLINE uint32_t LSE_CALL CTouchPath::TotalPoints() const {
		return m_ui16Total;
	}
	
	/**
	 * Returns the dead counter.
	 *
	 * \return Returns the dead counter.
	 */
	LSE_INLINE uint32_t LSE_CALL CTouchPath::DeadCount() const {
		return m_ui16DeadFrames;
	}
	
	/**
	 * Returns true if the path was just ended.
	 *
	 * \return Returns true if the path was just ended.
	 */
	LSE_INLINE LSBOOL LSE_CALL CTouchPath::JustEnded() const {
		return DeadCount() == 1;
	}
	
	/**
	 * Returns true if the path was just began.
	 *
	 * \return Returns true if the path was just began.
	 */
	LSE_INLINE LSBOOL LSE_CALL CTouchPath::JustBegan() const {
		return m_bJustBegan;
	}
	
	/**
	 * Gets the ID of the path.
	 *
	 * \return Returns the ID of the path.
	 */
	LSE_INLINE uint32_t LSE_CALL CTouchPath::Id() const {
		return static_cast<uint32_t>(m_aId);
	}
	
}	// namespace lse

#endif	// __LSE_TOUCHPATH_H__

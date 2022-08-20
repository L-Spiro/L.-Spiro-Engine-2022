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


#ifndef __LSE_TOUCH_H__
#define __LSE_TOUCH_H__

#include "../LSEEngine.h"
#include "LSETouchPath.h"

namespace lse {

	/**
	 * Class CTouch
	 * \brief Track touch movements.
	 *
	 * Description: Keep track of touch movements.
	 */
	class CTouch {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CTouch();
		LSE_CALLCTOR						~CTouch();
		
		
		// == Enumerations.
		/** Metrics for touch. */
		enum LSE_TOUCH_METRICS {
			LSE_TM_MAX_TOUCHES				= 20,				/**< Maximum simultaneous touches. */
			LSE_TM_DEAD_FRAMES				= 10,				/**< Number of ticks that paths can be dead before they are removed. */
		};


		// == Functions.
		/**
		 * Advances the touch input one logical tick.  Causes dead
		 *	paths to be removed.
		 */
		void LSE_CALL						Tick();
		
		/**
		 * Adds a touch-down event.  If no paths are available, false is returned.
		 *
		 * \param _vPos The position of the path to start.
		 * \param _ui64Time Time of the path to start.
		 * \return Returns true if the path was added.
		 */
		LSBOOL LSE_CALL						TouchDown( const CVector2 &_vPos, uint64_t _ui64Time );
		
		/**
		 * Adds a touch-move event to a path whose end point matches _vStart.
		 *
		 * \param _vStart The position of the path to which _vEnd will be added.
		 * \param _vEnd Position to add to an existing path whose end point matches _vStart.
		 * \param _ui64Time Time of the event to add to a path whose end point
		 *	is the same as _vStart.
		 * \return Returns true if the path was updated.
		 */
		LSBOOL LSE_CALL						TouchMove( const CVector2 &_vStart, const CVector2 &_vEnd, uint64_t _ui64Time );
		
		/**
		 * Adds a touch-up event.  If no paths at the given position exist, false is returned.
		 *
		 * \param _vStart The position of the path to end.
		 * \param _vEnd The position of the touch-end.
		 * \return Returns true if the path was ended.
		 */
		LSBOOL LSE_CALL						TouchEnd( const CVector2 &_vStart, const CVector2 &_vEnd, uint64_t _ui64Time );
		
		/**
		 * Adds a touch-cancel event.  If no paths at the given position exist, false is returned.
		 *
		 * \param _vStart The position of the path to cancel.
		 * \param _vEnd The position of the touch-cancel.
		 * \return Returns true if the path was cancelled.
		 */
		LSBOOL LSE_CALL						TouchCancel( const CVector2 &_vStart, const CVector2 &_vEnd, uint64_t _ui64Time );
		
		/**
		 * Gets a pointer to a path that ends with the given position or NULL if none exists.
		 *
		 * \param _vPos Position of the path to find.
		 * \return Returns the path that ends with _vPos or NULL if none exists.
		 */
		const CTouchPath * LSE_CALL			GetPath( const CVector2 &_vPos ) const;
		
		/**
		 * Gets a pointer to a path that just began or NULL if none exists.  If multiple
		 *	paths have just began, only the last one is returned.
		 *
		 * \return Returns the last path to have just begun or NULL if no paths just began.
		 */
		const CTouchPath * LSE_CALL			PathThatJustBegun() const;
		
		/**
		 * Gets a pointer to a path given its ID.  Returns NULL if no such path exists.
		 *
		 * \param _ui32Id ID of the path to find.
		 * \return Returns the path with the given ID or NULL if there is no such path.
		 */
		const CTouchPath * LSE_CALL			PathById( uint32_t _ui32Id ) const;
		
		/**
		 * Gets the total number of paths alive or dead.
		 *
		 * \return Returns the total paths.
		 */
		LSE_INLINE uint32_t LSE_CALL		TotalPaths() const;
		
		/**
		 * Gets a pointer to a path given its index.  Returns NULL if no such path exists.
		 *
		 * \param _ui32Index Index of the path to get.
		 * \return Returns the path with the given index or NULL if there is no such path.
		 */
		LSE_INLINE const CTouchPath * LSE_CALL
											PathByIndex( uint32_t _ui32Index ) const;


	protected :
		// == Members.
		/** Array of touch paths. */
		CTouchPath							m_tpPaths[LSE_TM_MAX_TOUCHES];
		
		/** Total paths. */
		uint32_t							m_ui32Total;
		
		
		// == Functions.
		/**
		 * Adds a path and returns the resulting index of the path, which if
		 *	is equal to LSE_TM_MAX_TOUCHES implies that no new path was added.
		 *
		 * \return Returns the index of the added path.
		 */
		uint32_t LSE_CALL					AddPath();
		
		/**
		 * Removes a path at the given index.
		 *
		 * \param _ui32Index Index of the path to remove.
		 */
		void LSE_CALL						RemPath( uint32_t _ui32Index );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the total number of paths alive or dead.
	 *
	 * \return Returns the total paths.
	 */
	LSE_INLINE uint32_t LSE_CALL CTouch::TotalPaths() const {
		return m_ui32Total;
	}
	
	/**
	 * Gets a pointer to a path given its index.  Returns NULL if no such path exists.
	 *
	 * \param _ui32Index Index of the path to get.
	 * \return Returns the path with the given index or NULL if there is no such path.
	 */
	LSE_INLINE const CTouchPath * LSE_CALL CTouch::PathByIndex( uint32_t _ui32Index ) const {
		return _ui32Index < TotalPaths() ? &m_tpPaths[_ui32Index] : NULL;
	}
	
}	// namespace lse

#endif	// __LSE_TOUCH_H__

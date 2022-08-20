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
 * Description: Gathers input on the main thread and feeds them in virtually timed sections to the game thread.
 */


#ifndef __LSE_INPUTQUEUE_H__
#define __LSE_INPUTQUEUE_H__

#include "../LSEEngine.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "LSEInputQueueKeyDown.h"
#include "LSEInputQueueKeyUp.h"
#include "LSEInputQueueMouseDown.h"
#include "LSEInputQueueMouseMove.h"
#include "LSEInputQueueMouseUp.h"
#include "LSEInputQueueMouseWheel.h"
#include "LSEInputQueueTouchBegin.h"
#include "LSEInputQueueTouchCancelled.h"
#include "LSEInputQueueTouchEnd.h"
#include "LSEInputQueueTouchMove.h"
#include "RingBuffer/LSTLRingBufferPoD.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** CStd::Max() won't work as an argument for templates or enumerated values. */
#define LSE_IQ_MAX( LEFT, RIGHT )				((LEFT > RIGHT) ? LEFT : RIGHT)

namespace lse {

	// == Types.
	/** Ring-buffer element size. */
	enum {
		LSE_IQ_TIER0							= sizeof( LSE_INPUTQUEUEKEYDOWN ),
		LSE_IQ_TIER1							= LSE_IQ_MAX( LSE_IQ_TIER0, sizeof( LSE_INPUTQUEUEKEYUP ) ),
		LSE_IQ_TIER2							= LSE_IQ_MAX( LSE_IQ_TIER1, sizeof( LSE_INPUTQUEUEMOUSEDOWN ) ),
		LSE_IQ_TIER3							= LSE_IQ_MAX( LSE_IQ_TIER2, sizeof( LSE_INPUTQUEUEMOUSEUP ) ),
		LSE_IQ_TIER4							= LSE_IQ_MAX( LSE_IQ_TIER3, sizeof( LSE_INPUTQUEUEMOUSEMOVE ) ),
		LSE_IQ_TIER5							= LSE_IQ_MAX( LSE_IQ_TIER4, sizeof( LSE_INPUTQUEUEMOUSEWHEEL ) ),
		LSE_IQ_TIER6							= LSE_IQ_MAX( LSE_IQ_TIER5, sizeof( LSE_INPUTQUEUETOUCHBEGIN ) ),
		LSE_IQ_TIER7							= LSE_IQ_MAX( LSE_IQ_TIER6, sizeof( LSE_INPUTQUEUETOUCHMOVE ) ),
		LSE_IQ_TIER8							= LSE_IQ_MAX( LSE_IQ_TIER7, sizeof( LSE_INPUTQUEUETOUCHEND ) ),
		LSE_IQ_TIER9							= LSE_IQ_MAX( LSE_IQ_TIER8, sizeof( LSE_INPUTQUEUETOUCHCANCELLED ) ),
		LSE_IQ_RING_BUFFER_ELEMENT_SIZE			= LSE_IQ_TIER9,				/**< Size of each element in the ring buffer. */
		LSE_IQ_MAX_QUEUE						= 100,						/**< Total input items to queue in the input queue. */
	};

	/**
	 * Class CInputQueue
	 * \brief Gathers input on the main thread and feeds them in virtually timed sections to the game thread.
	 *
	 * Description: Gathers input on the main thread and feeds them in virtually timed sections to the game thread.
	 */
	class CInputQueue {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CInputQueue();
		LSE_CALLCTOR							~CInputQueue();


		// == Functions.
		/**
		 * Adds a key-down event.
		 *
		 * \param _kcCode The key being pressed.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnKeyDown( LSE_KEYCODES _kcCode, uint64_t _ui64Time );

		/**
		 * Adds a key-up event.
		 *
		 * \param _kcCode The key being up.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnKeyUp( LSE_KEYCODES _kcCode, uint64_t _ui64Time );

		/**
		 * Adds a mouse-move event.
		 *
		 * \param _fX The X position.
		 * \param _fY The Y position.
		 * \param _bAbsolute If true, the given position is absolute.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnMouseMove( LSREAL _fX, LSREAL _fY, LSBOOL _bAbsolute, uint64_t _ui64Time );

		/**
		 * Adds a mouse button-down event.
		 *
		 * \param _mbcCode Mouse button code of the mouse button clicked.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnMouseDown( LSE_MOUSEBUTTONCODES _mbcCode, uint64_t _ui64Time );

		/**
		 * Adds a mouse button-up event.
		 *
		 * \param _mbcCode Mouse button code of the mouse button unclicked.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnMouseUp( LSE_MOUSEBUTTONCODES _mbcCode, uint64_t _ui64Time );
		
		/**
		 * Adds a touch-begin event.
		 *
		 * \param _fX X position of the touch, relative to the game view.
		 * \param _fY Y position of the touch, relative to the game view.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnTouchBegin( LSREAL _fX, LSREAL _fY, uint64_t _ui64Time );
		
		/**
		 * Adds a touch-move event.
		 *
		 * \param _fX Starting X position of the touch, relative to the game view.
		 * \param _fY Starting Y position of the touch, relative to the game view.
		 * \param _fEndX Final X position of the touch, relative to the game view.
		 * \param _fEndY Final Y position of the touch, relative to the game view.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnTouchMove( LSREAL _fX, LSREAL _fY,
			LSREAL _fEndX, LSREAL _fEndY, uint64_t _ui64Time );
			
		/**
		 * Adds a touch-end event.
		 *
		 * \param _fX Starting X position of the touch, relative to the game view.
		 * \param _fY Starting Y position of the touch, relative to the game view.
		 * \param _fEndX Final X position of the touch, relative to the game view.
		 * \param _fEndY Final Y position of the touch, relative to the game view.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnTouchEnd( LSREAL _fX, LSREAL _fY,
			LSREAL _fEndX, LSREAL _fEndY, uint64_t _ui64Time );
			
		/**
		 * Adds a touch-cancel event.
		 *
		 * \param _fX Starting X position of the touch, relative to the game view.
		 * \param _fY Starting Y position of the touch, relative to the game view.
		 * \param _fEndX Final X position of the touch, relative to the game view.
		 * \param _fEndY Final Y position of the touch, relative to the game view.
		 * \param _ui64Time Time of the event in microseconds.
		 */
		void LSE_CALL							OnTouchCancel( LSREAL _fX, LSREAL _fY,
			LSREAL _fEndX, LSREAL _fEndY, uint64_t _ui64Time );

		/**
		 * Gets input events up to the given time stamp, inclusively.
		 *
		 * \param _ui64Time Time in microseconds up to which to get input events.
		 * \param _ppiqibReturn Holds the returned array of pointers to LSE_INPUTQUEUEITEMBASE structures.
		 *	There must be at least LSE_IQ_MAX_QUEUE items allocated in the array.
		 * \return Returns the number of inputs returned in _piqibReturn.
		 */
		uint32_t LSE_CALL						EatInputs( uint64_t _ui64Time, const LSE_INPUTQUEUEITEMBASE ** _ppiqibReturn ) const;

		/**
		 * Removes the given number of inputs from the bottom of the queue.
		 *
		 * \param _ui32Total Number of items to remove.
		 */
		void LSE_CALL							RemoveInputs( uint32_t _ui32Total );

		/**
		 * Removes all inputs.
		 */
		void LSE_CALL							RemoveAllInputs();

		

	protected :
		// == Members.
		/** The critical section. */
		CCriticalSection						m_csCrit;

		/** The input ring buffer. */
		CRingBufferPoD<LSE_IQ_RING_BUFFER_ELEMENT_SIZE, LSE_IQ_MAX_QUEUE>
												m_rbpdRingBuffer;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

}	// namespace lse

#endif	// __LSE_INPUTQUEUE_H__

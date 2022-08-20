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

#include "LSEInputQueue.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CInputQueue::CInputQueue() {
	}
	LSE_CALLCTOR CInputQueue::~CInputQueue() {
	}

	// == Functions.
	/**
	 * Adds a key-down event.
	 *
	 * \param _kcCode The key being pressed.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnKeyDown( LSE_KEYCODES _kcCode, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUEKEYDOWN * piqkdEvent = static_cast<LSE_INPUTQUEUEKEYDOWN *>(m_rbpdRingBuffer.Alloc());
		if ( !piqkdEvent ) { return; }

		piqkdEvent->itType = LSE_IT_KEYBOARDBTNDWN;
		piqkdEvent->kcKey = _kcCode;
		piqkdEvent->ui64Time = _ui64Time;
	}

	/**
	 * Adds a key-up event.
	 *
	 * \param _kcCode The key being up.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnKeyUp( LSE_KEYCODES _kcCode, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUEKEYUP * piqkuEvent = static_cast<LSE_INPUTQUEUEKEYUP *>(m_rbpdRingBuffer.Alloc());
		if ( !piqkuEvent ) { return; }

		piqkuEvent->itType = LSE_IT_KEYBOARDBTNUP;
		piqkuEvent->kcKey = _kcCode;
		piqkuEvent->ui64Time = _ui64Time;
	}

	/**
	 * Adds a mouse-move event.
	 *
	 * \param _fX The X position.
	 * \param _fY The Y position.
	 * \param _bAbsolute If true, the given position is absolute.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnMouseMove( LSREAL _fX, LSREAL _fY, LSBOOL _bAbsolute, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUEMOUSEMOVE * piqmmEvent = static_cast<LSE_INPUTQUEUEMOUSEMOVE *>(m_rbpdRingBuffer.Alloc());
		if ( !piqmmEvent ) { return; }

		piqmmEvent->itType = LSE_IT_MOUSEMOVE;
		piqmmEvent->fX = _fX;
		piqmmEvent->fY = _fY;
		piqmmEvent->bAbsolute = _bAbsolute;
		piqmmEvent->ui64Time = _ui64Time;
	}

	/**
	 * Adds a mouse button-down event.
	 *
	 * \param _mbcCode Mouse button code of the mouse button clicked.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnMouseDown( LSE_MOUSEBUTTONCODES _mbcCode, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUEMOUSEDOWN * piqmdEvent = static_cast<LSE_INPUTQUEUEMOUSEDOWN *>(m_rbpdRingBuffer.Alloc());
		if ( !piqmdEvent ) { return; }

		piqmdEvent->itType = LSE_IT_MOUSEBTNDWN;
		piqmdEvent->mbcButton = _mbcCode;
		piqmdEvent->ui64Time = _ui64Time;
	}

	/**
	 * Adds a mouse button-up event.
	 *
	 * \param _mbcCode Mouse button code of the mouse button unclicked.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnMouseUp( LSE_MOUSEBUTTONCODES _mbcCode, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUEMOUSEUP * piqmdEvent = static_cast<LSE_INPUTQUEUEMOUSEUP *>(m_rbpdRingBuffer.Alloc());
		if ( !piqmdEvent ) { return; }

		piqmdEvent->itType = LSE_IT_MOUSEBTNUP;
		piqmdEvent->mbcButton = _mbcCode;
		piqmdEvent->ui64Time = _ui64Time;
	}
	
	/**
	 * Adds a touch-begin event.
	 *
	 * \param _fX X position of the touch, relative to the game view.
	 * \param _fY Y position of the touch, relative to the game view.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnTouchBegin( LSREAL _fX, LSREAL _fY, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUETOUCHBEGIN * piqtbEvent = static_cast<LSE_INPUTQUEUETOUCHBEGIN *>(m_rbpdRingBuffer.Alloc());
		if ( !piqtbEvent ) { return; }

		piqtbEvent->itType = LSE_IT_TOUCHBEGIN;
		piqtbEvent->fX = _fX;
		piqtbEvent->fY = _fY;
		piqtbEvent->ui64Time = _ui64Time;
	}
	
	/**
	 * Adds a touch-move event.
	 *
	 * \param _fX Starting X position of the touch, relative to the game view.
	 * \param _fY Starting Y position of the touch, relative to the game view.
	 * \param _fEndX Final X position of the touch, relative to the game view.
	 * \param _fEndY Final Y position of the touch, relative to the game view.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnTouchMove( LSREAL _fX, LSREAL _fY,
		LSREAL _fEndX, LSREAL _fEndY, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUETOUCHMOVE * piqtmEvent = static_cast<LSE_INPUTQUEUETOUCHMOVE *>(m_rbpdRingBuffer.Alloc());
		if ( !piqtmEvent ) { return; }

		piqtmEvent->itType = LSE_IT_TOUCHMOVE;
		piqtmEvent->fX = _fX;
		piqtmEvent->fY = _fY;
		piqtmEvent->fEndX = _fEndX;
		piqtmEvent->fEndY = _fEndY;
		piqtmEvent->ui64Time = _ui64Time;
	}
	
	/**
	 * Adds a touch-end event.
	 *
	 * \param _fX Starting X position of the touch, relative to the game view.
	 * \param _fY Starting Y position of the touch, relative to the game view.
	 * \param _fEndX Final X position of the touch, relative to the game view.
	 * \param _fEndY Final Y position of the touch, relative to the game view.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnTouchEnd( LSREAL _fX, LSREAL _fY,
		LSREAL _fEndX, LSREAL _fEndY, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUETOUCHEND * piqteEvent = static_cast<LSE_INPUTQUEUETOUCHEND *>(m_rbpdRingBuffer.Alloc());
		if ( !piqteEvent ) { return; }

		piqteEvent->itType = LSE_IT_TOUCHEND;
		piqteEvent->fX = _fX;
		piqteEvent->fY = _fY;
		piqteEvent->fEndX = _fEndX;
		piqteEvent->fEndY = _fEndY;
		piqteEvent->ui64Time = _ui64Time;
	}
	
	/**
	 * Adds a touch-cancel event.
	 *
	 * \param _fX Starting X position of the touch, relative to the game view.
	 * \param _fY Starting Y position of the touch, relative to the game view.
	 * \param _fEndX Final X position of the touch, relative to the game view.
	 * \param _fEndY Final Y position of the touch, relative to the game view.
	 * \param _ui64Time Time of the event in microseconds.
	 */
	void LSE_CALL CInputQueue::OnTouchCancel( LSREAL _fX, LSREAL _fY,
		LSREAL _fEndX, LSREAL _fEndY, uint64_t _ui64Time ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSE_INPUTQUEUETOUCHCANCELLED * piqtcEvent = static_cast<LSE_INPUTQUEUETOUCHCANCELLED *>(m_rbpdRingBuffer.Alloc());
		if ( !piqtcEvent ) { return; }

		piqtcEvent->itType = LSE_IT_TOUCHCANCEL;
		piqtcEvent->fX = _fX;
		piqtcEvent->fY = _fY;
		piqtcEvent->fEndX = _fEndX;
		piqtcEvent->fEndY = _fEndY;
		piqtcEvent->ui64Time = _ui64Time;
	}

	/**
	 * Gets input events up to the given time stamp, inclusively.
	 *
	 * \param _ui64Time Time in microseconds up to which to get input events.
	 * \param _ppiqibReturn Holds the returned array of pointers to LSE_INPUTQUEUEITEMBASE structures.
	 *	There must be at least LSE_IQ_MAX_QUEUE items allocated in the array.
	 * \return Returns the number of inputs returned in _piqibReturn.
	 */
	uint32_t LSE_CALL CInputQueue::EatInputs( uint64_t _ui64Time, const LSE_INPUTQUEUEITEMBASE ** _ppiqibReturn ) const {
		const uint8_t * pui8Base = static_cast<const uint8_t *>(m_rbpdRingBuffer.FirstAllocated());

		uint32_t ui32Total = static_cast<uint32_t>(m_rbpdRingBuffer.Counter().TotalAllocated() / LSE_IQ_RING_BUFFER_ELEMENT_SIZE);
		uint32_t ui32Count = 0;
		for ( ui32Count = 0; ui32Count < ui32Total; ++ui32Count ) {
			const LSE_INPUTQUEUEITEMBASE * piqibBase = reinterpret_cast<const LSE_INPUTQUEUEITEMBASE *>(pui8Base);
			if ( piqibBase->ui64Time > _ui64Time ) { break; }
			(*_ppiqibReturn++) = piqibBase;
			pui8Base += LSE_IQ_RING_BUFFER_ELEMENT_SIZE;
		}
		return ui32Count;
	}

	/**
	 * Removes the given number of inputs from the bottom of the queue.
	 *
	 * \param _ui32Total Number of items to remove.
	 */
	void LSE_CALL CInputQueue::RemoveInputs( uint32_t _ui32Total ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		while ( _ui32Total-- ) {
			m_rbpdRingBuffer.Free();
		}
	}

	/**
	 * Removes all inputs.
	 */
	void LSE_CALL CInputQueue::RemoveAllInputs() {
		CCriticalSection::CLocker lLock( m_csCrit );
		RemoveInputs( static_cast<uint32_t>(m_rbpdRingBuffer.Counter().TotalAllocated() / LSE_IQ_RING_BUFFER_ELEMENT_SIZE) );
		
	}

}	// namespace lse

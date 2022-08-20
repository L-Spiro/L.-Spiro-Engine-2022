/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: An N-buffered vertex buffer.  This allows double-, triple-, etc. buffering.
 */


#ifndef __LSG_NBUFFERVERTEXBUFFER_H__
#define __LSG_NBUFFERVERTEXBUFFER_H__

#include "../LSGGraphicsLib.h"
#include "LSGVertexBuffer.h"

namespace lsg {

	/**
	 * Class CNBufferVertexBuffer
	 * \brief An N-buffered vertex buffer.
	 *
	 * Description: An N-buffered vertex buffer.  This allows double-, triple-, etc. buffering.
	 */
	template <unsigned _uN>
	class CNBufferVertexBuffer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CNBufferVertexBuffer() :
			m_ui32Index( 0 ) {
		}
		LSE_CALLCTOR							~CNBufferVertexBuffer() {
		}


		// == Functions.
		/**
		 * Swaps to the next vertex buffer.
		 */
		void LSE_CALL							Swap() {
			m_ui32Index = (m_ui32Index + 1) % _uN;
		}

		/**
		 * Gets the current vertex buffer.
		 *
		 * \return Retruns a reference to the current vertex buffer.
		 */
		CVertexBuffer & LSE_CALL				Cur() {
			return m_vbBuffers[m_ui32Index];
		}

		/**
		 * Resets all vertex buffers.
		 */
		void LSE_CALL							Reset() {
			for ( uint32_t I = _uN; I--; ) {
				m_vbBuffers[I].Reset();
			}
			m_ui32Index = 0;
		}

		/**
		 * Gets the total buffers.
		 *
		 * \return Returns the total number of buffers.
		 */
		uint32_t LSE_CALL						TotalBuffers() const {
			return _uN;
		}

		/**
		 * Gets a buffer by index.
		 *
		 * \param _ui32Index Index of the buffer to get.
		 * \return Returns a reference to the given vertex buffer.
		 */
		CVertexBuffer & LSE_FCALL				GetBuffer( uint32_t _ui32Index ) {
			return m_vbBuffers[_ui32Index];
		}


	protected :
		// == Members.
		/** The current buffer index. */
		uint32_t								m_ui32Index;

		/** The array of vertex buffers. */
		CVertexBuffer							m_vbBuffers[_uN];
	};

}	// namespace lsg

#endif	// __LSG_NBUFFERVERTEXBUFFER_H__

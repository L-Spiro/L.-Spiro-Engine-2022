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
 * Description: The base class for all vertex buffers.
 */


#ifndef __LSG_VERTEXBUFFERBASE_H__
#define __LSG_VERTEXBUFFERBASE_H__

#include "../LSGGraphicsLib.h"
#include "OwnedOrBorrowed/LSTLOwnedOrBorrowedPointer.h"

namespace lsg {

	// == Types.
	/** A declaration of a single element in the vertex buffer. */
	typedef struct LSG_VERTEX_ELEMENT_DESC {
		uint32_t										ui32Format;				/**< Type of the element (LSG_VERTEX_ELEMENT). */
		uint32_t										ui32Offset;				/**< Offset of the element from the start of the vertex element.  0 means it immediately follows the previous element. */
		uint32_t										ui32Semantic;			/**< Semantic of the element (LSG_SEMANTICS). */
		uint32_t										ui32SemanticIndex;		/**< Semantic index of the element. */
	} * LPLSG_VERTEX_ELEMENT_DESC, * const LPCLSG_VERTEX_ELEMENT_DESC;

	/** Mapping of a vertex buffer for updating. */
	typedef struct LSG_VERTEX_BUFFER_MAP {
		void *										pvData;					/**< Pointer to the data to which the mapping has been set. */
		uint32_t										ui32Size;				/**< Size in bytes of the data to which pvData points. */
	} * LPLSG_VERTEX_BUFFER_MAP, * const LPCLSG_VERTEX_BUFFER_MAP;

	/**
	 * Class CVertexBufferBase
	 * \brief The base class for all vertex buffers.
	 *
	 * Description: The base class for all vertex buffers.
	 */
	class CVertexBufferBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR									CVertexBufferBase();


		// == Functions.
		/**
		 * Resets the vertex buffer base, freeing all allocated memory.
		 */
		void LSE_CALL									Reset();

		/**
		 * Gets the ID of this vertex buffer segment.
		 *
		 * \return Returns the ID of this vertex buffer segment.
		 */
		LSE_INLINE uint32_t LSE_CALL					GetId() const;

		/**
		 * Gets the topology.
		 *
		 * \return Returns the topology.
		 */
		LSE_INLINE uint32_t LSE_CALL					Topology() const;

		/**
		 * Gets the usage.
		 *
		 * \return Returns the usage.
		 */
		LSE_INLINE uint32_t LSE_CALL					Usage() const;

		/**
		 * Gets the element size.
		 *
		 * \return Returns the element size.
		 */
		LSE_INLINE uint32_t LSE_CALL					ElementSize() const;

		/**
		 * Gets the number of elements.
		 *
		 * \return Returns the number of elements.
		 */
		LSE_INLINE uint32_t LSE_CALL					Elements() const;

		/**
		 * Gets the number of bytes between the starts of successive elements.
		 *
		 * \return Returns the number of bytes between the starts of successive elements.
		 */
		LSE_INLINE uint32_t LSE_CALL					Stride() const;

		/**
		 * Gets the number of primitives.
		 *
		 * \return Returns the number of primitives.
		 */
		LSE_INLINE uint32_t LSE_CALL					Primitives() const;

	protected :
		// == Members.
		/** Topology. */
		uint32_t										m_ui32Topology;

		/** Usage. */
		uint32_t										m_ui32Usage;

		/** The lock count. */
		//uint32_t										m_ui32Lock;

		/** Have we finalized? */
		LSBOOL											m_bFinalized;

		/** The stride of the vertex buffer.  Usually a multiple of 32. */
		uint32_t										m_ui32Stride;

		/** The total number of elements in the vertex buffer. */
		uint32_t										m_ui32Elements;

		/** The total number of primitives in the vertex buffer. */
		uint32_t										m_ui32PrimitiveCount;

		/** Total in the declaration. */
		uint16_t										m_ui16TotalDecl;

		/** The size of a single element in the buffer. */
		uint16_t										m_ui16ElementSize;

		/** The vertex buffer ID. */
		uint32_t										m_ui32Id;

		/** Vertex buffer data. */
		COwnedOrBorrowedPointer<uint8_t>				m_oobpBuffer;

		/** Vertex buffer ID values. */
		static uint32_t									m_ui32VertIdCounter;

		/** Critical section. */
		static CCriticalSection							m_csCrit;


		// == Functions.
		/**
		 * Generates a new vertex-buffer ID.
		 *
		 * \return Returns a new vertex-buffer ID.
		 */
		static LSE_INLINE uint32_t LSE_CALL				NewId();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the ID of this vertex buffer segment.
	 *
	 * \return Returns the ID of this vertex buffer segment.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::GetId() const {
		return m_ui32Id;
	}

	/**
	 * Gets the topology.
	 *
	 * \return Returns the topology.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::Topology() const {
		return m_ui32Topology;
	}

	/**
	 * Gets the usage.
	 *
	 * \return Returns the usage.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::Usage() const {
		return m_ui32Usage;
	}

	/**
	 * Gets the element size.
	 *
	 * \return Returns the element size.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::ElementSize() const {
		return m_ui16ElementSize;
	}

	/**
	 * Gets the number of elements.
	 *
	 * \return Returns the number of elements.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::Elements() const {
		return m_ui32Elements;
	}

	/**
	 * Gets the number of bytes between the starts of successive elements.
	 *
	 * \return Returns the number of bytes between the starts of successive elements.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::Stride() const {
		return m_ui32Stride;
	}

	/**
	 * Gets the number of primitives.
	 *
	 * \return Returns the number of primitives.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::Primitives() const {
		return m_ui32PrimitiveCount;
	}

	/**
	 * Generates a new vertex-buffer ID.
	 *
	 * \return Returns a new vertex-buffer ID.
	 */
	LSE_INLINE uint32_t LSE_CALL CVertexBufferBase::NewId() {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		return ++m_ui32VertIdCounter;
	}

}	// namespace lsg

#endif	// __LSG_VERTEXBUFFERBASE_H__

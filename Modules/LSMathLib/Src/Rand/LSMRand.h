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
 * Description: Random-number class.  Uses the Mersenne Twister approach due to Makoto Matsumoto, Takuji Nishimura,
 *	and Shawn Cokus.
 *
 * Reference:
 *	M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform Pseudo-Random
 *	Number Generator", ACM Transactions on Modeling and Computer Simulation, Vol. 8, No. 1, January pp.3-30 (1998).
 *
 *	Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 *	Copyright (C) 2000 - 2003, Richard J. Wagner
 *	All rights reserved.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/ARTICLES/earticles.html
 * http://portal.acm.org/citation.cfm?doid=272991.272995
 */


#ifndef __LSM_RAND_H__
#define __LSM_RAND_H__

#include "../LSMMathLib.h"

#if defined( LSE_VISUALSTUDIO )
#pragma warning( push )

// warning C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning( disable : 4146 )
#endif	// #if defined( LSE_VISUALSTUDIO )

namespace lsm {

	/**
	 * Class CRand
	 * \brief Random-number generator.
	 *
	 * Description: Random-number class.  Uses the Mersenne Twister approach due to Makoto Matsumoto, Takuji Nishimura,
	 *	and Shawn Cokus.
	 */
	class CRand {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR			CRand();


		// == Types.
		/**
		 * Length of the state array.
		 */
		enum {
			LSM_STATE_LEN				= 624,
			LSM_STATE_LEN_ARRAY
		};


		// == Functions.
		/**
		 * Seed the number generator.
		 *
		 * \param _ui32Seed The seed, usually obtained by getting the current time.
		 */
		void LSE_FCALL				Seed( uint32_t _ui32Seed );

		/**
		 * Gets a random unsigned integer.
		 *
		 * \return Returns a random unsigned 32-bit integer from 0x00000000 to 0xFFFFFFFF.
		 */
		uint32_t LSE_FCALL				RandUInt32() const;

		/**
		 * Gets an unsigned integer within a certain range (exclusive).
		 *
		 * \param _ui32Max Upper bounds of the number, exclusive.  Number is generated from 0
		 *	to (_ui32Max - 1).
		 * \return A random number from 0 to (_ui32Max - 1).
		 */
		uint32_t LSE_FCALL				RandUInt32( uint32_t _ui32Max ) const;

		/**
		 * Gets a random unsigned integer.
		 *
		 * \return Returns a random unsigned 64-bit integer from 0x0000000000000000ULL to 0xFFFFFFFFFFFFFFFFULL.
		 */
		uint64_t LSE_FCALL				RandUInt64() const;

		/**
		 * Gets an unsigned integer within a certain range (exclusive).
		 *
		 * \param _ui64Max Upper bounds of the number, exclusive.  Number is generated from 0
		 *	to (_ui64Max - 1).
		 * \return A random number from 0 to (_ui64Max - 1).
		 */
		uint64_t LSE_FCALL				RandUInt64( uint64_t _ui64Max ) const;

		/**
		 * Gets a random floating-point number from 0 to 1 inclusive.
		 *
		 * \return A random LSREAL value from 0 to 1 inclusive.
		 */
		LSREAL LSE_FCALL				RandReal() const;

		/**
		 * Gets a random floating-point number from _fLow to _fHigh inclusive.
		 *
		 * \param _fLow Lowest allowed value.
		 * \param _fHigh Highest allowed value.
		 * \return Returns a random LSREAL between _fLow and _fHigh inclusive.
		 */
		LSREAL LSE_FCALL				RandReal( LSREAL _fLow, LSREAL _fHigh ) const;

	protected :
		// == Types.
		/**
		 * Period.
		 */
		enum {
			LSM_PERIOD					= 397
		};


		// == Members.
		// State array.
		mutable uint32_t				m_ui32State[LSM_STATE_LEN_ARRAY];

		// Next value to get from the state.
		mutable uint32_t *				m_pui32NextInState;

		// Values left in the state before a reload is required.
		mutable int32_t					m_i32LeftInState;


		// == Functions.
		/**
		 * Initialize the state array with a seed.
		 *
		 * \param _ui32Seed The seed.
		 */
		void LSE_FCALL				Initialize( uint32_t _ui32Seed ) const;

		/**
		 * Refill the state array.
		 */
		void LSE_FCALL				Reload() const;

		/**
		 * Gets the highest bit.
		 *
		 * \param _ui32Val Value whose highest bit is to be gotten.
		 * \return Returns the highest bit in the given value.
		 */
		static uint32_t LSE_FCALL		HiBit( const uint32_t & _ui32Val );

		/**
		 * Gets the lowest bit.
		 *
		 * \param _ui32Val Value whose lowest bit is to be gotten.
		 * \return Returns the lowest bit in the given value.
		 */
		static uint32_t LSE_FCALL		LoBit( const uint32_t & _ui32Val );

		/**
		 * Gets all bits but the highest bit.
		 *
		 * \param _ui32Val Value whose lower bots are to be obtained.
		 * \return Returns all but the highest bit.
		 */
		static uint32_t LSE_FCALL		LoBits( const uint32_t & _ui32Val );

		/**
		 * Mixes bits.
		 *
		 * \param _ui32L Left operand.
		 * \param _ui32R Right operand.
		 * \return Returns the mixed bits.
		 */
		static uint32_t LSE_FCALL		MixBits( const uint32_t & _ui32L, const uint32_t & _ui32R );

		/**
		 * "Mersenne Twist" up values.
		 *
		 * \param _ui32F A value.
		 * \param _ui32L A value.
		 * \param _ui32R A value.
		 * \return Returns the twisted value.
		 */
		static uint32_t LSE_FCALL		MersenneTwist( const uint32_t & _ui32F, const uint32_t & _ui32L, const uint32_t & _ui32R );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CRand::CRand() :
		m_pui32NextInState( NULL ),
		m_i32LeftInState( LSM_STATE_LEN_ARRAY ) {
		Seed( 19650218 );
	}

	// == Functions.
	/**
	 * Seed the number generator.
	 *
	 * \param _ui32Seed The seed, usually obtained by getting the current time.
	 */
	LSE_INLINE void LSE_FCALL CRand::Seed( uint32_t _ui32Seed ) {
		Initialize( _ui32Seed );
		Reload();
	}

	/**
	 * Gets a random unsigned integer.
	 *
	 * \return Returns a random unsigned 32-bit integer from 0x00000000 to 0xFFFFFFFF.
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::RandUInt32() const {
		if ( m_i32LeftInState == 0 ) { Reload(); }
		--m_i32LeftInState;

		uint32_t ui32Val;
		ui32Val = (*m_pui32NextInState++);
		ui32Val ^= (ui32Val >> 11);
		ui32Val ^= (ui32Val <<  7) & 0x9D2C5680UL;
		ui32Val ^= (ui32Val << 15) & 0xEFC60000UL;
		return (ui32Val ^ (ui32Val >> 18));
	}

	/**
	 * Gets an unsigned integer within a certain range (exclusive).
	 *
	 * \param _ui32Max Upper bounds of the number, exclusive.  Number is generated from 0
	 *	to (_ui32Max - 1).
	 * \return A random number from 0 to (_ui32Max - 1).
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::RandUInt32( uint32_t _ui32Max ) const {
		return static_cast<uint32_t>(RandUInt32() * (static_cast<double>(1.0 / 4294967296.0)) * _ui32Max);
	}

	/**
	 * Gets a random unsigned integer.
	 *
	 * \return Returns a random unsigned 64-bit integer from 0x0000000000000000ULL to 0xFFFFFFFFFFFFFFFFULL.
	 */
	LSE_INLINE uint64_t LSE_FCALL CRand::RandUInt64() const {
		uint64_t ui64High = RandUInt32();
		return RandUInt32() | (ui64High << 32ULL);
	}

	/**
	 * Gets an unsigned integer within a certain range (exclusive).
	 *
	 * \param _ui64Max Upper bounds of the number, exclusive.  Number is generated from 0
	 *	to (_ui64Max - 1).
	 * \return A random number from 0 to (_ui64Max - 1).
	 */
	LSE_INLINE uint64_t LSE_FCALL CRand::RandUInt64( uint64_t _ui64Max ) const {
		// If the range can be supported by RandUInt32(), revert to that.
		if ( _ui64Max <= (1ULL << 32ULL) ) { return RandUInt32( static_cast<uint32_t>(_ui64Max) ); }
		// Otherwise the low part can be any random number and the high part is capped.
		uint64_t ui64High = RandUInt32( static_cast<uint32_t>(_ui64Max >> 32ULL) );
		return RandUInt32() | (ui64High << 32ULL);
	}

	/**
	 * Gets a random floating-point number from 0 to 1 inclusive.
	 *
	 * \return A random LSREAL value from 0 to 1 inclusive.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRand::RandReal() const {
		return static_cast<LSREAL>(RandUInt32() * (static_cast<double>(1.0 / 4294967295.0)));
	}

	/**
	 * Gets a random floating-point number from _fLow to _fHigh inclusive.
	 *
	 * \param _fLow Lowest allowed value.
	 * \param _fHigh Highest allowed value.
	 * \return Returns a random LSREAL between _fLow and _fHigh inclusive.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRand::RandReal( LSREAL _fLow, LSREAL _fHigh ) const {
		return _fLow + RandReal() * (_fHigh - _fLow);
	}

	/**
	 * Initialize the state array with a seed.
	 *
	 * \param _ui32Seed The seed.
	 */
	LSE_INLINE void LSE_FCALL CRand::Initialize( uint32_t _ui32Seed ) const {
		uint32_t * pui32Cur = m_ui32State;
		uint32_t * pui32Next = m_ui32State;
		(*pui32Next++) = _ui32Seed;
		for ( uint32_t I = 1; I < LSM_STATE_LEN_ARRAY; ++I ) {
			(*pui32Next++) = (1812433253 * ((*pui32Cur) ^ ((*pui32Cur) >> 30)) + I) & 0xFFFFFFFF;
			++pui32Cur;
		}
	}

	/**
	 * Refill the state array.
	 */
	LSE_INLINE void LSE_FCALL CRand::Reload() const {

		uint32_t * pui32Cur = m_ui32State;
		for ( uint32_t I = LSM_STATE_LEN_ARRAY - LSM_PERIOD; I--; ++pui32Cur ) {
			(*pui32Cur) = MersenneTwist( pui32Cur[LSM_PERIOD], pui32Cur[0], pui32Cur[1] );
		}
		for ( uint32_t I = LSM_PERIOD; --I; ++pui32Cur ) {
			(*pui32Cur) = MersenneTwist( pui32Cur[LSM_PERIOD-LSM_STATE_LEN_ARRAY], pui32Cur[0], pui32Cur[1] );
		}
		(*pui32Cur) = MersenneTwist( pui32Cur[LSM_PERIOD-LSM_STATE_LEN_ARRAY], pui32Cur[0], m_ui32State[0] );

		m_i32LeftInState = LSM_STATE_LEN_ARRAY;
		m_pui32NextInState = m_ui32State;
	}


	/**
	 * Gets the highest bit.
	 *
	 * \param _ui32Val Value whose highest bit is to be gotten.
	 * \return Returns the highest bit in the given value.
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::HiBit( const uint32_t & _ui32Val ) {
		return _ui32Val & 0x80000000;
	}

	/**
	 * Gets the lowest bit.
	 *
	 * \param _ui32Val Value whose lowest bit is to be gotten.
	 * \return Returns the lowest bit in the given value.
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::LoBit( const uint32_t & _ui32Val ) {
		return _ui32Val & 0x00000001;
	}

	/**
	 * Gets all bits but the highest bit.
	 *
	 * \param _ui32Val Value whose lower bots are to be obtained.
	 * \return Returns all but the highest bit.
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::LoBits( const uint32_t & _ui32Val ) {
		return _ui32Val & 0x7FFFFFFF;
	}

	/**
	 * Mixes bits.
	 *
	 * \param _ui32L Left operand.
	 * \param _ui32R Right operand.
	 * \return Returns the mixed bits.
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::MixBits( const uint32_t & _ui32L, const uint32_t & _ui32R ) {
		return HiBit( _ui32L ) | LoBits( _ui32R );
	}

	/**
	 * "Mersenne Twist" up values.
	 *
	 * \param _ui32F A value.
	 * \param _ui32L A value.
	 * \param _ui32R A value.
	 * \return Returns the twisted value.
	 */
	LSE_INLINE uint32_t LSE_FCALL CRand::MersenneTwist( const uint32_t & _ui32F, const uint32_t & _ui32L, const uint32_t & _ui32R ) {
		return _ui32F ^ (MixBits( _ui32L, _ui32R ) >> 1) ^ (-LoBit( _ui32R ) & 0x9908B0DF);
	}

}	// namespace lsm


#if defined( LSE_VISUALSTUDIO )
#pragma warning( pop )
#endif	// #if defined( LSE_VISUALSTUDIO )

#endif	// __LSM_RAND_H__

/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A sampler class to assist with setting sampler states.
 */

#include "LSGSampler.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CSampler::CSampler() :
		m_pssSamplerState( NULL ),
		m_ui32Dirty( 0 ) {
		CGfx::DefaultSamplerDesc( m_sdCur );
	}
	LSE_CALLCTOR CSampler::~CSampler() {
		Reset();
	}

	// == Functions.
	/**
	 * Activates the sampler in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the sampler.  Must be less than LSG_MAX_SHADER_SAMPLERS.
	 * \return Returns true if the sampler was activated in the given slot.
	 */
	LSBOOL LSE_CALL CSampler::Activate( uint32_t _ui32Slot ) {
		if ( _ui32Slot >= LSG_MAX_SHADER_SAMPLERS ) { return false; }	// Sampler unit not available.
		if ( !Undirty() ) { return false; }
		CGfx::RenderState().pssSamplers[_ui32Slot] = m_pssSamplerState;
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL CSampler::Reset() {
		CGfx::ReleaseState( m_pssSamplerState );
		CGfx::DefaultSamplerDesc( m_sdCur );
		m_ui32Dirty = 0;
	}

	/**
	 * Undirties the sampler, creating a new graphics-API state if dirty.
	 *
	 * \return Returns true if the new state was successfully created.
	 */
	LSBOOL LSE_CALL CSampler::Undirty() {
		if ( m_ui32Dirty ) {
			m_ui32Dirty = 0;
			CGfx::ReleaseState( m_pssSamplerState );
			return SUCCEEDED( CGfx::CreateSamplerState( m_sdCur, &m_pssSamplerState ) );
		}
		return true;
	}

}	// namespace lsg

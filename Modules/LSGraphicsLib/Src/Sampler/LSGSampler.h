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


#ifndef __LSG_SAMPLER_H__
#define __LSG_SAMPLER_H__

#include "../LSGGraphicsLib.h"
#include "../Gfx/LSGGfx.h"

namespace lsg {

	/**
	 * Class CSampler
	 * \brief A sampler class to assist with setting sampler states.
	 *
	 * Description: A sampler class to assist with setting sampler states.
	 */
	class CSampler {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSampler();
		LSE_CALLCTOR							~CSampler();


		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _sOther Object to copy.
		 * \return Returns a reference to this object.
		 */
		LSE_INLINE CSampler & LSE_FCALL			operator = ( const CSampler &_sOther );



		// == Functions.
		/**
		 * Activates the sampler in the given slot.
		 *
		 * \param _ui32Slot Slot into which to activate the sampler.  Must be less than LSG_MAX_SHADER_SAMPLERS.
		 * \return Returns true if the sampler was activated in the given slot.
		 */
		LSBOOL LSE_CALL							Activate( uint32_t _ui32Slot );

		/**
		 * Resets everything to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Sets the filter.
		 *
		 * \param _fFilter The filter to set.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetFilter( LSG_FILTER _fFilter = LSG_F_MIN_MAG_MIP_LINEAR );

		/**
		 * Sets the address mode.
		 *
		 * \param _twAddressU The U address mode.
		 * \param _twAddressV The V address mode.
		 * \param _twAddressW The W address mode.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetAddressMode( LSG_TEXTURE_ADDRESS_MODE _twAddressU = LSG_TAM_WRAP, LSG_TEXTURE_ADDRESS_MODE _twAddressV = LSG_TAM_WRAP, LSG_TEXTURE_ADDRESS_MODE _twAddressW = LSG_TAM_WRAP );

		/**
		 * Sets the mipmap LOD bias.
		 *
		 * \param _fMipLODBias Offset from the calculated mipmap level.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetMipLodBias( LSREAL _fMipLODBias = LSM_ZERO );

		/**
		 * Sets the maximum anisotropy value.
		 *
		 * \param _ui32MaxAnisotropy Maximum anisotropic value, automatically clamped to be within
		 *	the valid range for the hardware.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetMaxAnisotropy( uint32_t _ui32MaxAnisotropy = 16 );

		/**
		 * Sets the comparison function.
		 *
		 * \param _cfComparisonFunc A function that compares sampled data against existing sampled data.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetComparisonFunc( LSG_COMPARISON_FUNC _cfComparisonFunc = LSG_CF_NEVER );

		/**
		 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
		 *
		 * \param _fR The red color (clamped between 0 and 1).
		 * \param _fG The green color (clamped between 0 and 1).
		 * \param _fB The blue color (clamped between 0 and 1).
		 * \param _fA The alpha color (clamped between 0 and 1).
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetBorderColor( LSREAL _fR = LSM_ONE, LSREAL _fG = LSM_ONE, LSREAL _fB = LSM_ONE, LSREAL _fA = LSM_ONE );

		/**
		 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
		 *
		 * \param _ui32R The red color (clamped between 0 and 255).
		 * \param _ui32G The green color (clamped between 0 and 255).
		 * \param _ui32B The blue color (clamped between 0 and 255).
		 * \param _ui32A The alpha color (clamped between 0 and 255).
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetBorderColor( uint32_t _ui32R = 255, uint32_t _ui32G = 255, uint32_t _ui32B = 255, uint32_t _ui32A = 255 );

		/**
		 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
		 *
		 * \param _vColor Color to set such that XYZW = RGBA.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetBorderColor( const CVector4 &_vColor );

		/**
		 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
		 *	Alpha is set to 1.0.
		 *
		 * \param _vColor Color to set such that XYZ = RGB.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetBorderColor( const CVector3 &_vColor );

		/**
		 * Sets the minimum and maximum mipmap LOD.
		 *
		 * \param _fMinLOD Lower end of the mipmap range to which to clamp access.
		 * \param _fMaxLOD Lower end of the mipmap range to which to clamp access.
		 * \return Returns a reference to this object so that this method can be
		 *	passed as an input to another method.
		 */
		LSE_INLINE CSampler & LSE_FCALL			SetMipLod( LSREAL _fMinLOD = LSM_ZERO, LSREAL _fMaxLOD = static_cast<LSREAL>(1000.0) );


	protected :
		// == Members.
		/** The graphics-API sampler state. */
		LSG_SAMPLER_STATE *						m_pssSamplerState;

		/** The current sampler settings. */
		LSG_SAMPLER_DESC						m_sdCur;

		/** Dirty flags. */
		uint32_t								m_ui32Dirty;


		// == Functions.
		/**
		 * Undirties the sampler, creating a new graphics-API state if dirty.
		 *
		 * \return Returns true if the new state was successfully created.
		 */
		LSBOOL LSE_CALL							Undirty();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * Assignment operator.
	 *
	 * \param _sOther Object to copy.
	 * \return Returns a reference to this object.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::operator = ( const CSampler &_sOther ) {
		Reset();
		m_sdCur = _sOther.m_sdCur;
		m_ui32Dirty = 1;

		return (*this);
	 }

	// == Functions.
	/**
	 * Sets the filter.
	 *
	 * \param _fFilter The filter to set.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetFilter( LSG_FILTER _fFilter ) {
		if ( m_sdCur.Filter != CStd::ImpCast( m_sdCur.Filter, _fFilter ) ) {
			m_ui32Dirty |= 1;
			m_sdCur.Filter = CStd::ImpCast( m_sdCur.Filter, _fFilter );
		}
		return (*this);
	}

	/**
	 * Sets the address mode.
	 *
	 * \param _twAddressU The U address mode.
	 * \param _twAddressV The V address mode.
	 * \param _twAddressW The W address mode.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetAddressMode( LSG_TEXTURE_ADDRESS_MODE _twAddressU, LSG_TEXTURE_ADDRESS_MODE _twAddressV, LSG_TEXTURE_ADDRESS_MODE _twAddressW ) {
		if ( m_sdCur.AddressU != CStd::ImpCast( m_sdCur.AddressU, _twAddressU ) ||
			m_sdCur.AddressV != CStd::ImpCast( m_sdCur.AddressV, _twAddressV ) ||
			m_sdCur.AddressW != CStd::ImpCast( m_sdCur.AddressW, _twAddressW ) ) {
			m_ui32Dirty |= 1;
			m_sdCur.AddressU = CStd::ImpCast( m_sdCur.AddressU, _twAddressU );
			m_sdCur.AddressV = CStd::ImpCast( m_sdCur.AddressV, _twAddressV );
			m_sdCur.AddressW = CStd::ImpCast( m_sdCur.AddressW, _twAddressW );
		}
		return (*this);
	}

	/**
	 * Sets the mipmap LOD bias.
	 *
	 * \param _fMipLODBias Offset from the calculated mipmap level.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetMipLodBias( LSREAL _fMipLODBias ) {
		if ( m_sdCur.MipLODBias != static_cast<LSGREAL>(_fMipLODBias) ) {
			m_ui32Dirty |= 1;
			m_sdCur.MipLODBias = static_cast<LSGREAL>(_fMipLODBias);
		}
		return (*this);
	}

	/**
	 * Sets the maximum anisotropy value.
	 *
	 * \param _ui32MaxAnisotropy Maximum anisotropic value, automatically clamped to be within
	 *	the valid range for the hardware.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetMaxAnisotropy( uint32_t _ui32MaxAnisotropy ) {
		_ui32MaxAnisotropy = CStd::Clamp<uint32_t>( _ui32MaxAnisotropy, 1, CGfx::GetMetrics().ui32MaxAniso );
		if ( m_sdCur.MaxAnisotropy != _ui32MaxAnisotropy ) {
			m_ui32Dirty |= 1;
			m_sdCur.MaxAnisotropy = _ui32MaxAnisotropy;
		}
		return (*this);
	}

	/**
	 * Sets the comparison function.
	 *
	 * \param _cfComparisonFunc A function that compares sampled data against existing sampled data.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetComparisonFunc( LSG_COMPARISON_FUNC _cfComparisonFunc ) {
		if ( m_sdCur.ComparisonFunc != CStd::ImpCast( m_sdCur.ComparisonFunc, _cfComparisonFunc ) ) {
			m_ui32Dirty |= 1;
			m_sdCur.ComparisonFunc = CStd::ImpCast( m_sdCur.ComparisonFunc, _cfComparisonFunc );
		}
		return (*this);
	}

	/**
	 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
	 *
	 * \param _fR The red color (clamped between 0 and 1).
	 * \param _fG The green color (clamped between 0 and 1).
	 * \param _fB The blue color (clamped between 0 and 1).
	 * \param _fA The alpha color (clamped between 0 and 1).
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetBorderColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA ) {
		LSGREAL fTemp[4] = {
			CStd::Clamp<LSGREAL>( _fR, 0.0f, 1.0f ),
			CStd::Clamp<LSGREAL>( _fG, 0.0f, 1.0f ),
			CStd::Clamp<LSGREAL>( _fB, 0.0f, 1.0f ),
			CStd::Clamp<LSGREAL>( _fA, 0.0f, 1.0f ),
		};
		if ( m_sdCur.BorderColor[0] != fTemp[0] ||
			m_sdCur.BorderColor[1] != fTemp[1] ||
			m_sdCur.BorderColor[2] != fTemp[2] ||
			m_sdCur.BorderColor[3] != fTemp[3] ) {
			m_ui32Dirty |= 1;
			m_sdCur.BorderColor[0] = fTemp[0];
			m_sdCur.BorderColor[1] = fTemp[1];
			m_sdCur.BorderColor[2] = fTemp[2];
			m_sdCur.BorderColor[3] = fTemp[3];
		}
		return (*this);
	}

	/**
	 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
	 *
	 * \param _ui32R The red color (clamped between 0 and 255).
	 * \param _ui32G The green color (clamped between 0 and 255).
	 * \param _ui32B The blue color (clamped between 0 and 255).
	 * \param _ui32A The alpha color (clamped between 0 and 255).
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetBorderColor( uint32_t _ui32R, uint32_t _ui32G, uint32_t _ui32B, uint32_t _ui32A ) {
		return SetBorderColor( _ui32R * static_cast<LSREAL>(1.0 / 255.0),
			_ui32G * static_cast<LSREAL>(1.0 / 255.0),
			_ui32B * static_cast<LSREAL>(1.0 / 255.0),
			_ui32A * static_cast<LSREAL>(1.0 / 255.0) );
	}

	/**
	 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
	 *
	 * \param _vColor Color to set such that XYZW = RGBA.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetBorderColor( const CVector4 &_vColor ) {
		return SetBorderColor( _vColor.x, _vColor.y, _vColor.z, _vColor.w );
	}

	/**
	 * Sets the border color of the sampler (used if LSG_TAM_BORDER is specified for any of the addressing modes).
	 *	Alpha is set to 1.0.
	 *
	 * \param _vColor Color to set such that XYZ = RGB.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetBorderColor( const CVector3 &_vColor ) {
		return SetBorderColor( _vColor.x, _vColor.y, _vColor.z, LSM_ONE );
	}

	/**
	 * Sets the minimum and maximum mipmap LOD.
	 *
	 * \param _fMinLOD Lower end of the mipmap range to which to clamp access.
	 * \param _fMaxLOD Lower end of the mipmap range to which to clamp access.
	 * \return Returns a reference to this object so that this method can be
	 *	passed as an input to another method.
	 */
	LSE_INLINE CSampler & LSE_FCALL CSampler::SetMipLod( LSREAL _fMinLOD, LSREAL _fMaxLOD ) {
		if ( m_sdCur.MinLOD != static_cast<LSGREAL>( _fMinLOD ) ||
			m_sdCur.MaxLOD != static_cast<LSGREAL>( _fMaxLOD ) ) {
			m_ui32Dirty |= 1;
			m_sdCur.MinLOD = static_cast<LSGREAL>( _fMinLOD );
			m_sdCur.MaxLOD = static_cast<LSGREAL>( _fMaxLOD );
		}
		return (*this);
	}

}	// namespace lsg

#endif	// __LSG_SAMPLER_H__

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
 * Description: A string name and a (usually) unique key that identifies said name.  Note that the key is not 100%
 *	necessarily guaranteed to be unique, but almost always will be.  Here, it is used to improve string-
 *	search performance, so it is not required to be unique.
 */


#ifndef __LSD_NAMEKEY_H__
#define __LSD_NAMEKEY_H__

#include "../LSDModelLib.h"
#include "Crc/LSSTDCrc.h"
#include "String/LSTLString.h"

namespace lsd {

	/**
	 * Class CNameKey
	 * \brief A string name and a (usually) unique key that identifies said name.
	 *
	 * Description: A string name and a (usually) unique key that identifies said name.  Note that the key is not 100%
	 *	necessarily guaranteed to be unique, but almost always will be.  Here, it is used to improve string-
	 *	search performance, so it is not required to be unique.
	 */
	class CNameKey {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CNameKey();
		LSE_CALLCTOR							CNameKey( const CString &_sName );


		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _pcName New name to assign.
		 * \return Returns a reference to this object.
		 */
		LSE_INLINE CNameKey & LSE_FCALL			operator = ( const char * _pcName );

		/**
		 * Assignment operator.
		 *
		 * \param _sName New name to assign.
		 * \return Returns a reference to this object.
		 */
		LSE_INLINE CNameKey & LSE_FCALL			operator = ( const CString &_sName );

		/**
		 * Less-than operator.
		 *
		 * \param _nkOther Object against which to compare.
		 * \return Returns true if this object compares less than the given object.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( const CNameKey &_nkOther ) const;

		/**
		 * Equality operator.
		 *
		 * \param _nkOther Object against which to compare.
		 * \return Returns true if this object equals the given object.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( const CNameKey &_nkOther ) const;


		// == Functions.
		/**
		 * Gets the name of the string.
		 *
		 * \return Returns a constant reference to the string.
		 */
		LSE_INLINE const CString & LSE_CALL		Name() const;


	protected :
		// == Members.
		/** Name. */
		CString									m_sName;

		/** Name converted to a quick-compare key. */
		uint64_t								m_ui64NameKey;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CNameKey::CNameKey() :
		m_ui64NameKey( 0ULL ) {
	}
	LSE_INLINE LSE_CALLCTOR CNameKey::CNameKey( const CString &_sName ) :
		m_sName( _sName ) {
		m_ui64NameKey = static_cast<uint64_t>(CCrc::GetCrc( reinterpret_cast<const uint8_t *>(m_sName.CStr()), m_sName.Length() )) |
			(static_cast<uint64_t>(CCrc::GetAltCrc( reinterpret_cast<const uint8_t *>(m_sName.CStr()), m_sName.Length() )) << 32ULL);
	}

	// == Operators.
	/**
	 * Assignment operator.
	 *
	 * \param _pcName New name to assign.
	 * \return Returns a reference to this object.
	 */
	LSE_INLINE CNameKey & LSE_FCALL CNameKey::operator = ( const char * _pcName ) {
		m_sName = _pcName;
		m_ui64NameKey = static_cast<uint64_t>(CCrc::GetCrc( reinterpret_cast<const uint8_t *>(m_sName.CStr()), m_sName.Length() )) |
			(static_cast<uint64_t>(CCrc::GetAltCrc( reinterpret_cast<const uint8_t *>(m_sName.CStr()), m_sName.Length() )) << 32ULL);
		return (*this);
	}

	/**
	 * Assignment operator.
	 *
	 * \param _sName New name to assign.
	 * \return Returns a reference to this object.
	 */
	LSE_INLINE CNameKey & LSE_FCALL CNameKey::operator = ( const CString &_sName ) {
		m_sName = _sName;
		m_ui64NameKey = static_cast<uint64_t>(CCrc::GetCrc( reinterpret_cast<const uint8_t *>(m_sName.CStr()), m_sName.Length() )) |
			(static_cast<uint64_t>(CCrc::GetAltCrc( reinterpret_cast<const uint8_t *>(m_sName.CStr()), m_sName.Length() )) << 32ULL);
		return (*this);
	}

	/**
	 * Less-than operator.
	 *
	 * \param _nkOther Object against which to compare.
	 * \return Returns true if this object compares less than the given object.
	 */
	LSE_INLINE bool LSE_FCALL CNameKey::operator < ( const CNameKey &_nkOther ) const {
		return m_ui64NameKey < _nkOther.m_ui64NameKey ||
			m_sName < _nkOther.m_sName;
	}

	/**
	 * Equality operator.
	 *
	 * \param _nkOther Object against which to compare.
	 * \return Returns true if this object equals the given object.
	 */
	LSE_INLINE bool LSE_FCALL CNameKey::operator == ( const CNameKey &_nkOther ) const {
		return m_ui64NameKey == _nkOther.m_ui64NameKey &&
			m_sName == _nkOther.m_sName;
	}

	// == Functions.
	/**
	 * Gets the name of the string.
	 *
	 * \return Returns a constant reference to the string.
	 */
	LSE_INLINE const CString & LSE_CALL CNameKey::Name() const {
		return m_sName;
	}

}	// namespace lsd

#endif	// __LSD_NAMEKEY_H__

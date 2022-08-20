/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS(C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: toplevel libogg include
 last mod: $Id: ogg.h 17571 2010-10-27 13:28:20Z xiphmont $

 ********************************************************************/

#include "LSSOgg.h"


#define LSS_BUFFER_INCREMENT 256


namespace lss {

	// == Members.
	/**
	 * Undocumented data from the Ogg library.
	 */
	uint32_t COgg::m_ui32Mask[] = {
		0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000F, 
		0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF, 0x000001FF, 
		0x000003FF, 0x000007FF, 0x00000FFF, 0x00001FFF, 0x00003FFF, 
		0x00007FFF, 0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 
		0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF, 
		0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF, 0x1FFFFFFF, 
		0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
	};

	/**
	 * Undocumented data from the Ogg library.
	 */
	uint32_t COgg::m_ui32Mask8B[] = {
		0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF
	};

	/**
	 * Undocumented data from the Ogg library.
	 */
	uint32_t COgg::crc_lookup[256] = {
		0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 
		0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005, 
		0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 
		0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD, 
		0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 
		0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75, 
		0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 
		0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD, 
		0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 
		0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 
		0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 
		0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D, 
		0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 
		0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95, 
		0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 
		0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 
		0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 
		0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072, 
		0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 
		0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 
		0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 
		0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02, 
		0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 
		0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA, 
		0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 
		0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692, 
		0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 
		0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A, 
		0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 
		0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 
		0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 
		0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A, 
		0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 
		0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB, 
		0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 
		0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53, 
		0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 
		0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B, 
		0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 
		0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 
		0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 
		0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B, 
		0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 
		0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3, 
		0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 
		0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 
		0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 
		0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3, 
		0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 
		0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 
		0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 
		0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24, 
		0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 
		0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC, 
		0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 
		0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 
		0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 
		0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C, 
		0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 
		0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 
		0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 
		0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C, 
		0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 
		0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
	};

	// == Functions.
	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_writeinit( oggpack_buffer * _ppbBuffer ) {
		CStd::MemSet( _ppbBuffer, 0, sizeof( (*_ppbBuffer) ) );
		_ppbBuffer->ptr = _ppbBuffer->buffer = static_cast<uint8_t *>(CMemLib::MAlloc( LSS_BUFFER_INCREMENT ));
		_ppbBuffer->buffer[0] = '\0';
		_ppbBuffer->storage = LSS_BUFFER_INCREMENT;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_writecheck( oggpack_buffer * _ppbBuffer ) {
		if ( !_ppbBuffer->ptr || !_ppbBuffer->storage ) { return -1; }
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_writetrunc( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		int32_t _i32Bytes = _i32Bits >> 3;
		if ( _ppbBuffer->ptr ) {
			_i32Bits -= _i32Bytes*8;
			_ppbBuffer->ptr = _ppbBuffer->buffer + _i32Bytes;
			_ppbBuffer->endbit = _i32Bits;
			_ppbBuffer->endbyte = _i32Bytes;
			*_ppbBuffer->ptr &= m_ui32Mask[_i32Bits];
		}
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_writealign( oggpack_buffer * _ppbBuffer ) {
		int32_t _i32Bits = 8 - _ppbBuffer->endbit;
		if ( _i32Bits < 8 ) { oggpack_write( _ppbBuffer, 0, _i32Bits ); }
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_writecopy( oggpack_buffer * _ppbBuffer, void * _pvSource, int32_t _i32Bits ) {
		oggpack_writecopy_helper( _ppbBuffer, _pvSource, _i32Bits, oggpack_write, 0 );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_reset( oggpack_buffer * _ppbBuffer ) {
		if ( !_ppbBuffer->ptr ) { return; }
		_ppbBuffer->ptr = _ppbBuffer->buffer;
		_ppbBuffer->buffer[0] = 0;
		_ppbBuffer->endbit = _ppbBuffer->endbyte = 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_writeclear( oggpack_buffer * _ppbBuffer ) {
		if ( _ppbBuffer->buffer ) { CMemLib::Free( _ppbBuffer->buffer ); }
		CStd::MemSet( _ppbBuffer, 0, sizeof( (*_ppbBuffer) ) );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _pui8Buf Undocumented parameter from the Ogg library.
	 * \param _i32Bytes Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_readinit( oggpack_buffer * _ppbBuffer, uint8_t * _pui8Buf, int32_t _i32Bytes ) {
		CStd::MemSet( _ppbBuffer, 0, sizeof( (*_ppbBuffer) ) );
		_ppbBuffer->buffer = _ppbBuffer->ptr = _pui8Buf;
		_ppbBuffer->storage = _i32Bytes;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _ui32Value Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_write( oggpack_buffer * _ppbBuffer, uint32_t _ui32Value, int32_t _i32Bits ) {
		if ( _i32Bits < 0 || _i32Bits > 32 ) { goto Err; }
		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage - 4 ) {
			void * pvRet;
			if ( !_ppbBuffer->ptr ) { return; }
			if ( _ppbBuffer->storage > LSSTD_MAX_INT32 - LSS_BUFFER_INCREMENT ) { goto Err; }
			pvRet = CMemLib::MReAlloc( _ppbBuffer->buffer, _ppbBuffer->storage + LSS_BUFFER_INCREMENT );
			if ( !pvRet ) { goto Err; }
			_ppbBuffer->buffer = static_cast<uint8_t *>(pvRet);
			_ppbBuffer->storage += LSS_BUFFER_INCREMENT;
			_ppbBuffer->ptr = _ppbBuffer->buffer + _ppbBuffer->endbyte;
		}

		_ui32Value &= m_ui32Mask[_i32Bits]; 
		_i32Bits += _ppbBuffer->endbit;

		_ppbBuffer->ptr[0] |= _ui32Value << _ppbBuffer->endbit;	
		
		if ( _i32Bits >= 8 ) {
			_ppbBuffer->ptr[1] = static_cast<uint8_t>(_ui32Value >> (8-_ppbBuffer->endbit ));
			if ( _i32Bits >= 16 ) {
				_ppbBuffer->ptr[2] = static_cast<uint8_t>(_ui32Value >> (16-_ppbBuffer->endbit ));
				if ( _i32Bits >= 24 ) {
					_ppbBuffer->ptr[3] = static_cast<uint8_t>(_ui32Value >> (24-_ppbBuffer->endbit ));
					if ( _i32Bits >= 32 ) {
						if ( _ppbBuffer->endbit ) {
							_ppbBuffer->ptr[4] = static_cast<uint8_t>(_ui32Value >> (32-_ppbBuffer->endbit ));
						}
						else {
							_ppbBuffer->ptr[4] = 0;
						}
					}
				}
			}
		}

		_ppbBuffer->endbyte += _i32Bits / 8;
		_ppbBuffer->ptr += _i32Bits / 8;
		_ppbBuffer->endbit = _i32Bits & 7;
		return;
	Err :
		oggpack_writeclear( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_look( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		uint32_t pvRet;
		uint32_t ui32M;

		if ( _i32Bits<0 || _i32Bits > 32 ) { return -1; }
		ui32M = m_ui32Mask[_i32Bits];
		_i32Bits += _ppbBuffer->endbit;

		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage-4 ) {
			// Not the main path.
			if ( _ppbBuffer->endbyte > _ppbBuffer->storage - ((_i32Bits + 7) >> 3) ) { return -1; }
			// Special case to avoid reading _ppbBuffer->ptr[0], which might be past the end of
			//	the buffer; also skips some useless accounting.
			else if ( !_i32Bits ) { return 0L; }
		}

		pvRet = _ppbBuffer->ptr[0] >> _ppbBuffer->endbit;
		if ( _i32Bits > 8 ) {
			pvRet |= _ppbBuffer->ptr[1] << (8 - _ppbBuffer->endbit);
			if ( _i32Bits > 16 ) {
				pvRet |= _ppbBuffer->ptr[2] << (16 - _ppbBuffer->endbit);
				if ( _i32Bits > 24 ) {
					pvRet |= _ppbBuffer->ptr[3] << (24 - _ppbBuffer->endbit);
					if ( _i32Bits > 32 && _ppbBuffer->endbit ) {
						pvRet |= _ppbBuffer->ptr[4] << (32 - _ppbBuffer->endbit);
					}
				}
			}
		}
		return ui32M & pvRet;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_look1( oggpack_buffer * _ppbBuffer ) {
		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage ) { return -1; }
		return (_ppbBuffer->ptr[0] >> _ppbBuffer->endbit) & 1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_adv( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		_i32Bits += _ppbBuffer->endbit;

		if ( _ppbBuffer->endbyte > _ppbBuffer->storage - ((_i32Bits + 7) >> 3) ) { goto OverFlow; }

		_ppbBuffer->ptr += _i32Bits / 8;
		_ppbBuffer->endbyte += _i32Bits / 8;
		_ppbBuffer->endbit = _i32Bits & 7;
		return;

	OverFlow :
		_ppbBuffer->ptr = NULL;
		_ppbBuffer->endbyte = _ppbBuffer->storage;
		_ppbBuffer->endbit = 1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_adv1( oggpack_buffer * _ppbBuffer ) {
		if ( ++(_ppbBuffer->endbit) > 7 ) {
			_ppbBuffer->endbit = 0;
			_ppbBuffer->ptr++;
			_ppbBuffer->endbyte++;
		}
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_read( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		int32_t pvRet;
		uint32_t ui32M;

		if ( _i32Bits<0 || _i32Bits > 32 ) { goto Err; }
		ui32M = m_ui32Mask[_i32Bits];
		_i32Bits += _ppbBuffer->endbit;

		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage-4 ) {
			// not the main path.
			if ( _ppbBuffer->endbyte > _ppbBuffer->storage - ((_i32Bits + 7) >> 3) ) { goto OverFlow; }
			// Special case to avoid reading _ppbBuffer->ptr[0], which might be past the end of
			//	the buffer; also skips some useless accounting.
			else if ( !_i32Bits ) { return 0L; }
		}

		pvRet = _ppbBuffer->ptr[0] >> _ppbBuffer->endbit;
		if ( _i32Bits > 8 ) {
			pvRet |= _ppbBuffer->ptr[1] << (8 - _ppbBuffer->endbit);	
			if ( _i32Bits > 16 ) {
				pvRet |= _ppbBuffer->ptr[2] << (16 - _ppbBuffer->endbit);	
				if ( _i32Bits > 24 ) {
					pvRet |= _ppbBuffer->ptr[3] << (24 - _ppbBuffer->endbit);	
					if ( _i32Bits > 32 && _ppbBuffer->endbit ) {
						pvRet |= _ppbBuffer->ptr[4] << (32 - _ppbBuffer->endbit);
					}
				}
			}
		}
		pvRet &= ui32M;
		_ppbBuffer->ptr += _i32Bits / 8;
		_ppbBuffer->endbyte += _i32Bits / 8;
		_ppbBuffer->endbit = _i32Bits & 7;
		return pvRet;

	OverFlow :
	Err :
		_ppbBuffer->ptr = NULL;
		_ppbBuffer->endbyte = _ppbBuffer->storage;
		_ppbBuffer->endbit = 1;
		return -1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_read1( oggpack_buffer * _ppbBuffer ) {
		int32_t pvRet;

		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage ) { goto OverFlow; }
		pvRet = (_ppbBuffer->ptr[0] >> _ppbBuffer->endbit) & 1;

		_ppbBuffer->endbit++;
		if ( _ppbBuffer->endbit > 7 ) {
			_ppbBuffer->endbit = 0;
			_ppbBuffer->ptr++;
			_ppbBuffer->endbyte++;
		}
		return pvRet;

	OverFlow:
		_ppbBuffer->ptr = NULL;
		_ppbBuffer->endbyte = _ppbBuffer->storage;
		_ppbBuffer->endbit = 1;
		return -1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_bytes( oggpack_buffer * _ppbBuffer ) {
		return (_ppbBuffer->endbyte + (_ppbBuffer->endbit + 7) / 8);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpack_bits( oggpack_buffer * _ppbBuffer ) {
		return (_ppbBuffer->endbyte * 8 + _ppbBuffer->endbit);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	uint8_t * COgg::oggpack_get_buffer( oggpack_buffer * _ppbBuffer ) {
		return (_ppbBuffer->buffer);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_writeinit( oggpack_buffer * _ppbBuffer ) {
		oggpack_writeinit( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_writecheck( oggpack_buffer * _ppbBuffer ) {
		return oggpack_writecheck( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_writetrunc( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		int32_t _i32Bytes = _i32Bits >> 3;
		if ( _ppbBuffer->ptr ) {
			_i32Bits -= _i32Bytes*8;
			_ppbBuffer->ptr = _ppbBuffer->buffer+_i32Bytes;
			_ppbBuffer->endbit = _i32Bits;
			_ppbBuffer->endbyte = _i32Bytes;
			(*_ppbBuffer->ptr) &= m_ui32Mask8B[_i32Bits];
		}
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_writealign( oggpack_buffer * _ppbBuffer ) {
		int32_t _i32Bits = 8 - _ppbBuffer->endbit;
		if ( _i32Bits < 8 ) {
			oggpackB_write( _ppbBuffer, 0, _i32Bits );
		}
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _pvSource Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_writecopy( oggpack_buffer * _ppbBuffer, void * _pvSource, int32_t _i32Bits ) {
		oggpack_writecopy_helper( _ppbBuffer, _pvSource, _i32Bits, oggpackB_write, 1 );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_reset( oggpack_buffer * _ppbBuffer ) {
		oggpack_reset( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_writeclear( oggpack_buffer * _ppbBuffer ) {
		oggpack_writeclear( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _pui8Buf Undocumented parameter from the Ogg library.
	 * \param _i32Bytes Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_readinit( oggpack_buffer * _ppbBuffer, uint8_t * _pui8Buf, int32_t _i32Bytes ) {
		oggpack_readinit( _ppbBuffer, _pui8Buf, _i32Bytes );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _ui32Value Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_write( oggpack_buffer * _ppbBuffer, uint32_t _ui32Value, int32_t _i32Bits ) {
		if ( _i32Bits < 0 || _i32Bits > 32 ) { goto Err; }
		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage - 4 ) {
			void * pvRet;
			if ( !_ppbBuffer->ptr ) { return; }
			if ( _ppbBuffer->storage > LSSTD_MAX_INT32 - LSS_BUFFER_INCREMENT ) { goto Err; }
			pvRet = CMemLib::MReAlloc( _ppbBuffer->buffer, _ppbBuffer->storage+LSS_BUFFER_INCREMENT );
			if ( !pvRet ) { goto Err; }
			_ppbBuffer->buffer = static_cast<uint8_t *>(pvRet);
			_ppbBuffer->storage += LSS_BUFFER_INCREMENT;
			_ppbBuffer->ptr = _ppbBuffer->buffer + _ppbBuffer->endbyte;
		}

		_ui32Value = (_ui32Value&m_ui32Mask[_i32Bits]) << (32 - _i32Bits);
		_i32Bits += _ppbBuffer->endbit;

		_ppbBuffer->ptr[0] |= _ui32Value >> (24 + _ppbBuffer->endbit);
		
		if ( _i32Bits >= 8 ) {
			_ppbBuffer->ptr[1] = static_cast<uint8_t>(_ui32Value >> (16 + _ppbBuffer->endbit));
			if ( _i32Bits >= 16 ) {
				_ppbBuffer->ptr[2] = static_cast<uint8_t>(_ui32Value >> (8 + _ppbBuffer->endbit));
				if ( _i32Bits >= 24 ) {
					_ppbBuffer->ptr[3] = static_cast<uint8_t>(_ui32Value >> (_ppbBuffer->endbit));
					if ( _i32Bits >= 32 ) {
						if ( _ppbBuffer->endbit ) {
							_ppbBuffer->ptr[4] = static_cast<uint8_t>(_ui32Value << (8 - _ppbBuffer->endbit));
						}
						else {
							_ppbBuffer->ptr[4] = 0;
						}
					}
				}
			}
		}

		_ppbBuffer->endbyte += _i32Bits / 8;
		_ppbBuffer->ptr += _i32Bits / 8;
		_ppbBuffer->endbit = _i32Bits & 7;
		return;
	Err :
		oggpack_writeclear( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_look( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		uint32_t pvRet;
		int32_t i32M = 32-_i32Bits;

		if ( i32M < 0 || i32M > 32 ) { return -1; }
		_i32Bits += _ppbBuffer->endbit;

		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage - 4 ) {
			// Not the main path.
			if ( _ppbBuffer->endbyte > _ppbBuffer->storage - ((_i32Bits + 7) >> 3) ) { return -1; }
			// Special case to avoid reading _ppbBuffer->ptr[0], which might be past the end of
			//	the buffer; also skips some useless accounting.
			else if ( !_i32Bits ) { return 0L; }
		}

		pvRet = _ppbBuffer->ptr[0] << (24 + _ppbBuffer->endbit);
		if ( _i32Bits > 8 ) {
			pvRet |= _ppbBuffer->ptr[1] << (16 + _ppbBuffer->endbit);	
			if ( _i32Bits > 16 ) {
				pvRet |= _ppbBuffer->ptr[2] << (8 + _ppbBuffer->endbit);	
				if ( _i32Bits > 24 ) {
					pvRet |= _ppbBuffer->ptr[3] << (_ppbBuffer->endbit);	
					if ( _i32Bits > 32 && _ppbBuffer->endbit ) {
						pvRet |= _ppbBuffer->ptr[4] >> (8 - _ppbBuffer->endbit);
					}
				}
			}
		}
		return ((pvRet & 0xFFFFFFFF) >> (i32M >> 1)) >> ((i32M + 1) >> 1);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_look1( oggpack_buffer * _ppbBuffer ) {
		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage ) { return -1; }
		return (_ppbBuffer->ptr[0] >> (7 - _ppbBuffer->endbit)) & 1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_adv( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		oggpack_adv( _ppbBuffer, _i32Bits );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpackB_adv1( oggpack_buffer * _ppbBuffer ) {
		oggpack_adv1( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_read( oggpack_buffer * _ppbBuffer, int32_t _i32Bits ) {
		int32_t pvRet;
		int32_t i32M = 32 - _i32Bits;

		if ( i32M < 0 || i32M > 32 ) { goto Err; }
		_i32Bits += _ppbBuffer->endbit;

		if ( _ppbBuffer->endbyte + 4 >= _ppbBuffer->storage ) {
			// Not the main path.
			if ( _ppbBuffer->endbyte > _ppbBuffer->storage - ((_i32Bits + 7) >> 3) ) { goto OverFlow; }
			// Special case to avoid reading _ppbBuffer->ptr[0], which might be past the end of
			//	the buffer; also skips some useless accounting.
			else if ( !_i32Bits ) { return 0L; }
		}

		pvRet = _ppbBuffer->ptr[0] << (24 + _ppbBuffer->endbit);
		if ( _i32Bits > 8 ) {
			pvRet |= _ppbBuffer->ptr[1] << (16 + _ppbBuffer->endbit);	
			if ( _i32Bits > 16 ) {
				pvRet |= _ppbBuffer->ptr[2] << (8 + _ppbBuffer->endbit);	
				if ( _i32Bits > 24 ) {
					pvRet |= _ppbBuffer->ptr[3] << (_ppbBuffer->endbit);	
					if ( _i32Bits > 32 && _ppbBuffer->endbit ) {
						pvRet |= _ppbBuffer->ptr[4] >> (8 - _ppbBuffer->endbit);
					}
				}
			}
		}
		pvRet = ((pvRet & 0xFFFFFFFFUL) >> (i32M >> 1)) >> ((i32M + 1) >> 1);

		_ppbBuffer->ptr += _i32Bits / 8;
		_ppbBuffer->endbyte += _i32Bits / 8;
		_ppbBuffer->endbit = _i32Bits & 7;
		return pvRet;

	OverFlow :
	Err :
		_ppbBuffer->ptr = NULL;
		_ppbBuffer->endbyte = _ppbBuffer->storage;
		_ppbBuffer->endbit = 1;
		return -1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_read1( oggpack_buffer * _ppbBuffer ) {
		int32_t pvRet;

		if ( _ppbBuffer->endbyte >= _ppbBuffer->storage ) { goto OverFlow; }
		pvRet = (_ppbBuffer->ptr[0] >> (7 - _ppbBuffer->endbit)) & 1;

		_ppbBuffer->endbit++;
		if ( _ppbBuffer->endbit > 7 ) {
			_ppbBuffer->endbit = 0;
			_ppbBuffer->ptr++;
			_ppbBuffer->endbyte++;
		}
		return pvRet;

	OverFlow :
		_ppbBuffer->ptr = NULL;
		_ppbBuffer->endbyte = _ppbBuffer->storage;
		_ppbBuffer->endbit = 1;
		return -1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_bytes( oggpack_buffer * _ppbBuffer ) {
		return oggpack_bytes( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::oggpackB_bits( oggpack_buffer * _ppbBuffer ) {
		return oggpack_bits( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	uint8_t * LSE_CALL COgg::oggpackB_get_buffer( oggpack_buffer * _ppbBuffer ) {
		return oggpack_get_buffer( _ppbBuffer );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _possOs Undocumented parameter from the Ogg library.
	 * \param _popOp Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_packetin( ogg_stream_state * _possOs, ogg_packet * _popOp ) {
		ogg_iovec_t oiIov;
		oiIov.iov_base = _popOp->packet;
		oiIov.iov_len = _popOp->bytes;
		return ogg_stream_iovecin( _possOs, &oiIov, 1, _popOp->e_o_s, _popOp->granulepos );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _possOs Undocumented parameter from the Ogg library.
	 * \param _poiIov Undocumented parameter from the Ogg library.
	 * \param _i32Count Undocumented parameter from the Ogg library.
	 * \param _i32Eos Undocumented parameter from the Ogg library.
	 * \param _i64GranulePos Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_iovecin( ogg_stream_state * _possOs, ogg_iovec_t * _poiIov, 
		int32_t _i32Count, int32_t _i32Eos, int64_t _i64GranulePos ) {
		int32_t i32Bytes = 0, i32LacingVals, I;

		if ( ogg_stream_check( _possOs ) )  { return -1; }
		if ( !_poiIov ) { return 0; }
	 
		for ( I = 0; I < _i32Count; ++I ) { i32Bytes += static_cast<int32_t>(_poiIov[I].iov_len); }
		i32LacingVals = i32Bytes / 255 + 1;

		if ( _possOs->body_returned ) {
			// Advance packet data according to the body_returned pointer.  We
			//	had to keep it around to return a pointer into the buffer last
			//	call.
			
			_possOs->body_fill -= _possOs->body_returned;
			if ( _possOs->body_fill ) {
				CStd::MemMove( _possOs->body_data, _possOs->body_data+_possOs->body_returned,
					_possOs->body_fill );
			}
			_possOs->body_returned = 0;
		}
	 
		// Make sure we have the buffer storage.
		if ( _os_body_expand( _possOs, i32Bytes ) || _os_lacing_expand( _possOs, i32LacingVals ) ) {
			return -1;
		}

		// Copy in the submitted packet.  Yes, the copy is a waste; this is
		//	the liability of overly clean abstraction for the time being.  It
		//	will actually be fairly easy to eliminate the extra copy in the
		//	future.

		for ( I = 0; I < _i32Count; ++I ) {
			CStd::MemCpy( _possOs->body_data + _possOs->body_fill, _poiIov[I].iov_base, _poiIov[I].iov_len );
			_possOs->body_fill += static_cast<int32_t>(_poiIov[I].iov_len);
		}

		// Store lacing vals for this packet.
		for ( I=0; I < i32LacingVals - 1; I++ ) {
			_possOs->lacing_vals[_possOs->lacing_fill+I] = 255;
			_possOs->granule_vals[_possOs->lacing_fill+I] = _possOs->granulepos;
		}
		_possOs->lacing_vals[_possOs->lacing_fill+I] = i32Bytes%255;
		_possOs->granulepos=_possOs->granule_vals[_possOs->lacing_fill+I] = _i64GranulePos;

		// Flag the first segment as the beginning of the packet.
		_possOs->lacing_vals[_possOs->lacing_fill] |= 0x100;

		_possOs->lacing_fill += i32LacingVals;

		// For the sake of completeness.
		_possOs->packetno++;

		if ( _i32Eos ) { _possOs->e_o_s = 1; }

		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _possOs Undocumented parameter from the Ogg library.
	 * \param _popOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_pageout( ogg_stream_state * _possOs, ogg_page * _popOg ) {
		int32_t i32Force = 0;
		if ( ogg_stream_check( _possOs ) ) { return 0; }

		if ( ( _possOs->e_o_s && _possOs->lacing_fill ) ||
			( _possOs->lacing_fill && !_possOs->b_o_s ) ) {
			i32Force = 1;
		}

		return ogg_stream_flush_i( _possOs, _popOg, i32Force, 4096 );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _possOs Undocumented parameter from the Ogg library.
	 * \param _popOg Undocumented parameter from the Ogg library.
	 * \param _i32NFill Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_pageout_fill( ogg_stream_state * _possOs, ogg_page * _popOg, int32_t _i32NFill ) {
		int32_t i32Force = 0;
		if ( ogg_stream_check( _possOs ) ) { return 0; }

		if ( ( _possOs->e_o_s && _possOs->lacing_fill ) || ( _possOs->lacing_fill && !_possOs->b_o_s ) ) {
			i32Force = 1;
		}

		return ogg_stream_flush_i( _possOs, _popOg, i32Force, _i32NFill );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _possOs Undocumented parameter from the Ogg library.
	 * \param _popOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_flush( ogg_stream_state * _possOs, ogg_page * _popOg ) {
		return ogg_stream_flush_i( _possOs, _popOg, 1, 4096 );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_init( ogg_sync_state * _pssOy ) {
		if ( _pssOy ) {
			_pssOy->storage = -1; // Used as a readiness flag.
			CStd::MemSet( _pssOy, 0, sizeof( (*_pssOy) ) );
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_clear( ogg_sync_state * _pssOy ) {
		if ( _pssOy ) {
			if ( _pssOy->data ) { CMemLib::Free( _pssOy->data ); }
			CStd::MemSet( _pssOy, 0, sizeof( (*_pssOy) ) );
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_reset( ogg_sync_state * _pssOy ) {
		if ( ogg_sync_check( _pssOy ) ) { return -1; }

		_pssOy->fill = 0;
		_pssOy->returned = 0;
		_pssOy->unsynced = 0;
		_pssOy->headerbytes = 0;
		_pssOy->bodybytes = 0;
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_destroy( ogg_sync_state * _pssOy ) {
		if ( _pssOy ) {
			ogg_sync_clear( _pssOy );
			CMemLib::Free( _pssOy );
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_check( ogg_sync_state * _pssOy ) {
		if ( _pssOy->storage < 0 ) { return -1; }
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \param _i32Size Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int8_t * LSE_CALL COgg::ogg_sync_buffer( ogg_sync_state * _pssOy,  int32_t _i32Size ) {
		if ( ogg_sync_check( _pssOy ) ) { return NULL; }

		// First, clear out any space that has been previously returned.
		if ( _pssOy->returned ) {
			_pssOy->fill -= _pssOy->returned;
			if ( _pssOy->fill>0 ) {
				CStd::MemMove( _pssOy->data, _pssOy->data+_pssOy->returned, _pssOy->fill );
			}
			_pssOy->returned = 0;
		}

		if ( _i32Size>_pssOy->storage - _pssOy->fill ) {
			// We need to extend the internal buffer.
			int32_t i32NewSize = _i32Size + _pssOy->fill + 4096; // An extra page to be nice.
			void *pvRet;

			if ( _pssOy->data ) {
				pvRet = CMemLib::MReAlloc( _pssOy->data, i32NewSize );
			}
			else {
				pvRet = CMemLib::MAlloc( i32NewSize );
			}
			if ( !pvRet ) {
				ogg_sync_clear( _pssOy );
				return NULL;
			}
			_pssOy->data = static_cast<uint8_t *>(pvRet);
			_pssOy->storage = i32NewSize;
		}

		// Expose a segment at least as large as requested at the fill mark.
		return reinterpret_cast<int8_t *>(_pssOy->data + _pssOy->fill);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \param _i32Bytes Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_wrote( ogg_sync_state * _pssOy,  int32_t _i32Bytes ) {
		if ( ogg_sync_check( _pssOy ) ) { return -1; }
		if ( _pssOy->fill + _i32Bytes > _pssOy->storage ) { return -1; }
		_pssOy->fill += _i32Bytes;
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_pageseek( ogg_sync_state * _pssOy, ogg_page * _ppOg ) {
		uint8_t * pui8Page = _pssOy->data + _pssOy->returned;
		uint8_t * pui8Next;
		int32_t _i32Bytes = _pssOy->fill - _pssOy->returned;

		if ( ogg_sync_check( _pssOy ) ) { return 0; }
		
		if ( _pssOy->headerbytes == 0 ) {
			int32_t i32HeaderBytes, I;
			if ( _i32Bytes < 27 ) { return 0; }					// Not enough for a header.
			
			// Verify capture pattern.
			if ( !CStd::MemCmpF( pui8Page, "OggS", 4 ) ) { goto SyncFail; }
			
			i32HeaderBytes = pui8Page[26] + 27;
			if ( _i32Bytes < i32HeaderBytes ) { return 0; }		// Not enough for header + seg table.
			
			// Count up body length in the segment table.
			
			for ( I = 0; I < pui8Page[26]; I++ ) {
				_pssOy->bodybytes += pui8Page[27+I];
			}
			_pssOy->headerbytes = i32HeaderBytes;
		}
		
		if ( _pssOy->bodybytes + _pssOy->headerbytes > _i32Bytes ) { return 0; }
		
		// The whole test page is buffered.	 Verify the checksum.
		{
			// Grab the checksum _i32Bytes, set the header field to zero.
			int8_t i8CheckSum[4];
			ogg_page pLog;
			
			CStd::MemCpy( i8CheckSum, pui8Page + 22, 4 );
			CStd::MemSet( pui8Page + 22, 0, 4 );
			
			// Set up a temp pui8Page struct and recompute the checksum.
			pLog.header = pui8Page;
			pLog.header_len = _pssOy->headerbytes;
			pLog.body = pui8Page + _pssOy->headerbytes;
			pLog.body_len = _pssOy->bodybytes;
			ogg_page_checksum_set( &pLog );
			
			// Compare.
			if ( !CStd::MemCmpF( i8CheckSum, pui8Page+22, 4 ) ) {
				// D'oh.  Mismatch!  Corrupt pui8Page( or miscapture and not a pui8Page at all ).
				//	Replace the computed checksum with the one actually read in.
				CStd::MemCpy( pui8Page+22, i8CheckSum, 4 );
				
				// Bad checksum.  Lose sync.
				goto SyncFail;
			}
		}
		
		// Yes, have a whole pui8Page all ready to go.
		{
			uint8_t * pui8ThisPage = _pssOy->data + _pssOy->returned;
			int32_t i32Bytes;

			if ( _ppOg ) {
				_ppOg->header = pui8ThisPage;
				_ppOg->header_len = _pssOy->headerbytes;
				_ppOg->body = pui8ThisPage + _pssOy->headerbytes;
				_ppOg->body_len = _pssOy->bodybytes;
			}

			_pssOy->unsynced = 0;
			_pssOy->returned += (i32Bytes = _pssOy->headerbytes + _pssOy->bodybytes);
			_pssOy->headerbytes = 0;
			_pssOy->bodybytes = 0;
			return i32Bytes;
		}
		
	SyncFail :
		
		_pssOy->headerbytes = 0;
		_pssOy->bodybytes = 0;
		
		// Search for possible capture.
		pui8Next = static_cast<uint8_t *>(CStd::MemChr( pui8Page + 1, 'O', _i32Bytes - 1 ));
		if ( !pui8Next ) {
			pui8Next = _pssOy->data + _pssOy->fill;
		}

		_pssOy->returned = static_cast<int32_t>(pui8Next - _pssOy->data);
		return static_cast<int32_t>(-(pui8Next - pui8Page));
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOy Undocumented parameter from the Ogg library.
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_sync_pageout( ogg_sync_state * _pssOy,  ogg_page * _ppOg ) {
		if ( ogg_sync_check( _pssOy ) ) { return 0; }

		// All we need to do is verify a page at the head of the stream
		//	buffer.  If it doesn't verify, we look for the next potential
		//	frame.

		for ( ;; ) {
			int32_t pvRet = ogg_sync_pageseek( _pssOy, _ppOg );
			if ( pvRet > 0 ) {
				// Have a page.
				return 1;
			}
			if ( pvRet == 0 ) {
				// Need more data.
				return 0;
			}
			
			// Head did not start a synced page.  Skipped some _i32Bytes.
			if ( !_pssOy->unsynced ) {
				_pssOy->unsynced = 1;
				return -1;
			}

			// Loop.  Keep looking.
		}
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_pagein( ogg_stream_state * _pssOs,  ogg_page * _ppOg ) {
		uint8_t * pui8Header = _ppOg->header;
		uint8_t * pui8Body = _ppOg->body;
		int32_t	i32BodySize = _ppOg->body_len;
		int32_t	i32SegPtr = 0;

		int32_t i32Version = ogg_page_version( _ppOg );
		int32_t i32Continued = ogg_page_continued( _ppOg );
		int32_t i32Bos = ogg_page_bos( _ppOg );
		int32_t i32Eos = ogg_page_eos( _ppOg );
		int64_t i64GranulePos = ogg_page_granulepos( _ppOg );
		int32_t i32SerialNo = ogg_page_serialno( _ppOg );
		int32_t i32PageNo = ogg_page_pageno( _ppOg );
		int32_t i32Segments = pui8Header[26];
		
		if ( ogg_stream_check( _pssOs ) ) { return -1; }

		// Clean up "returned data".
		{
			int32_t i32Lr = _pssOs->lacing_returned;
			int32_t i32Br = _pssOs->body_returned;

			// Body data.
			if ( i32Br ) {
				_pssOs->body_fill -= i32Br;
				if ( _pssOs->body_fill ) {
					CStd::MemMove( _pssOs->body_data, _pssOs->body_data + i32Br, _pssOs->body_fill );
				}
				_pssOs->body_returned = 0;
			}

			if ( i32Lr ) {
				// Segment table.
				if ( _pssOs->lacing_fill-i32Lr ) {
					CStd::MemMove( _pssOs->lacing_vals, _pssOs->lacing_vals + i32Lr, 
						(_pssOs->lacing_fill - i32Lr) * sizeof( (*_pssOs->lacing_vals) ) );
					CStd::MemMove( _pssOs->granule_vals, _pssOs->granule_vals + i32Lr, 
						(_pssOs->lacing_fill - i32Lr) * sizeof( (*_pssOs->granule_vals) ) );
				}
				_pssOs->lacing_fill -= i32Lr;
				_pssOs->lacing_packet -= i32Lr;
				_pssOs->lacing_returned = 0;
			}
		}

		// Check the serial number.
		if ( i32SerialNo != _pssOs->serialno ) { return -1; }
		if ( i32Version > 0 ) { return -1; }

		if ( _os_lacing_expand( _pssOs, i32Segments + 1 ) ) { return -1; }

		// Are we in sequence?
		if ( i32PageNo != _pssOs->pageno ) {
			int32_t I;

			// Unroll previous partial packet( if any ).
			for ( I = _pssOs->lacing_packet; I < _pssOs->lacing_fill; I++ ) {
				_pssOs->body_fill -= _pssOs->lacing_vals[I] & 0xFF;
			}
			_pssOs->lacing_fill = _pssOs->lacing_packet;

			// Make a note of dropped data in segment table.
			if ( _pssOs->pageno != -1 ) {
				_pssOs->lacing_vals[_pssOs->lacing_fill++] = 0x400;
				_pssOs->lacing_packet++;
			}
		}

		// Are we a "i32Continued packet" page?  If so, we may need to skip
		//	some i32Segments.
		if ( i32Continued ) {
			if ( _pssOs->lacing_fill < 1 || 
				 _pssOs->lacing_vals[_pssOs->lacing_fill-1] == 0x400 ) {
				i32Bos = 0;
				for ( ; i32SegPtr < i32Segments; i32SegPtr++ ) {
					int32_t i32Val = pui8Header[27+i32SegPtr];
					pui8Body += i32Val;
					i32BodySize -= i32Val;
					if ( i32Val < 255 ) {
						i32SegPtr++;
						break;
					}
				}
			}
		}
		
		if ( i32BodySize ) {
			if ( _os_body_expand( _pssOs, i32BodySize ) ) { return -1; }
			CStd::MemCpy( _pssOs->body_data + _pssOs->body_fill, pui8Body, i32BodySize );
			_pssOs->body_fill += i32BodySize;
		}

		{
			int32_t i32Saved = -1;
			while ( i32SegPtr < i32Segments ) {
				int32_t i32Val = pui8Header[27+i32SegPtr];
				_pssOs->lacing_vals[_pssOs->lacing_fill] = i32Val;
				_pssOs->granule_vals[_pssOs->lacing_fill] = -1;
					
				if ( i32Bos ) {
					_pssOs->lacing_vals[_pssOs->lacing_fill] |= 0x100;
					i32Bos = 0;
				}

				if ( i32Val < 255 ) { i32Saved = _pssOs->lacing_fill; }
					
				_pssOs->lacing_fill++;
				i32SegPtr++;
					
				if ( i32Val<255 ) { _pssOs->lacing_packet = _pssOs->lacing_fill; }
			}
			
			/* set the i64GranulePos on the last granuleval of the last full packet */
			if ( i32Saved!= -1 ) {
				_pssOs->granule_vals[i32Saved] = i64GranulePos;
			}

		}

		if ( i32Eos ) {
			_pssOs->e_o_s = 1;
			if ( _pssOs->lacing_fill > 0 ) {
				_pssOs->lacing_vals[_pssOs->lacing_fill-1] |= 0x200;
			}
		}

		_pssOs->pageno = i32PageNo + 1;

		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _ppOp Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_packetout( ogg_stream_state * _pssOs, ogg_packet * _ppOp ) {
		if ( ogg_stream_check( _pssOs ) ) { return 0; }
		return _packetout( _pssOs, _ppOp, 1 );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _ppOp Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_packetpeek( ogg_stream_state * _pssOs, ogg_packet * _ppOp ) {
		if ( ogg_stream_check( _pssOs ) ) { return 0; }
		return _packetout( _pssOs, _ppOp, 0 );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _i32SerialNo Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_init( ogg_stream_state * _pssOs, int32_t _i32SerialNo ) {
		if ( _pssOs ) {
			CStd::MemSet( _pssOs, 0, sizeof( (*_pssOs) ) );
			_pssOs->body_storage = 16 * 1024;
			_pssOs->lacing_storage = 1024;

			_pssOs->body_data = static_cast<uint8_t *>(CMemLib::MAlloc( _pssOs->body_storage*sizeof( (*_pssOs->body_data) ) ));
			_pssOs->lacing_vals = static_cast<int32_t *>(CMemLib::MAlloc( _pssOs->lacing_storage*sizeof( (*_pssOs->lacing_vals) ) ));
			_pssOs->granule_vals = static_cast<int64_t *>(CMemLib::MAlloc( _pssOs->lacing_storage*sizeof( (*_pssOs->granule_vals) ) ));

			if ( !_pssOs->body_data || !_pssOs->lacing_vals || !_pssOs->granule_vals ) {
				ogg_stream_clear( _pssOs );
				return -1;
			}

			_pssOs->serialno = _i32SerialNo;

			return 0;
		}
		return -1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_clear( ogg_stream_state * _pssOs ) {
		if ( _pssOs ) {
			if ( _pssOs->body_data ) { CMemLib::Free( _pssOs->body_data ); }
			if ( _pssOs->lacing_vals ) {CMemLib::Free( _pssOs->lacing_vals ); }
			if ( _pssOs->granule_vals ) { CMemLib::Free( _pssOs->granule_vals ); }

			CStd::MemSet( _pssOs, 0, sizeof( (*_pssOs) ) );
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_reset( ogg_stream_state * _pssOs ) {
		if ( ogg_stream_check( _pssOs ) ) { return -1; }

		_pssOs->body_fill = 0;
		_pssOs->body_returned = 0;

		_pssOs->lacing_fill = 0;
		_pssOs->lacing_packet = 0;
		_pssOs->lacing_returned = 0;

		_pssOs->header_fill = 0;

		_pssOs->e_o_s = 0;
		_pssOs->b_o_s = 0;
		_pssOs->pageno = -1;
		_pssOs->packetno = 0;
		_pssOs->granulepos = 0;

		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _i32SerialNo Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_reset_serialno( ogg_stream_state * _pssOs, int32_t _i32SerialNo ) {
		if ( ogg_stream_check( _pssOs ) ) { return -1; }
		ogg_stream_reset( _pssOs );
		_pssOs->serialno = _i32SerialNo;
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_destroy( ogg_stream_state * _pssOs ) {
		if ( _pssOs ) {
			ogg_stream_clear( _pssOs );
			CMemLib::Free( _pssOs );
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_check( ogg_stream_state * _pssOs ) {
		if ( !_pssOs || !_pssOs->body_data ) { return -1; }
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_eos( ogg_stream_state * _pssOs ) {
		if ( ogg_stream_check( _pssOs ) ) { return 1; }
		return _pssOs->e_o_s;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::ogg_page_checksum_set( ogg_page * _ppOg ) {
		if ( _ppOg ) {
			uint32_t ui32CrcReg = 0;
			int32_t I;

			// Safety; _i32Needed for API behavior, but not framing code.
			_ppOg->header[22] = 0;
			_ppOg->header[23] = 0;
			_ppOg->header[24] = 0;
			_ppOg->header[25] = 0;
				
			for ( I = 0; I < _ppOg->header_len; I++ ) {
				ui32CrcReg = (ui32CrcReg << 8) ^ crc_lookup[((ui32CrcReg>>24)&0xFF)^_ppOg->header[I]];
			}
			for ( I = 0; I < _ppOg->body_len; I++ ) {
				ui32CrcReg = (ui32CrcReg << 8) ^ crc_lookup[((ui32CrcReg>>24)&0xFF)^_ppOg->body[I]];
			}
				
			_ppOg->header[22] = static_cast<uint8_t>(ui32CrcReg&0xFF);
			_ppOg->header[23] = static_cast<uint8_t>((ui32CrcReg >> 8)&0xFF);
			_ppOg->header[24] = static_cast<uint8_t>((ui32CrcReg >> 16)&0xFF);
			_ppOg->header[25] = static_cast<uint8_t>((ui32CrcReg >> 24)&0xFF);
		}
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_version( const ogg_page * _ppOg ) {
		return static_cast<int32_t>(_ppOg->header[4]);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_continued( const ogg_page * _ppOg ) {
		return static_cast<int32_t>(_ppOg->header[5] & 0x01);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_bos( const ogg_page * _ppOg ) {
		return static_cast<int32_t>(_ppOg->header[5] & 0x02);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_eos( const ogg_page * _ppOg ) {
		return static_cast<int32_t>(_ppOg->header[5] & 0x04);
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int64_t LSE_CALL COgg::ogg_page_granulepos( const ogg_page * _ppOg ) {
		uint8_t *page = _ppOg->header;
		int64_t i64GranulePos = page[13] & 0xFF;
		i64GranulePos = (i64GranulePos << 8) | (page[12] & 0xFF);
		i64GranulePos = (i64GranulePos << 8) | (page[11] & 0xFF);
		i64GranulePos = (i64GranulePos << 8) | (page[10] & 0xFF);
		i64GranulePos = (i64GranulePos << 8) | (page[9] & 0xFF);
		i64GranulePos = (i64GranulePos << 8) | (page[8] & 0xFF);
		i64GranulePos = (i64GranulePos << 8) | (page[7] & 0xFF);
		i64GranulePos = (i64GranulePos << 8) | (page[6] & 0xFF);
		return i64GranulePos;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_serialno( const ogg_page * _ppOg ) {
		return (_ppOg->header[14] |
			(_ppOg->header[15] << 8) |
			(_ppOg->header[16] << 16) |
			(_ppOg->header[17] << 24));
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_pageno( const ogg_page * _ppOg ) {
		return (_ppOg->header[18] |
			(_ppOg->header[19] << 8) |
			(_ppOg->header[20] << 16) |
			(_ppOg->header[21] << 24));
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOg Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_page_packets( const ogg_page * _ppOg ) {
		int32_t I, N = _ppOg->header[26], i32Count = 0;
		for ( I = 0;I < N; I++ ) {
			if ( _ppOg->header[27+I] < 255 ) { i32Count++; }
		}
		return i32Count;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppOp Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	void LSE_CALL COgg::ogg_packet_clear( ogg_packet * _ppOp ) {
		CMemLib::Free( _ppOp->packet );
		CStd::MemSet( _ppOp, 0, sizeof( (*_ppOp) ) );
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _ppbBuffer Undocumented parameter from the Ogg library.
	 * \param _pvSource Undocumented parameter from the Ogg library.
	 * \param _i32Bits Undocumented parameter from the Ogg library.
	 * \param _i32Msb Undocumented parameter from the Ogg library.
	 */
	void LSE_CALL COgg::oggpack_writecopy_helper( oggpack_buffer * _ppbBuffer, 
		void * _pvSource, 
		int32_t _i32Bits, 
		void (LSE_CALL * PfFunc)( oggpack_buffer *, 
			uint32_t, 
			int32_t ), 
		int32_t _i32Msb ) {
		uint8_t * pui8Ptr = static_cast<uint8_t *>(_pvSource);

		int32_t _i32Bytes = _i32Bits / 8;
		_i32Bits -= _i32Bytes * 8;

		if ( _ppbBuffer->endbit ) {
			int32_t I;
			// Unaligned copy.	Do it the hard way.
			for ( I = 0; I < _i32Bytes; I++ ) {
				PfFunc( _ppbBuffer, static_cast<uint32_t>(pui8Ptr[I]), 8 );
			}
		}
		else {
			// Aligned block copy.
			if ( _ppbBuffer->endbyte + _i32Bytes + 1 >= _ppbBuffer->storage ) {
				void * pvRet;
				if ( !_ppbBuffer->ptr ) { goto Err; }
				if ( _ppbBuffer->endbyte + _i32Bytes + LSS_BUFFER_INCREMENT > _ppbBuffer->storage ) { goto Err; }
				_ppbBuffer->storage = _ppbBuffer->endbyte + _i32Bytes + LSS_BUFFER_INCREMENT;
				pvRet = CMemLib::MReAlloc( _ppbBuffer->buffer, _ppbBuffer->storage );
				if ( !pvRet ) { goto Err; }
				_ppbBuffer->buffer = static_cast<uint8_t *>(pvRet);
				_ppbBuffer->ptr = _ppbBuffer->buffer + _ppbBuffer->endbyte;
			}

			CStd::MemMove( _ppbBuffer->ptr, _pvSource, _i32Bytes );
			_ppbBuffer->ptr += _i32Bytes;
			_ppbBuffer->endbyte += _i32Bytes;
			(*_ppbBuffer->ptr) = 0;

		}
		if ( _i32Bits ) {
			if ( _i32Msb ) {
				PfFunc( _ppbBuffer, static_cast<uint32_t>(pui8Ptr[_i32Bytes] >> (8 - _i32Bits)), _i32Bits );
			}
			else {
				PfFunc( _ppbBuffer, static_cast<uint32_t>(pui8Ptr[_i32Bytes]), _i32Bits );
			}
		}
		return;
	Err :
		oggpack_writeclear( _ppbBuffer );
	}	

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _i32Needed Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::_os_lacing_expand( ogg_stream_state * _pssOs, int32_t _i32Needed ) {
		if ( _pssOs->lacing_storage <= _pssOs->lacing_fill + _i32Needed ) {
			void * pvRet;
			pvRet = CMemLib::MReAlloc( _pssOs->lacing_vals, (_pssOs->lacing_storage + _i32Needed + 32) *
				sizeof( (*_pssOs->lacing_vals) ) );
			if ( !pvRet ) {
				ogg_stream_clear( _pssOs );
				return -1;
			}
			_pssOs->lacing_vals = static_cast<int32_t *>(pvRet);
			pvRet = CMemLib::MReAlloc( _pssOs->granule_vals, (_pssOs->lacing_storage + _i32Needed + 32) *
				sizeof( (*_pssOs->granule_vals) ) );
			if ( !pvRet ) {
				ogg_stream_clear( _pssOs );
				return -1;
			}
			_pssOs->granule_vals = static_cast<int64_t *>(pvRet);
			_pssOs->lacing_storage += (_i32Needed + 32);
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _i32Needed Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::_os_body_expand( ogg_stream_state * _pssOs, int32_t _i32Needed ) {
		if ( _pssOs->body_storage <= _pssOs->body_fill+_i32Needed ) {
			void * pvRet;
			pvRet = CMemLib::MReAlloc( _pssOs->body_data, (_pssOs->body_storage + _i32Needed + 1024) *
				sizeof( (*_pssOs->body_data) ) );
			if ( !pvRet ) {
				ogg_stream_clear( _pssOs );
				return -1;
			}
			_pssOs->body_storage += _i32Needed + 1024;
			_pssOs->body_data = static_cast<uint8_t *>(pvRet);
		}
		return 0;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _ppOp Undocumented parameter from the Ogg library.
	 * \param _i32Adv Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::_packetout( ogg_stream_state * _pssOs, ogg_packet * _ppOp, int32_t _i32Adv ) {
		// The last part of decode.  We have the stream broken into packet
		//	segments.  Now we need to group them into packets( or return the
		//	out of sync markers ).

		int32_t i32Ptr = _pssOs->lacing_returned;

		if ( _pssOs->lacing_packet <= i32Ptr ) { return 0; }

		if ( _pssOs->lacing_vals[i32Ptr] & 0x400 ) {
			// We need to tell the codec there's a gap; it might need to
			//	handle previous packet dependencies.
			_pssOs->lacing_returned++;
			_pssOs->packetno++;
			return -1;
		}

		if ( !_ppOp && !_i32Adv ) { return 1; }	// Just using peek as an inexpensive way
												//	to ask if there's a whole packet
												//	waiting.

		// Gather the whole packet.  We'll have no holes or a partial packet.
		{
			int32_t _i32Size = _pssOs->lacing_vals[i32Ptr] & 0xFF;
			int32_t _i32Bytes = _i32Size;
			int32_t i32Eos = _pssOs->lacing_vals[i32Ptr] & 0x200; // Last packet of the stream?
			int32_t i32Bos = _pssOs->lacing_vals[i32Ptr] & 0x100; // First packet of the stream?

			while ( _i32Size == 255 ) {
				int32_t i32Val = _pssOs->lacing_vals[++i32Ptr];
				_i32Size = i32Val & 0xFF;
				if ( i32Val & 0x200 ) { i32Eos = 0x200; }
				_i32Bytes += _i32Size;
			}

			if ( _ppOp ) {
				_ppOp->e_o_s = i32Eos;
				_ppOp->b_o_s = i32Bos;
				_ppOp->packet = _pssOs->body_data + _pssOs->body_returned;
				_ppOp->packetno = _pssOs->packetno;
				_ppOp->granulepos = _pssOs->granule_vals[i32Ptr];
				_ppOp->bytes = _i32Bytes;
			}

			if ( _i32Adv ) {
				_pssOs->body_returned += _i32Bytes;
				_pssOs->lacing_returned = i32Ptr + 1;
				_pssOs->packetno++;
			}
		}
		return 1;
	}

	/**
	 * Undocumented function from the Ogg library.
	 *
	 * \param _pssOs Undocumented parameter from the Ogg library.
	 * \param _popOg Undocumented parameter from the Ogg library.
	 * \param _i32Force Undocumented parameter from the Ogg library.
	 * \param _i32NFill Undocumented parameter from the Ogg library.
	 * \return Undocumented return from the Ogg library.
	 */
	int32_t LSE_CALL COgg::ogg_stream_flush_i( ogg_stream_state * _pssOs, ogg_page * _popOg, int32_t _i32Force, int32_t _i32NFill ) {
		int32_t I;
		int32_t i32Vals = 0;
		int32_t i32MaxVals = (_pssOs->lacing_fill > 255 ? 255 : _pssOs->lacing_fill);
		int32_t i32Bytes = 0;
		int32_t i32Acc = 0;
		int64_t i64GranulePos = -1;

		if ( ogg_stream_check( _pssOs ) ) { return 0; }
		if ( i32MaxVals == 0 ) { return 0; }

		// Construct a page.
		// Decide how many segments to include.

		// If this is the initial header case, the first page must only include
		//	the initial header packet.
		if ( _pssOs->b_o_s == 0 ) {
			i64GranulePos = 0;
			for ( i32Vals = 0; i32Vals < i32MaxVals; i32Vals++ ) {
				if ( (_pssOs->lacing_vals[i32Vals] & 0x0FF) < 255 ) {
					i32Vals++;
					break;
				}
			}
		}
		else {

			// The extra i32PacketsDone, i32PacketJustDone logic here attempts to do two things:
			//	1 ) Don't unneccessarily span pages.
			//	2 ) Unless necessary, don't flush pages if there are less than four packets on
			//		them; this expands page size to reduce unneccessary overhead if incoming packets
			//		are large.
			// These are not necessary behaviors, just "always better than naive flushing"
			//	without requiring an application to explicitly request a specific optimized
			//	behavior. We'll want an explicit behavior setup pathway eventually as well.

			int32_t i32PacketsDone = 0;
			int32_t i32PacketJustDone = 0;
			for ( i32Vals = 0; i32Vals < i32MaxVals; i32Vals++ ) {
				if ( i32Acc > _i32NFill && i32PacketJustDone >= 4 ) {
					_i32Force=1;
					break;
				}
				i32Acc += _pssOs->lacing_vals[i32Vals] & 0x0FF;
				if ( (_pssOs->lacing_vals[i32Vals] & 0xFF) < 255 ) {
					i64GranulePos = _pssOs->granule_vals[i32Vals];
					i32PacketJustDone = ++i32PacketsDone;
				}
				else {
					i32PacketJustDone = 0;
				}
			}
			if ( i32Vals == 255 ) { _i32Force = 1; }
		}

		if ( !_i32Force ) { return 0; }

		// Construct the header in temp storage.
		CStd::MemCpy( _pssOs->header, "OggS", 4 );

		// Stream structure version.
		_pssOs->header[4] = 0x00;

		// Continued packet flag?
		_pssOs->header[5] = 0x00;
		if ( (_pssOs->lacing_vals[0] & 0x100) == 0 ) { _pssOs->header[5] |= 0x01; }
		// First page flag?
		if ( _pssOs->b_o_s == 0 ) { _pssOs->header[5] |= 0x02; }
		// Last page flag?
		if ( _pssOs->e_o_s && _pssOs->lacing_fill == i32Vals ) { _pssOs->header[5] |= 0x04; }
		_pssOs->b_o_s = 1;

		// 64 bits of PCM position.
		for ( I = 6; I < 14; I++ ) {
			_pssOs->header[I] = static_cast<uint8_t>(i64GranulePos & 0xFF);
			i64GranulePos >>= 8;
		}

		// 32 bits of stream serial number.
		{
			int32_t i32SerialNo = _pssOs->serialno;
			for ( I = 14; I < 18; I++ ) {
				_pssOs->header[I] = static_cast<uint8_t>(i32SerialNo & 0xFF);
				i32SerialNo >>= 8;
			}
		}

		// 32 bits of page counter (we have both counter and page header
		//	because this val can roll over).
		if ( _pssOs->pageno == -1 ) { _pssOs->pageno = 0; }	// Because someone called
															//	stream_reset; this would be a
															//	strange thing to do in an
															//	encode stream, but it has
															//	plausible uses.
		{
			int32_t i32PageNo=_pssOs->pageno++;
			for ( I = 18; I < 22; I++ ) {
				_pssOs->header[I] = static_cast<uint8_t>(i32PageNo & 0xFF);
				i32PageNo >>= 8;
			}
		}
		
		// Zero for computation; filled in later.
		_pssOs->header[22] = 0;
		_pssOs->header[23] = 0;
		_pssOs->header[24] = 0;
		_pssOs->header[25] = 0;
		
		// Segment table.
		_pssOs->header[26] = static_cast<uint8_t>(i32Vals & 0xFF);
		for ( I = 0; I < i32Vals; I++ ) {
			i32Bytes += _pssOs->header[I+27] = static_cast<uint8_t>(_pssOs->lacing_vals[I] & 0xFF);
		}
		
		// Set pointers in the ogg_page struct.
		_popOg->header=_pssOs->header;
		_popOg->header_len = _pssOs->header_fill = i32Vals + 27;
		_popOg->body = _pssOs->body_data + _pssOs->body_returned;
		_popOg->body_len = i32Bytes;
		
		// Advance the lacing data and set the body_returned pointer.
		
		_pssOs->lacing_fill -= i32Vals;
		CStd::MemMove( _pssOs->lacing_vals, _pssOs->lacing_vals + i32Vals, _pssOs->lacing_fill * sizeof( (*_pssOs->lacing_vals) ) );
		CStd::MemMove( _pssOs->granule_vals, _pssOs->granule_vals + i32Vals, _pssOs->lacing_fill * sizeof( (*_pssOs->granule_vals) ) );
		_pssOs->body_returned += i32Bytes;
		
		// Calculate the checksum.
		ogg_page_checksum_set( _popOg );

		return 1;
	}

}	// namespace lss

#undef LSS_BUFFER_INCREMENT

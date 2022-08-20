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
 * Description: Generate unique states.
 */

#include "LSTStateFactory.h"


// == Functions.
/**
 * Creates a state by ID.
 *
 * \param _ui32Id The unique ID of the state to create.
 * \return Returns the created state or NULL if none could be created.
 */
CState * LSE_CALL ::CStateFactory::GetState( uint32_t _ui32Id ) const {
	switch ( _ui32Id ) {
		/*case LST_S_SOUND : {
			return LSENEW CSoundTest();
		}*/
		case LST_S_BENCHMARK : {
			return LSENEW CBenchMark();
		}
		case LST_S_MODELTEST : {
			return LSENEW CBaseTest();
		}
		case LST_S_SPRITETEST : {
			return LSENEW CSpriteTest();
		}/*
		case LST_S_TERRAINTEST : {
			return LSENEW CTerrainTest();
		}
		case LST_S_BUILDINGTEST : {
			return LSENEW CBuildingTest();
		}
		case LST_S_PERFECMAPS : {
			return LSENEW CPerfectMaps();
		}*/
		case LST_S_TRIANGLETEST : {
			return LSENEW CTriangleTest();
		}
		case LST_S_TEXTURETEST : {
			return LSENEW CTextureTest();
		}
		case LST_S_LOADMESHTEST : {
			return LSENEW CLoadMeshTest();
		}
	}
	return lse::CStateFactory::GetState( _ui32Id );
}
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


#ifndef __LST_STATEFACTORY_H__
#define __LST_STATEFACTORY_H__

#include "../LSTTest.h"
#include "LSTBaseTest.h"
#include "LSTLoadMeshTest.h"
#include "LSTBenchMark.h"
/*#include "LSTBuildingTest.h"
#include "LSTModelTest.h"
#include "LSTPerfectMaps.h"
#include "LSTSoundTest.h"*/
#include "LSTSpriteTest.h"
#include "LSTStates.h"
//#include "LSTTerrainTest.h"
#include "LSTTriangleTest.h"
#include "LSTTextureTest.h"

/**
 * Class CStateFactory
 * \brief Generate unique states.
 *
 * Description: Generate unique states.
 */
class CStateFactory : public lse::CStateFactory {
	// All is public.  This class has no secrets.
public :
	// == Functions.
	/**
	 * Creates a state by ID.
	 *
	 * \param _ui32Id The unique ID of the state to create.
	 * \return Returns the created state or NULL if none could be created.
	 */
	virtual CState * LSE_CALL				GetState( uint32_t _ui32Id ) const;
};

#endif	// __LST_STATEFACTORY_H__


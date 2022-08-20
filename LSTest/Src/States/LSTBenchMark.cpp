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
 * Description: Benchmarking the engine.
 */

#include "LSTBenchMark.h"
#include "Bmp/LSIBmp.h"
#include "Map/LSTLMap.h"
#include "MemOps/LSTLMemCmpF.h"
#include <vector>

#ifdef __ARM_NEON__
#include "Matrix/LSMMatrix4x4_ARM.h"
#include <GLKit/GLKit.h>
#include <GLKit/GLKMatrix4.h>
#endif	// #ifdef __ARM_NEON__

#include "Matrix/LSMMatrix4x4_SSE3.h"


// == Various constructors.
LSE_CALLCTOR CBenchMark::CBenchMark() {
}




float SinF_Medium( float _fX ) {
    int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float fX2 = _fX * _fX;
	return (i32I & 1) ?
		-_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 )))))) :
		_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 ))))));
}

float SinF_Medium2( float _fX ) {
    int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );    // 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float x2 = _fX * _fX;
    return (i32I & 1) ?    
        (((((-2.05329886426852681325e-08f*x2 + 2.70405212177138309926e-06f)*x2
            - 1.98125766473822295666e-04f)*x2 + 8.33255797624588012695e-03f)*x2
            - 1.66665777564048767090e-01f)*x2 + 9.99999701976776123047e-01f)*-_fX :
        (((((-2.05329886426852681325e-08f*x2 + 2.70405212177138309926e-06f)*x2
            - 1.98125766473822295666e-04f)*x2 + 8.33255797624588012695e-03f)*x2
            - 1.66665777564048767090e-01f)*x2 + 9.99999701976776123047e-01f)*_fX;
}

float SinF_G( float _fX ) {
	int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float x2 = _fX * _fX;
    return (i32I & 1) ?
        (((((-2.05342856289746600727e-08f*x2 + 2.70405218307799040084e-06f)*x2
            - 1.98125763417806681909e-04f)*x2 + 8.33255814755188010464e-03f)*x2
            - 1.66665772196961623983e-01f)*x2 + 9.99999707044156546685e-01f)*-_fX :
        (((((-2.05342856289746600727e-08f*x2 + 2.70405218307799040084e-06f)*x2
            - 1.98125763417806681909e-04f)*x2 + 8.33255814755188010464e-03f)*x2
            - 1.66665772196961623983e-01f)*x2 + 9.99999707044156546685e-01f)*_fX;

}


uint64_t Factorial( uint64_t _ui64Val ) {
	if ( !_ui64Val ) { return 0; }
	uint64_t ui64Ret = _ui64Val;
	while ( --_ui64Val ) {
		ui64Ret *= _ui64Val;
	}
	return ui64Ret;
}

double Sin( double _dVal ) {
	int64_t i32I = int64_t( _dVal * 0.31830988618379067153776752674503 );
	_dVal = (_dVal - double( i32I ) * 3.1415926535897932384626433832795);

	double fX2 = _dVal * _dVal;

	return (i32I & 1) ?
		-_dVal * (1.0 +
			fX2 * (( -1.0 / Factorial( 3 ) ) +
			fX2 * (( 1.0 / Factorial( 5 ) ) +
			fX2 * (( -1.0 / Factorial( 7 ) ) +
			fX2 * (( 1.0 / Factorial( 9 ) ) +
			fX2 * (( -1.0 / Factorial( 11 ) ) +
			fX2 * (( 1.0 / Factorial( 13 ) ) +
			fX2 * (( -1.0 / Factorial( 15 ) ) +
			fX2 * (( 1.0 / Factorial( 17 ) ) +
			fX2 * (( -1.0 / Factorial( 19 ) ))))))))))) :
		_dVal * (1.0 +
			fX2 * (( -1.0 / Factorial( 3 ) ) +
			fX2 * (( 1.0 / Factorial( 5 ) ) +
			fX2 * (( -1.0 / Factorial( 7 ) ) +
			fX2 * (( 1.0 / Factorial( 9 ) ) +
			fX2 * (( -1.0 / Factorial( 11 ) ) +
			fX2 * (( 1.0 / Factorial( 13 ) ) +
			fX2 * (( -1.0 / Factorial( 15 ) ) +
			fX2 * (( 1.0 / Factorial( 17 ) ) +
			fX2 * (( -1.0 / Factorial( 19 ) )))))))))));
}

template <typename fType>
fType SinVariable( fType _dVal, fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5, fType _d6, fType _d7 ) {
	int32_t i32I = int32_t( _dVal * fType( 0.31830988618379067153776752674503 ) );
	_dVal = (_dVal - fType( i32I ) * fType( 3.1415926535897932384626433832795 ));

	fType fX2 = _dVal * _dVal;
	return (i32I & 1) ?
		-_dVal * (_d0 +
			fX2 * (( -_d1 ) +
			fX2 * (( _d2 ) +
			fX2 * (( -_d3 ) +
			fX2 * (( _d4 ) +
			fX2 * (( -_d5 ) +
			fX2 * (( _d6 ) +
			fX2 * (( -_d7 ))))))))) :
		_dVal * (_d0 +
			fX2 * (( -_d1 ) +
			fX2 * (( _d2 ) +
			fX2 * (( -_d3 ) +
			fX2 * (( _d4 ) +
			fX2 * (( -_d5 ) +
			fX2 * (( _d6 ) +
			fX2 * (( -_d7 )))))))));
}
template <typename fType>
fType SinVariable_11( fType _dVal, fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5 ) {
	int32_t i32I = int32_t( _dVal * fType( 0.31830988618379067153776752674503 ) );
	_dVal = (_dVal - fType( i32I ) * fType( 3.1415926535897932384626433832795 ));

	fType fX2 = _dVal * _dVal;
	return (i32I & 1) ?
		-_dVal * (_d0 +
			fX2 * (( -_d1 ) +
			fX2 * (( _d2 ) +
			fX2 * (( -_d3 ) +
			fX2 * (( _d4 ) +
			fX2 * (( -_d5 ))))))) :
		_dVal * (_d0 +
			fX2 * (( -_d1 ) +
			fX2 * (( _d2 ) +
			fX2 * (( -_d3 ) +
			fX2 * (( _d4 ) +
			fX2 * (( -_d5 )))))));
}
template <typename fType>
fType CosVariable( fType _dVal, fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5, fType _d6, fType _d7 ) {
	int32_t i32I = int32_t( _dVal * fType( 0.31830988618379067153776752674503 ) );
	_dVal = (_dVal - fType( i32I ) * fType( 3.1415926535897932384626433832795 ));

	fType fX2 = _dVal * _dVal;
	return (i32I & 1) ?
		-_d0 -
			fX2 * (fType( -_d1 ) +
			fX2 * (fType( _d2 ) +
			fX2 * (fType( -_d3 ) +
			fX2 * (fType( _d4 ) +
			fX2 * (fType( -_d5 ) +
			fX2 * (fType( _d6 ) +
			fX2 * fType( -_d7 ))))))) :
		_d0 +
			fX2 * (fType( -_d1 ) +
			fX2 * (fType( _d2 ) +
			fX2 * (fType( -_d3 ) +
			fX2 * (fType( _d4 ) +
			fX2 * (fType( -_d5 ) +
			fX2 * (fType( _d6 ) +
			fX2 * fType( -_d7 )))))));
}
template <typename fType>
fType CosVariable_10( fType _dVal, fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5 ) {
	int32_t i32I = int32_t( _dVal * fType( 0.31830988618379067153776752674503 ) );
	_dVal = (_dVal - fType( i32I ) * fType( 3.1415926535897932384626433832795 ));

	fType fX2 = _dVal * _dVal;
	return (i32I & 1) ?
		-_d0 -
			fX2 * (fType( -_d1 ) +
			fX2 * (fType( _d2 ) +
			fX2 * (fType( -_d3 ) +
			fX2 * (fType( _d4 ) +
			fX2 * (fType( -_d5 )))))) :
		_d0 +
			fX2 * (fType( -_d1 ) +
			fX2 * (fType( _d2 ) +
			fX2 * (fType( -_d3 ) +
			fX2 * (fType( _d4 ) +
			fX2 * (fType( -_d5 ))))));
}

template <typename fType>
fType EvalSin( fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5, fType _d6, fType _d7, fType _dMaxError ) {
	const uint32_t ui32Total = 12000;
	fType dError = 0.0;
	for ( uint32_t I = 0; I <= ui32Total; ++I ) {
		fType dI = I * (fType( 6.283185307179586476925286766559 ) / ui32Total) - fType( 3.1415926535897932384626433832795 );
		fType dSin = static_cast<fType>(::sin( dI ));
		fType dSinAcc = SinVariable<fType>( dI, _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7 );
		if ( ::fabs( dSinAcc ) > 1.0 ) { return 10.0; }
		dError += static_cast<fType>(::fabs( dSin - dSinAcc ));
		if ( dError > _dMaxError ) { break; }
	}
	return dError;
}

template <typename fType>
fType EvalCos( fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5, fType _d6, fType _d7, fType _dMaxError ) {
	const uint32_t ui32Total = 12000;
	fType dError = 0.0;
	for ( uint32_t I = 0; I <= ui32Total; ++I ) {
		fType dI = fType( I * (6.283185307179586476925286766559 / ui32Total) - 3.1415926535897932384626433832795 );
		fType dSin = static_cast<fType>(::cos( dI ));
		fType dSinAcc = CosVariable<fType>( dI, _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7 );
		if ( ::fabs( dSinAcc ) > 1.0 ) { return 10.0; }
		dError += static_cast<fType>(::fabs( dSin - dSinAcc ));
		if ( dError > _dMaxError ) { break; }
	}
	return dError;
}

template <typename fType>
fType EvalSin_11( fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5, fType _dMaxError, fType &_dMaxError2 ) {
	const uint32_t ui32Total = 12000000;
	fType dError = 0.0;
	fType dMaxError = 0.0;
	for ( uint32_t I = 0; I <= ui32Total; ++I ) {
		fType dI = I * (fType( 6.283185307179586476925286766559 ) / ui32Total) - fType( 3.1415926535897932384626433832795 );
		fType dSin = static_cast<fType>(::sin( dI ));
		fType dSinAcc = SinVariable_11<fType>( dI, _d0, _d1, _d2, _d3, _d4, _d5 );
		if ( ::fabs( dSinAcc ) > 1.0 ) { return 1000000.0; }
		fType dThisError = static_cast<fType>(::fabs( dSin - dSinAcc ));
		if ( dThisError > _dMaxError2 ) { return 1000000.0; }
		dMaxError = CStd::Max<fType>( dMaxError, dThisError );
		dError += dThisError;
		if ( dError > _dMaxError ) { break; }
	}
	_dMaxError2 = dMaxError;
	return dError;
}

template <typename fType>
fType EvalCos_10( fType _d0, fType _d1, fType _d2, fType _d3, fType _d4, fType _d5, fType _dMaxError, fType &_dMaxError2 ) {
	const uint32_t ui32Total = 120000;
	fType dError = 0.0;
	fType dMaxError = 0.0;
	for ( uint32_t I = 0; I <= ui32Total; ++I ) {
		fType dI = fType( I * (6.283185307179586476925286766559 / ui32Total) - 3.1415926535897932384626433832795 );
		fType dSin = static_cast<fType>(::cos( dI ));
		fType dSinAcc = CosVariable_10<fType>( dI, _d0, _d1, _d2, _d3, _d4, _d5 );
		if ( ::fabs( dSinAcc ) > 1.0 ) { return 1000002.0; }
		fType dThisError = static_cast<fType>(::fabs( dSin - dSinAcc ));
		if ( dThisError > _dMaxError2 ) { return 1000000.0; }
		dMaxError = CStd::Max<fType>( dMaxError, dThisError );
		dError += dThisError;
		dError += static_cast<fType>(::fabs( dSin - dSinAcc ));
		if ( dError > _dMaxError ) { break; }
	}
	_dMaxError2 = dMaxError;
	return dError;
}


float atan_near_0(float x, float _f0 = 1.0f, float _f1 = 0.333333333f, float _f2 = 0.060093788f) {
  float x2 = x * x;
  return x / (_f0 + _f1 * x2 - _f2 * x2 * x2);
}

float my_atan(float x, float _f0 = 1.0f, float _f1 = 0.333333333f, float _f2 = 0.060093788f) {
  static const float pi_halves = float(std::atan(1.0)*2.0);
  if (std::fabs(x) > 1.0f)
    return std::copysign(pi_halves, x) - atan_near_0(1.0f/x, _f0, _f1, _f2);
  return atan_near_0(x, _f0, _f1, _f2);
}

double EvalAtan( float _f0, float _f1, float _f2 ) {
	//if ( my_atan( 1.0f, _f0, _f1, _f2 ) != (3.1415926535897932384626433832795f / 4.0f) ) { return 100000.0f; }
	double dError = 0.0;
	for ( uint32_t I = 0; I < 150; ++I ) {
		float fI = I / 10.0f;
		double dAtan = ::atanf( fI );
		double dAtanApp = my_atan( fI, _f0, _f1, _f2 );
		double dThis = fabs( dAtan - dAtanApp );
		dError += dThis;
	}

	return dError;
}



float SinF_LS( float _fX ) {
    int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float fX2 = _fX * _fX;
	return (i32I & 1) ?
		-_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 )))))) :
		_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 ))))));
}

float sin_garrett( float _fX ) {
	int32_t i32M = static_cast<int32_t>( _fX * (1.0f / 3.1415926535897932384626433832795f) );
	_fX = (_fX - static_cast<float>( i32M ) * 3.1415926535897932384626433832795f);

	float fX2 = _fX * _fX;
	return (i32M & 1) ?
		(((((-2.05342856289746600727e-08f * fX2 + 2.70405218307799040084e-06f) * fX2
			- 1.98125763417806681909e-04f) * fX2 + 8.33255814755188010464e-03f) * fX2
			- 1.66665772196961623983e-01f) * fX2 + 9.99999707044156546685e-01f) * -_fX :
		(((((-2.05342856289746600727e-08f * fX2 + 2.70405218307799040084e-06f) * fX2
			- 1.98125763417806681909e-04f) * fX2 + 8.33255814755188010464e-03f) * fX2
			- 1.66665772196961623983e-01f) * fX2 + 9.99999707044156546685e-01f) * _fX;

}

float SinAdam(float x)
{
    int32_t i32I = int32_t( x * (1.0f / 3.1415926535897932384626433832795f) );
    x = (x - float( i32I ) * 3.1415926535897932384626433832795f);

    float fX2 = x * x;
    float fX4 = fX2 * fX2;
    float fX6 = fX2 * fX4;
    float fX8 = fX4 * fX4;
    float fX10 = fX6 * fX4;
    float fX12 = fX6 * fX6;
    float fX14 = fX6 * fX8;

    return (i32I & 1) ?
        -x * (float( 1.00000000000000000000e+00 ) +
            (fX2 * float( -1.66666671633720397949e-01 )) +
            ((fX4 * float( 8.33333376795053482056e-03 )) +
            (fX6 * float( -1.98412497411482036114e-04 ))) +
            ((fX8 * float( 2.75565571428160183132e-06 )) +
            (fX10 * float( -2.50368472620721149724e-08 ))) +
            ((fX12 * float( 1.58849267073435385100e-10 )) +
            (fX14 * float( -6.58925550841432672300e-13 )))
                ):
        x * (float( 1.00000000000000000000e+00 ) +
            (fX2 * float( -1.66666671633720397949e-01 )) +
            ((fX4 * float( 8.33333376795053482056e-03 )) +
            (fX6 * float( -1.98412497411482036114e-04 ))) +
            ((fX8 * float( 2.75565571428160183132e-06 )) +
            (fX10 * float( -2.50368472620721149724e-08 ))) +
            ((fX12 * float( 1.58849267073435385100e-10 )) +
            (fX14 * float( -6.58925550841432672300e-13 )))
                );
}

// == Functions.
/**
 * Called when the state is just created.  Use this to initialize resources.
 *
 * \param _pgGame Can be accessed for whatever game-specific information needs to be
 *	obtained.
 * \param _ui32State The current state identifier (the same class could
 *	then be used for multiple states if they are similar enough).
 * \param _ui32PrevState The identifier of the previous state.
 * \param _uptrUser User-defined data passed to the state.  How this is used is entirely
 *	defined by the state.  Examples of use:
 * #1: Which map to load in a game state.
 * #2: Which tutorial page to show in a tutorial state.
 * #3: Used as a pointer to data containing a lot of state-specific information such as
 *	map file to load, previous state of the player, etc.
 */
void LSE_CALL CBenchMark::Init( CGame * /*_pgGame*/, int32_t /*_ui32State*/, int32_t /*_ui32PrevState*/, uintptr_t /*_uptrUser*/ ) {
	/*char szBuffer[128];
	_gcvt_s( szBuffer, LSE_ELEMENTS( szBuffer ), 23.365354, 64 );
	CStd::DebugPrintA( szBuffer );*/
	CTime tTimer;

	CMap<int, int> mMap;
	mMap.Insert( 9, 9 );
	mMap.Insert( 8, 8 );
	mMap.Insert( 7, 7 );
	mMap.Insert( 4, 4 );
	mMap.Insert( 2, 2 );
	mMap.Insert( 0, 0 );


	{
		uint64_t ui64Time = tTimer.GetRealTime();
		float fVal = 0.0f;
		for ( int32_t I = 0; I < 2300000; ++I ) {
			fVal += ::sinf( I / 134.0f );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		char szBuffer[512];																					
		CStd::SPrintF( szBuffer, 512, "%.33f %u \r\n", fVal, (uint32_t)ui64Total0 );
		CStd::DebugPrintA( szBuffer );


		ui64Time = tTimer.GetRealTime();
		fVal = 0.0f;
		for ( int32_t I = 0; I < 2300000; ++I ) {
			fVal += SinAdam( I / 134.0f );
		}
		ui64Total0 = tTimer.GetRealTime() - ui64Time;

		CStd::SPrintF( szBuffer, 512, "%.33f %u \r\n", fVal, (uint32_t)ui64Total0 );
		CStd::DebugPrintA( szBuffer );
	}


	float fTest0 = 1.00022506713867187500e+00f;//9.99934434890747070313e-01;//1.0f;
	float fTest1 = 3.24211299419403076172e-01f;//3.32740783691406250000e-01;//0.333333333f;
	float fTest2 = 5.11969886720180511475e-02f;//6.53432160615921020508e-02;//0.060093788f;
	int32_t & iTest0 = (*reinterpret_cast<int32_t *>(&fTest0));
	int32_t & iTest1 = (*reinterpret_cast<int32_t *>(&fTest1));
	int32_t & iTest2 = (*reinterpret_cast<int32_t *>(&fTest2));


	//0.78539816339744830961566084581988
#define LST_PRINT_STUFF																								\
	{																												\
		char szBuffer[512];																						\
		CStd::SPrintF( szBuffer, 512, "%.33f\r\n"																	\
			"%.20e %.20e %.20e\r\n"																					\
			"%.8X %.8X %.8X \r\n",																					\
			dMinError,																								\
			fTest0, fTest1, fTest2,																					\
			iTest0, iTest1, iTest2 );																				\
		CStd::DebugPrintA( szBuffer );																				\
	}

	float fWinner0 = fTest0;
	float fWinner1 = fTest1;
	float fWinner2 = fTest2;
	double dMinError = EvalAtan( fTest0, fTest1, fTest2 );
	LST_PRINT_STUFF
	const int32_t uiRange0 = 10000;
	const int32_t uiRange1 = 10000;
	const int32_t uiRange2 = 10000;
	bool bChanged = true;
	while ( bChanged ) {
		bChanged = false;
		fTest0 = fWinner0;
		//iTest0 -= uiRange0;
		for ( int32_t I = 0; I <= uiRange0; ++I, ++iTest0 ) {
			fTest1 = fWinner1;
			iTest1 -= uiRange1;
			for ( int32_t J = -uiRange1; J <= uiRange1; ++J, ++iTest1 ) {
				fTest2 = fWinner2;
				iTest2 -= uiRange2;
				for ( int32_t K = -uiRange2; K <= uiRange2; ++K, ++iTest2 ) {
					double dThisError = EvalAtan( fTest0, fTest1, fTest2 );
					if ( dThisError < dMinError ) {
						dMinError = dThisError;
						fWinner0 = fTest0;
						fWinner1 = fTest1;
						fWinner2 = fTest2;
						LST_PRINT_STUFF
						bChanged = true;
					}
				}
			}
		}
	}
	fTest0 = fWinner0;
	fTest1 = fWinner1;
	fTest2 = fWinner2;

#if 0

	float fTemp = CMathLib::SinF_Medium( 1.5707963267948966192313216916398f );
	const uint32_t ui32Total = 1200;

	float fTemp0 = 1.0f / Factorial( 15 );
	float fTemp1 = float(1.0 / Factorial( 15 ));
	// Raw.				0.000880861073
	float d0 = EvalSin<float>( 1.0f, 
		1.0f / Factorial( 3 ),
		1.0f / Factorial( 5 ),
		1.0f / Factorial( 7 ),
		1.0f / Factorial( 9 ),
		1.0f / Factorial( 11 ),
		1.0f / Factorial( 13 ),
		1.0f / Factorial( 15 ),
		10.0f );
	// Gotanda.			0.000690596353
	float d1 = EvalSin<float>( 1.0f, 0.166666666450f, 0.00833333300997f, 0.000198412482578f, 2.75565569873e-6f, 2.50368483602e-8f, 1.58846760201e-10f, 6.57978461956e-13f, 10.0f );
	// Research paper.	0.000689232897
	float d2 = EvalSin<float>( 9.99999999958141380079e-01f,
		1.66666666451352167974e-01f,
		8.33333301181570639096e-03f,
		1.98412483604340805859e-04f,
		2.75565598752102704008e-06f,
		2.50368914392103083120e-08f,
		1.58850004791504823423e-10f,
		6.58075489175121657026e-13f,
		10.0f );
	// Mine.			0.000445976737
	float d3 = EvalSin<float>( 1.00000000000000000000e+00,
		1.66666671633720397949e-01,
		8.33333376795053482056e-03,
		1.98412497411482036114e-04,
		2.75565548690792638808e-06,
		2.50369236454162091832e-08,
		1.58850002596189199267e-10,
		6.58075482128095412548e-13,
		10.0f );
	// Mine.			0.000447012135
	float d4 = EvalSin<float>( 1.00000000000000000e+00f,
		1.66666671633720398e-01f,
		8.33333376795053482e-03f,
		1.98412497411482036e-04f,
		2.75565616902895272e-06f,
		2.50369147636320122e-08f,
		1.58850002596189199e-10f,
		6.58075482128095413e-13f,
		10.0f );
	// Mine.			0.000445423997
	float d5 = EvalSin<float>( 1.00000000000000000000e+00,
		1.66666671633720397949e-01,
		8.33333376795053482056e-03,
		1.98412497411482036114e-04,
		2.75565571428160183132e-06,
		2.50368472620721149724e-08,
		1.58849267073435385100e-10,
		6.58925550841432672300e-13,
		10.0f );

	double d00 = EvalSin<double>( 9.99999999958141594369e-01,	// Mine.
		1.66666666451352446510e-01,
		8.33333301181456848061e-03,
		1.98412483604294194060e-04,
		2.75565598751558709709e-06,
		2.50368914257412183582e-08,
		1.58850007480674172780e-10,
		6.58076002675210620158e-13,
		10.0f );
	double d01 = EvalSin<double>( 9.99999999958141380079e-01,	// Research paper.
		1.66666666451352167974e-01,
		8.33333301181570639096e-03,
		1.98412483604340805859e-04,
		2.75565598752102704008e-06,
		2.50368914392103083120e-08,
		1.58850004791504823423e-10,
		6.58075489175121657026e-13,
		10.0f );

	// Unreal.
	// Average = 0.000011496102666666666666666666666667
	// Max = 0.000187166996.
	// Goes above 0.
	float fMax11_0 = 10000.0f;
	float fSin11_0 = EvalSin_11<float>( 1.0,
		0.16666667,
		0.0083333310,
		0.00019840874,
		2.7525562e-06,
		2.3889859e-08,
		10000.0,
		fMax11_0 );
	// Research paper.
	// Average = 0.000000086373766666666666666666666666667
	// Max = 0.000000879168510.
	float fMax11_1 = 10000.0f;
	float fSin11_1 = EvalSin_11<float>( 9.99999707044156546685e-01,
		1.66665772196961623983e-01,
		8.33255814755188010464e-03,
		1.98125763417806681909e-04,
		2.70405218307799040084e-06,
		2.05342856289746600727e-08,
		10000.0,
		fMax11_1 );
	// Mine.
	// Average = 0.000000071991304583333333333333333333333.
	// Max = 0.000000543892384.
	float fMax11_2 = 10000.0f;
	float fSin11_2 = EvalSin_11<float>( 9.99999701976776123047e-01,
		1.66665777564048767090e-01,
		8.33255797624588012695e-03,
		1.98125766473822295666e-04,
		2.70405212177138309926e-06,
		2.05329886426852681325e-08,
		10000.0,
		fMax11_2 );



	// Unreal.
	// Average = 0.00010072299666666666666666666666667.
	// Max = 0.000762462616.
	float fMax10_0 = 10000.0f;
	volatile float fCos10_0 = EvalCos_10<float>( 1.0,
		0.5,
		0.041666638,
		0.0013888378,
		2.4760495e-05,
		2.6051615e-07,
		10000.0,
		fMax10_0 );
	// Research paper.
	// Average = 0.00000084569900833333333333333333333333.
	// Max = 0.00000244379044.
	float fMax10_1 = 10000.0f;
	volatile float fCos10_1 = EvalCos_10<float>( 9.99999443739537210853e-1,
		4.99995582499065048420e-01,
		4.16610337354021107429e-02,
		1.38627507062573673756e-03,
		2.42532401381033027481e-05,
		2.21941782786353727022e-07,
		10000.0,
		fMax10_1 );
	// Mine.
	// Average = 0.00000084443936666666666666666666666667.
	// Max = 0.00000244379044.
	float fMax10_2 = 10000.0f;
	volatile float fCos11_2 = EvalCos_10<float>( 9.99999463558197021484e-01,
		4.99995589256286621094e-01,
		4.16610352694988250732e-02,
		1.38627504929900169373e-03,
		2.42532332777045667171e-05,
		2.21941789391166821588e-07,
		10000.0,
		fMax10_2 );


	// 0.000482872245
	float dC0 = EvalCos<float>( 9.99999999919365479957e-01,	// Research paper.
		4.99999998886526927002e-01,
		4.16666641590361985136e-02,
		1.38888674687691339750e-03,
		2.48006913718665260256e-05,
		2.75369918573799545860e-07,
		2.06207503915813519567e-09,
		9.77507131527006498114e-12, 10.0 );
	// 0.000479504582
	float dC1 = EvalCos<float>( 1.00000000000000000000e+00,	// Mine.
		5.00000000000000000000e-01,
		4.16666641831398010254e-02,
		1.38888671062886714935e-03,
		2.48006890615215525031e-05,
		2.75369927749125054106e-07,
		2.06207229069832465029e-09,
		9.77507137733812925262e-12, 10.0 );
	
	/*float fTest0 = 1.0f / Factorial( 3 );
	float fTest1 = 1.0f / Factorial( 5 );
	float fTest2 = 1.0f / Factorial( 7 );
	float fTest3 = 1.0f / Factorial( 9 );
	float fTest4 = 1.0f / Factorial( 11 );
	float fTest5 = 1.0f / Factorial( 13 );
	float fTest6 = 1.0f / Factorial( 15 );*/
	/*float fTest0 = 0.499999998879;
	float fTest1 = 0.0416666641446;
	float fTest2 = 0.00138888673663;
	float fTest3 = 0.0000248006879614;
	float fTest4 = 2.75369342835e-7;
	float fTest5 = 2.06202732618e-9;
	float fTest6 = 9.77353119581e-12;*/
#define LST_FT		float
#define LST_IT		int32_t
//#define LST_PT		"%.8llX "
#define LST_PT		"%.8X "
#define LST_FUNC	EvalCos_10
	LST_FT fTest0 = 9.99999463558197021484e-01;
	LST_FT fTest1 = 4.99995589256286621094e-01;
	LST_FT fTest2 = 4.16610352694988250732e-02;
	LST_FT fTest3 = 1.38627504929900169373e-03;
	LST_FT fTest4 = 2.42532405536621809006e-05;
	LST_FT fTest5 = 2.21917616727296262980e-07;
	LST_FT fTest6 = 1.58850007480674172780e-10;
	LST_FT fTest7 = 6.58076002675210620158e-13;
	/*LST_FT fTest0 = 1.00000000000000000000e+00;
	LST_FT fTest1 = 1.66666671633720397949e-01;
	LST_FT fTest2 = 8.33333376795053482056e-03;
	LST_FT fTest3 = 1.98412497411482036114e-04;
	LST_FT fTest4 = 2.75565571428160183132e-06;
	LST_FT fTest5 = 2.50368472620721149724e-08;
	LST_FT fTest6 = 1.58849267073435385100e-10;
	LST_FT fTest7 = 6.58925550841432672300e-13;*/
	LST_IT & iTest0 = (*reinterpret_cast<LST_IT *>(&fTest0));
	LST_IT & iTest1 = (*reinterpret_cast<LST_IT *>(&fTest1));
	LST_IT & iTest2 = (*reinterpret_cast<LST_IT *>(&fTest2));
	LST_IT & iTest3 = (*reinterpret_cast<LST_IT *>(&fTest3));
	LST_IT & iTest4 = (*reinterpret_cast<LST_IT *>(&fTest4));
	LST_IT & iTest5 = (*reinterpret_cast<LST_IT *>(&fTest5));
	LST_IT & iTest6 = (*reinterpret_cast<LST_IT *>(&fTest6));
	LST_IT & iTest7 = (*reinterpret_cast<LST_IT *>(&fTest7));

	const int32_t iRange = 1024 * 13;
	LST_FT fWinner = fTest0;
	LST_FT fMax = 1000000;
	LST_FT fScore = LST_FUNC<LST_FT>( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, 1000.0f, fMax );
	
#define LST_PRINT_STUFF																								\
	{																												\
		char szBuffer[512];																						\
		CStd::SPrintF( szBuffer, 512, "%.33f %.33f\r\n"																\
			"%.20e %.20e %.20e %.20e %.20e %.20e\r\n"																\
			LST_PT LST_PT LST_PT LST_PT LST_PT LST_PT "\r\n",														\
			fScore,	fMax,																							\
			fTest0, fTest1, fTest2, fTest3, fTest4, fTest5,															\
			iTest0, iTest1, iTest2, iTest3, iTest4, iTest5 );														\
		CStd::DebugPrintA( szBuffer );																				\
	}

	LST_PRINT_STUFF

#define DO_TEST( NUM, TOTAL )		fWinner = fTest ## NUM;															\
	fMax = 1000000;																									\
	fScore = LST_FUNC<LST_FT>( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, 1000.0, fMax );						\
	iTest ## NUM -= (TOTAL);																						\
	for ( uint64_t I = 0; I <= (TOTAL) * 2; I += 1 ) {																\
		LST_FT fThis = LST_FUNC<LST_FT>( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fScore, fMax );			\
		if ( fThis < fScore ) {																						\
			fScore = fThis;																							\
			fWinner = fTest ## NUM;																					\
			bImproved = true;																						\
			LST_PRINT_STUFF																							\
		}																											\
		iTest ## NUM += 1;																							\
	}																												\
	fTest ## NUM = fWinner


#define DO_TEST2( NUM )																													\
	{																																	\
		uint32_t ui32Add = 1024 * 16;																									\
		LST_FT fScore = LST_FUNC<LST_FT>( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6, fTest7, 10000000000.0f );				\
		LST_FT fWinner = fTest ## NUM;																									\
		fTest ## NUM *= 1.0f;																											\
		while ( ui32Add ) {																												\
			LST_FT fThis = LST_FUNC<LST_FT>( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6, fTest7, fScore );					\
			if ( fThis > fScore ) {																										\
				iTest ## NUM -= ui32Add;																								\
				ui32Add >>= 1;																											\
				continue;																												\
			}																															\
			fWinner = fTest ## NUM;																										\
			fScore = fThis;																												\
			iTest ## NUM += ui32Add;																									\
		}																																\
	}

	/*DO_TEST2( 0 )
	DO_TEST2( 1 )
	DO_TEST2( 2 )
	DO_TEST2( 3 )
	DO_TEST2( 4 )
	DO_TEST2( 5 )
	DO_TEST2( 6 )
	DO_TEST2( 7 )*/

	LSBOOL bImproved = true;
	while ( bImproved ) {
		bImproved = false;
		DO_TEST( 5, 100000000 );
		DO_TEST( 4, 100000000 );
		DO_TEST( 3, 100000000 );
		DO_TEST( 2, 100000000 );
		DO_TEST( 1, 100000000 );
		DO_TEST( 0, 100000000 );
	}


	LST_PRINT_STUFF

	/*float fWinner = fTest0;
	float fScore = EvalSin( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6 );
	iTest0 -= iRange;
	for ( uint32_t I = 0; I <= iRange * 2; ++I ) {
		float fThis = EvalSin( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6 );
		if ( fThis < fScore ) {
			fScore = fThis;
			fWinner = fTest0;
		}
		iTest0 += 1;
	}
	fTest0 = fWinner;

	float fWinner1 = fTest1;
	fScore = EvalSin( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6 );
	iTest1 -= iRange;
	for ( uint32_t I = 0; I <= iRange * 2; ++I ) {
		float fThis = EvalSin( fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6 );
		if ( fThis < fScore ) {
			fScore = fThis;
			fWinner1 = fTest1;
		}
		iTest1 += 1;
	}*/

	
	/*for ( uint32_t I = 0; I <= ui32Total; ++I ) {
		double dI = I * (6.283185307179586476925286766559 / ui32Total) - 3.1415926535897932384626433832795;
		double dSin = ::sin( dI );
		double dSinAcc = SinVariable( (float)dI, fTest0, fTest1, fTest2, fTest3, fTest4, fTest5, fTest6 );//Sin( dI );
		double dSinMath = CMathLib::Sin( dI );
		double dDif = dSin - dSinAcc;
		double dDif2 = dSin - dSinMath;
		char szBuffer[256];
		CStd::SPrintF( szBuffer, 256, "%.23f %.23f, %.23f %.23f \r\n", dSin, dSinAcc, ::fabs( dDif ), ::fabs( dDif2 ) );
		CStd::DebugPrintA( szBuffer );
	}*/


	const uint32_t ui32Total = 5000;
    const double dLow = -3.1415926535897932384626433832795 * 2.0;
    const double dHigh = 3.1415926535897932384626433832795 * 2.0;
    const double dH = (dHigh - dLow) / ui32Total;
	{
		double dError = 0.0;
		double dMaxError = 0.0;
		double dMaxVal = 0.0;
		uint64_t ui64StartSin = tTimer.GetRealTime();
		char szBuffBuff[512];
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			double dI = dLow + I * dH;
			double dSinAcc = SinF_Medium( dI );
			::sprintf( szBuffBuff, "%.250f", DBL_MAX );
			dError += dSinAcc;
		}
		ui64StartSin = tTimer.GetRealTime() - ui64StartSin;
		CString sPrintMe = CString::CreateFromUI64( ui64StartSin ) + CString( " " ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		CStd::DebugPrintA( szBuffBuff );
		CStd::DebugPrintA( "\r\n" );
	}
	{
		double dError = 0.0;
		double dMaxError = 0.0;
		double dMaxVal = 0.0;
		uint64_t ui64StartSin = tTimer.GetRealTime();
		char szBuffBuff[512];
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			double dI = dLow + I * dH;
			double dSinAcc = SinF_Medium( dI );
			//::sprintf( szBuffBuff, "%.250f", dSinAcc );
			CStd::PrintFloat64( szBuffBuff, 400, DBL_MAX );
			dError += dSinAcc;
		}
		ui64StartSin = tTimer.GetRealTime() - ui64StartSin;
		CString sPrintMe = CString::CreateFromUI64( ui64StartSin ) + CString( " " ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		CStd::DebugPrintA( szBuffBuff );
		CStd::DebugPrintA( "\r\n" );
	}
#endif // 0

	/*uint32_t _ui32Width, _ui32Height;
	LSI_PIXEL_FORMAT _pfFormat;
	CImageLib::CTexelBuffer _tbReturn;
	CBmp::LoadBmp( "F:\\My Projects\\LSEngine\\LSTest\\Data\\5eslrt.bmp",
		_ui32Width, _ui32Height, _pfFormat,
		_tbReturn );*/
	CMatrix4x4 mLeft, mRight;
	mLeft.MatrixRotationZXY( 0.1f, 0.2f, 1.1f );
	mRight.MatrixRotationZXY( 0.1f, 0.2f, 1.1f );
	CMatrix4x4 mRes = mLeft * mRight;
	CVector4 vVec = CMatrix4x4::MultiplyVec4( mRes, CVector4( 0.2f, 3.5f, -0.4f, 12.03f ) );
	
#ifdef __ARM_NEON__
	//GLKMatrix4 mResArm = GLKMatrix4Multiply( (*(GLKMatrix4 *)&mLeft), (*(GLKMatrix4 *)&mRight) );
	CMatrix4x4_ARM mResArm = (*(CMatrix4x4_ARM *)&mLeft) * (*(CMatrix4x4_ARM *)&mRight);
	CMatrix4x4 mRes2 = (*(CMatrix4x4 *)&mResArm);
#endif	// #ifdef __ARM_NEON__

#ifdef LSE_X64
	CMatrix4x4_SSE3 mResArm = (*(CMatrix4x4_SSE3 *)&mLeft) * (*(CMatrix4x4_SSE3 *)&mRight);
	CMatrix4x4 mRes2 = (*(CMatrix4x4 *)&mResArm);
	CVector4 vVec2 = CMatrix4x4_SSE3::MultiplyVec4( mResArm, CVector4( 0.2f, 3.5f, -0.4f, 12.03f ) );
	
	
//	mRes.Transpose();
//	mRes2.Transpose();
	/*char szBuffer[2048];
	char * pcP = szBuffer;
	for ( uint32_t Y = 0; Y < 4; ++Y ) {
		for ( uint32_t X = 0; X < 4; ++X ) {
			pcP += ::sprintf( pcP, "%.8f %.8f \r\n", mRes[Y][X], mRes2[Y][X] );
		}
	}
	CStd::DebugPrintA( szBuffer );
	volatile int gjhg = 0;*/
#endif	// #ifdef LSE_X64
	
#ifdef __ARM_NEON__
	{
		// CVector4() * Matrix4x4.
		CStd::DebugPrintA( "Testing Matrix * Vector.\r\n" );
		const uint32_t ui32Total = 333000;
		CVector<CVector4, uint32_t> vSrc;
		CVector<CVector4, uint32_t> vDst;
		vSrc.Resize( ui32Total );
		vDst.Resize( ui32Total );
		CRand rRand;
		for ( uint32_t I = ui32Total; I--; ) {
			vSrc[I] = CVector4( rRand.RandReal( -1.0f, 1.0f ), rRand.RandReal( -1.0f, 1.0f ), rRand.RandReal( -1.0f, 1.0f ), 1.0f );
		}
		CMatrix4x4 mMatrix;
		mMatrix.MatrixRotationZXY( 0.1f, 0.2f, 1.1f );
		CMatrix4x4_ARM mArm = (*reinterpret_cast<CMatrix4x4_ARM *>(&mMatrix));
		uint64_t ui64Time = tTimer.GetRealTime();
		CMatrix4x4::MultiplyVec4Batch( mMatrix, &vSrc[0], ui32Total, &vDst[0] );
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		ui64Time = tTimer.GetRealTime();
		/*
		::memcpy( &vDst[0], &vSrc[0], ui32Total * sizeof( CVector4 ) );
		GLKMatrix4MultiplyVector4Array( (*reinterpret_cast<GLKMatrix4 *>(&mMatrix)), reinterpret_cast<GLKVector4 *>(&vDst[0]), ui32Total );
		*/
		CMatrix4x4_ARM::MultiplyVec4Batch( mArm, &vSrc[0], ui32Total, &vDst[0] );
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;
		
		for ( uint32_t I = ui32Total; I--; ) {
			if ( vSrc[I].x != vDst[I].x && vSrc[I].x == 0.0f ) {
				::printf( "Dif: %f %f %f %u", vSrc[I].x, vDst[I].x, vSrc[I].x - vDst[I].x, I );
			}
		}

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "C++ beats INTRINSICS.\r\n" );
		}
		else {
			CStd::DebugPrintA( "INTRINSICS beats C++.\r\n" );
		}
	}
#endif	// #ifdef __ARM_NEON__

#if 0
	{
		// CVector4() * Matrix4x4.
		CStd::DebugPrintA( "Testing Matrix * Vector.\r\n" );
		const uint32_t ui32Total = 333300;
		CVector<CVector4, uint32_t> vSrc;
		CVector<CVector4, uint32_t> vDst;
		vSrc.Resize( ui32Total );
		vDst.Resize( ui32Total );
		CRand rRand;
		for ( uint32_t I = ui32Total; I--; ) {
			vSrc[I] = CVector4( rRand.RandReal( -1.0f, 1.0f ), rRand.RandReal( -1.0f, 1.0f ), rRand.RandReal( -1.0f, 1.0f ), 1.0f );
		}
		CMatrix4x4 mMatrix;
		mMatrix.MatrixRotationZXY( 0.1f, 0.2f, 1.1f );
		uint64_t ui64Time = tTimer.GetRealTime();
		CMatrix4x4::MultiplyVec4Batch( mMatrix, &vSrc[0], ui32Total, &vDst[0] );
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		ui64Time = tTimer.GetRealTime();
		CMatrix4x4::MultiplyVec4BatchIntrinsics( mMatrix, &vSrc[0], ui32Total, &vDst[0] );
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "ASM beats INTRINSICS.\r\n" );
		}
		else {
			CStd::DebugPrintA( "INTRINSICS beats ASM.\r\n" );
		}
	}
#endif	// #if 0
	struct LST_OBJ {
		double	dPos[3];
		double	dNormal[3];
		double	dColor[3];
		double	dUv[2][8];
	};

	// CStd::MemCmpF().
	CStd::DebugPrintA( "Testing CStd::MemCmpF().\r\n" );
	for ( uint32_t J = 0UL; J < 3; ++J ) {
		// Testing only small data.
		LST_OBJ oLeft, oRight;
		CStd::MemSet( &oLeft, 0, sizeof( LST_OBJ ) );
		CStd::MemSet( &oRight, 0, sizeof( LST_OBJ ) );

		uint32_t ui32Ret0 = 0;
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 89000000UL; I-- > 0UL; ) {
			ui32Ret0 += CStd::MemCmpF( &oLeft, &oRight, sizeof( LST_OBJ ) );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		uint32_t ui32Ret1 = 0;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 89000000UL; I-- > 0UL; ) {
			ui32Ret1 += ::memcmp( &oLeft, &oRight, sizeof( LST_OBJ ) ) == 0;
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;


		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString( "Same return? " ) + CString::CreateFromUI64( ui32Ret0 == ui32Ret1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CStd::MemCmpF() beats ::memcmp().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::memcmp() beats CStd::MemCmpF().\r\n" );
		}
	}


	// CMemCmpF::Cmp().
	CStd::DebugPrintA( "Testing CMemCmpF::Cmp().\r\n" );
	for ( uint32_t J = 0UL; J < 3; ++J ) {
		// Testing only small data.
		LST_OBJ oLeft, oRight;
		CStd::MemSet( &oLeft, 0, sizeof( LST_OBJ ) );
		CStd::MemSet( &oRight, 0, sizeof( LST_OBJ ) );

		uint32_t ui32Ret0 = 0;
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 89000000UL; I-- > 0UL; ) {
			ui32Ret0 += CMemCmpF::Cmp( &oLeft, &oRight, sizeof( LST_OBJ ) );
			++(*reinterpret_cast<uint32_t *>(&oLeft));
			++(*reinterpret_cast<uint32_t *>(&oRight));
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		uint32_t ui32Ret1 = 0;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 89000000UL; I-- > 0UL; ) {
			ui32Ret1 += ::memcmp( &oLeft, &oRight, sizeof( LST_OBJ ) ) == 0;
			++(*reinterpret_cast<uint32_t *>(&oLeft));
			++(*reinterpret_cast<uint32_t *>(&oRight));
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;


		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString( "Same return? " ) + CString::CreateFromUI64( ui32Ret0 == ui32Ret1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CMemCmpF::Cmp() beats ::memcmp().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::memcmp() beats CMemCmpF::Cmp().\r\n" );
		}
	}


	/*{
		
		for ( uint32_t K = 1UL; K--; ) {
			CVector<LST_OBJ, uint32_t> vMyVec;
			std::vector<LST_OBJ> vTheirVec;
			CRand rRand;
			rRand.Seed( static_cast<uint32_t>(tTimer.GetRealTime()) );
#define LST_VEC_INSERTS	2000
			const uint32_t ui32AllocSize = 10000 + LST_VEC_INSERTS;
			vMyVec.Allocate( ui32AllocSize );
			vTheirVec.reserve( ui32AllocSize );
			for ( uint32_t I = 100000; I--; ) {
				vMyVec.Push( LST_OBJ() );
				vTheirVec.push_back( LST_OBJ() );
			}

			// Perform worst-case insertions.
			uint64_t ui64Time = tTimer.GetRealTime();
			for ( uint32_t I = LST_VEC_INSERTS; I--; ) {
				vMyVec.Insert( LST_OBJ(), 0 );
			}
			uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

			// Perform worst-case insertions on std::vector.
			ui64Time = tTimer.GetRealTime();
			for ( uint32_t I = LST_VEC_INSERTS; I--; ) {
				vTheirVec.insert( vTheirVec.begin(), LST_OBJ() );
			}
			uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

			CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
				CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
				CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) +
				CString::CreateFromI64( vMyVec.Length() ) + CString( "\r\n" ) +
				CString::CreateFromI64( vTheirVec.size() ) + CString( "\r\n" );
			CStd::DebugPrintA( sPrintMe.CStr() );
			if ( ui64Total0 <= ui64Total1 ) {
				CStd::DebugPrintA( "CVector() beats std::vector().\r\n" );
			}
			else {
				CStd::DebugPrintA( "std::vector() beats CVector().\r\n" );
			}
		}
		CStd::DebugPrintA( "\r\n" );
	}*/

	/*{
		for ( uint32_t K = 14UL; K--; ) {
			CVectorPoD<uint32_t, uint32_t> vSortMe, vCopyMe;
			CRand rRand;
			rRand.Seed( static_cast<uint32_t>(tTimer.GetRealTime()) );
			while ( !vSortMe.Resize( rRand.RandUInt32( 4192UL ) + 32000UL ) ) {}
			for ( uint32_t I = vSortMe.Length(); I--; ) {
				vSortMe[I] = rRand.RandUInt32();
			}

			// Perform the sort on a copy.
			vCopyMe = vSortMe;
			uint64_t ui64Time = tTimer.GetRealTime();
			CSearch::QSort( &vCopyMe[0], vCopyMe.Length(), sizeof( uint32_t ), reinterpret_cast<CSearch::PfSortFunc>(SortFunc), NULL );
			uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

			// Perform the sort on a copy.
			vCopyMe = vSortMe;
			ui64Time = tTimer.GetRealTime();
			CSearch::MergeSort( &vCopyMe[0], vCopyMe.Length(), sizeof( uint32_t ), reinterpret_cast<CSearch::PfSortFunc>(SortFunc), NULL );
			uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

			CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
				CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
				CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
			CStd::DebugPrintA( sPrintMe.CStr() );
			if ( ui64Total0 <= ui64Total1 ) {
				CStd::DebugPrintA( "CSearch::QSort() beats CSearch::MergeSort().\r\n" );
			}
			else {
				CStd::DebugPrintA( "CSearch::MergeSort() beats CSearch::QSort().\r\n" );
			}
		}
		CStd::DebugPrintA( "\r\n" );
	}*/

	{
		for ( uint32_t K = 14UL; K--; ) {
			CVectorPoD<uint32_t, uint32_t> vSortMe, vCopyMe, vCheckMe;
			CRand rRand;
			rRand.Seed( static_cast<uint32_t>(tTimer.GetRealTime()) );
			while ( !vSortMe.Resize( rRand.RandUInt32( 4192UL ) + 32000UL ) ) {}
			for ( uint32_t I = vSortMe.Length(); I--; ) {
				vSortMe[I] = rRand.RandUInt32();
			}

			// Perform the sort on a copy.
			vCopyMe = vSortMe;
			uint64_t ui64Time = tTimer.GetRealTime();
			CSearch::QSort( &vCopyMe[0], vCopyMe.Length(), sizeof( uint32_t ), reinterpret_cast<CSearch::PfSortFunc>(SortFunc), NULL );
			uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

			// Perform the sort on a copy.
			vCheckMe = vCopyMe;
			vCopyMe = vSortMe;
			ui64Time = tTimer.GetRealTime();
			CAlgorithm::QSort( &vCopyMe[0], vCopyMe.Length() );
			uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;


			CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
				CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
				CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
			CStd::DebugPrintA( sPrintMe.CStr() );
			if ( ui64Total0 <= ui64Total1 ) {
				CStd::DebugPrintA( "CSearch::QSort() beats CAlgorithm::QSort().\r\n" );
			}
			else {
				CStd::DebugPrintA( "CAlgorithm::QSort() beats CSearch::QSort().\r\n" );
			}
		}
		CStd::DebugPrintA( "\r\n" );
	}

	{
		// CFixed() * (multiplication).
		CStd::DebugPrintA( "Testing CFixed().\r\n" );
		CFixed<int32_t, 10> fFixedResult = 0.0f;
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( volatile uint32_t I = 0xFF00000; I--; ) {
			fFixedResult *= I;
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		float fFloatResult = 0.0f;
		ui64Time = tTimer.GetRealTime();
		for ( volatile uint32_t I = 0xFF00000; I--; ) {
			fFloatResult *= I;
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFixedResult) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFloatResult) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CFixed() * beats float.\r\n" );
		}
		else {
			CStd::DebugPrintA( "float * beats CFixed().\r\n" );
		}
	}


	{
		// CFixed() / (division).
		CStd::DebugPrintA( "Testing CFixed().\r\n" );
		CFixed<int32_t, 10> fFixedResult = 0.0f;
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( int32_t I = 0xFF00000; I--; ) {
			fFixedResult /= (I + 1);
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		float fFloatResult = 0.0f;
		ui64Time = tTimer.GetRealTime();
		for ( int32_t I = 0xFF00000; I--; ) {
			fFloatResult /= (I + 1.0f);
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFixedResult) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFloatResult) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CFixed() / beats float.\r\n" );
		}
		else {
			CStd::DebugPrintA( "float / beats CFixed().\r\n" );
		}
	}

	CStd::DebugPrintA( "\r\n" );


	{
		// powf().
		CStd::DebugPrintA( "Testing std::powf().\r\n" );
		LSREAL fReal = static_cast<LSREAL>(-1000.0);
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0x1FF00000; I--; ) {
			fReal += std::powf( 3.1415932f, 2.1f );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		float fFloatResult = -1000.0f;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0x1FF00000; I--; ) {
			fFloatResult += std::powf( 3.1415932f, 60.1f );
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fReal) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFloatResult) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "std::powf() High beats std::powf() Low.\r\n" );
		}
		else {
			CStd::DebugPrintA( "std::powf() Low beats std::powf() High.\r\n" );
		}
	}

	CStd::DebugPrintA( "\r\n" );

	{
		// CMath::Sqrt().
		CStd::DebugPrintA( "Testing CMathLib::Sqrt().\r\n" );
		LSREAL fReal = static_cast<LSREAL>(1000.0);
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0xFF00000; I--; ) {
			fReal += CMathLib::Sqrt( static_cast<LSREAL>(I + 1) );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		float fFloatResult = 1000.0f;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0xFF00000; I--; ) {
			fFloatResult += ::sqrtf( static_cast<float>(I + 1) );
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fReal) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFloatResult) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CMathLib::Sqrt() beats ::sqrtf().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::sqrtf() beats CMathLib::Sqrt().\r\n" );
		}
	}

	{
		// CMath::Cos().
		CStd::DebugPrintA( "Testing CMathLib::Cos().\r\n" );
		LSREAL fReal = static_cast<LSREAL>(1000.0);
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0xFF00000; I--; ) {
			fReal += CMathLib::Cos( static_cast<float>(I) / static_cast<float>(0xFF00000) * LSM_PI );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		float fFloatResult = 1000.0f;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0xFF00000; I--; ) {
			fFloatResult += ::cosf( static_cast<float>(I) / static_cast<float>(0xFF00000) * LSM_PI );
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fReal) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFloatResult) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CMathLib::Cos() beats ::cosf().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::cosf() beats CMathLib::Cos().\r\n" );
		}
	}

	CStd::DebugPrintA( "\r\n" );



	{
		// CMath::Sin().
		CStd::DebugPrintA( "Testing CMathLib::Sin().\r\n" );
		LSREAL fReal = static_cast<LSREAL>(1000.0);
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0xFF00000; I--; ) {
			fReal += CMathLib::Sin( static_cast<float>(I) / static_cast<float>(0xFF00000) * LSM_PI );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		float fFloatResult = 1000.0f;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0xFF00000; I--; ) {
			fFloatResult += ::sinf( static_cast<float>(I) / static_cast<float>(0xFF00000) * LSM_PI );
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fReal) ) + CString( "\r\n" ) + 
			CString::CreateFromDouble( static_cast<double>(fFloatResult) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CMathLib::Sin() beats ::sinf().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::sinf() beats CMathLib::Sin().\r\n" );
		}
	}

	CStd::DebugPrintA( "\r\n" );

	// CStd::MemChr().
	CStd::DebugPrintA( "Testing CStd::MemChr().\r\n" );
	for ( uint32_t J = 0UL; J < 3; ++J ) {
		uint8_t * pui8Buffer = LSENEW uint8_t[8192];
		CStd::MemSet( pui8Buffer, 0, 8192 );
		pui8Buffer[8189+J] = 'A';

		void * pvRet0 = NULL;
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 890000UL; I-- > 0UL; ) {
			pvRet0 = CStd::MemChr( pui8Buffer, 'A', 8192 );
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;

		void * pvRet1 = NULL;
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 890000UL; I-- > 0UL; ) {
			pvRet1 = ::memchr( pui8Buffer, 'A', 8192 );
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;

		LSEDELETE [] pui8Buffer;

		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString( "Same return? " ) + CString::CreateFromUI64( pvRet0 == pvRet1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CStd::MemChr() beats ::memchr().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::memchr() beats CStd::MemChr().\r\n" );
		}
	}


	// CMemLib::MAlloc() and CMemLib::Free().
	CStd::DebugPrintA( "Testing CMemLib::MAlloc() and CMemLib::Free().\r\n" );

	for ( uint32_t J = 0UL; J < 3; ++J ) {
		// Generate random sizes and indices.
		uint32_t * pui32Size = LSENEW uint32_t[0x4000];
		uint32_t * pui32Index = LSENEW uint32_t[0x4000];
		uint8_t ** ppui8Pointers = LSENEW uint8_t *[0x4000];
		CRand rRand;
		rRand.Seed( static_cast<uint32_t>(tTimer.GetRealTime()) );
		for ( uint32_t I = 0x4000; I-- > 0UL; ) {
			pui32Size[I] = rRand.RandUInt32( 0x40000 ) + 0x4;
			pui32Index[I] = rRand.RandUInt32( 0x4000 );
			ppui8Pointers[I] = NULL;
		}

		// Allocate with CMemLib first.
		uint64_t ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0x4000; I-- > 0UL; ) {
			uint8_t * pui8New = static_cast<uint8_t *>(CMemLib::MReAlloc( ppui8Pointers[pui32Index[I&(0x4000-1UL)]], pui32Size[I&(0x4000-1UL)] ));
			if ( pui8New ) {
				ppui8Pointers[pui32Index[I&(0x4000-1UL)]] = pui8New;
			}
		}
		for ( uint32_t I = 0x4000; I-- > 0UL; ) {
			CMemLib::Free( ppui8Pointers[I] );
			ppui8Pointers[I] = NULL;
		}
		uint64_t ui64Total0 = tTimer.GetRealTime() - ui64Time;


		// Now the standard functions.
		ui64Time = tTimer.GetRealTime();
		for ( uint32_t I = 0x4000; I-- > 0UL; ) {
			uint8_t * pui8New = static_cast<uint8_t *>(::realloc( ppui8Pointers[pui32Index[I&(0x4000-1UL)]], pui32Size[I&(0x4000-1UL)] ));
			if ( pui8New ) {
				ppui8Pointers[pui32Index[I&(0x4000-1UL)]] = pui8New;
			}
		}
		for ( uint32_t I = 0x4000; I-- > 0UL; ) {
			::free( ppui8Pointers[I] );
			ppui8Pointers[I] = NULL;
		}
		uint64_t ui64Total1 = tTimer.GetRealTime() - ui64Time;


		LSEDELETE [] pui32Size;
		LSEDELETE [] pui32Index;
		LSEDELETE [] ppui8Pointers;


		CString sPrintMe = CString::CreateFromUI64( ui64Total0 ) + CString( " " ) + CString::CreateFromUI64( ui64Total1 ) +
			CString( "\r\nDifference: " ) + CString::CreateFromI64( ui64Total0 - ui64Total1 ) + CString( "\r\n" ) +
			CString::CreateFromDouble( static_cast<double>(ui64Total0) / static_cast<double>(ui64Total1) ) + CString( "\r\n" );
		CStd::DebugPrintA( sPrintMe.CStr() );
		if ( ui64Total0 <= ui64Total1 ) {
			CStd::DebugPrintA( "CMemLib::MAlloc() beats ::malloc().\r\n" );
		}
		else {
			CStd::DebugPrintA( "::malloc() beats CMemLib::MAlloc().\r\n" );
		}

		CMemLib::ReleaseEmptyPools();
	}



	
}

/**
 * Sort function.
 *
 * \param _pvContext Unused.
 * \param _pui32Left Left operand.
 * \param _pui32Right Left operand.
 * \return Returns the lexicographical comparison between the two objects.
 */
int32_t LSE_CCALL CBenchMark::SortFunc( void * /*_pvContext*/,
	const uint32_t * _pui32Left, const uint32_t * _pui32Right ) {
	if ( (*_pui32Left) < (*_pui32Right) ) { return -1; }
	if ( (*_pui32Left) == (*_pui32Right) ) { return 0; }
	return 1;
}
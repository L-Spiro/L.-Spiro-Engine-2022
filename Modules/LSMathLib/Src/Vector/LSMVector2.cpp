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
 * Description: A standard 2D vector of type LSREAL.
 */

#include "LSMVector2.h"


namespace lsm {

	const CVector2 CVector2::m_vPoisson25[25] = {
		CVector2( LSREAL( -0.978698 ), LSREAL( -0.0884121 ) ),
		CVector2( LSREAL( -0.841121 ), LSREAL( 0.521165 ) ),
		CVector2( LSREAL( -0.71746 ), LSREAL( -0.50322 ) ),
		CVector2( LSREAL( -0.702933 ), LSREAL( 0.903134 ) ),
		CVector2( LSREAL( -0.663198 ), LSREAL( 0.15482 ) ),
		CVector2( LSREAL( -0.495102 ), LSREAL( -0.232887 ) ),
		CVector2( LSREAL( -0.364238 ), LSREAL( -0.961791 ) ),
		CVector2( LSREAL( -0.345866 ), LSREAL( -0.564379 ) ),
		CVector2( LSREAL( -0.325663 ), LSREAL( 0.64037 ) ),
		CVector2( LSREAL( -0.182714 ), LSREAL( 0.321329 ) ),
		CVector2( LSREAL( -0.142613 ), LSREAL( -0.0227363 ) ),
		CVector2( LSREAL( -0.0564287 ), LSREAL( -0.36729 ) ),
		CVector2( LSREAL( -0.0185858 ), LSREAL( 0.918882 ) ),
		CVector2( LSREAL( 0.0381787 ), LSREAL( -0.728996 ) ),
		CVector2( LSREAL( 0.16599 ), LSREAL( 0.093112 ) ),
		CVector2( LSREAL( 0.253639 ), LSREAL( 0.719535 ) ),
		CVector2( LSREAL( 0.369549 ), LSREAL( -0.655019 ) ),
		CVector2( LSREAL( 0.423627 ), LSREAL( 0.429975 ) ),
		CVector2( LSREAL( 0.530747 ), LSREAL( -0.364971 ) ),
		CVector2( LSREAL( 0.566027 ), LSREAL( -0.940489 ) ),
		CVector2( LSREAL( 0.639332 ), LSREAL( 0.0284127 ) ),
		CVector2( LSREAL( 0.652089 ), LSREAL( 0.669668 ) ),
		CVector2( LSREAL( 0.773797 ), LSREAL( 0.345012 ) ),
		CVector2( LSREAL( 0.968871 ), LSREAL( 0.840449 ) ),
		CVector2( LSREAL( 0.991882 ), LSREAL( -0.657338 ) ),
	};

	/**
	 * 32-point Poisson filter from -1 to 1 on both axes.
	 */
	const CVector2 CVector2::m_vPoisson32[32] = {
		CVector2( LSREAL( -0.975402 ), LSREAL( -0.0711386 ) ),
		CVector2( LSREAL( -0.920347 ), LSREAL( -0.41142 ) ),
		CVector2( LSREAL( -0.883908 ), LSREAL( 0.217872 ) ),
		CVector2( LSREAL( -0.884518 ), LSREAL( 0.568041 ) ),
		CVector2( LSREAL( -0.811945 ), LSREAL( 0.90521 ) ),
		CVector2( LSREAL( -0.792474 ), LSREAL( -0.779962 ) ),
		CVector2( LSREAL( -0.614856 ), LSREAL( 0.386578 ) ),
		CVector2( LSREAL( -0.580859 ), LSREAL( -0.208777 ) ),
		CVector2( LSREAL( -0.53795 ), LSREAL( 0.716666 ) ),
		CVector2( LSREAL( -0.515427 ), LSREAL( 0.0899991 ) ),
		CVector2( LSREAL( -0.454634 ), LSREAL( -0.707938 ) ),
		CVector2( LSREAL( -0.420942 ), LSREAL( 0.991272 ) ),
		CVector2( LSREAL( -0.261147 ), LSREAL( 0.588488 ) ),
		CVector2( LSREAL( -0.211219 ), LSREAL( 0.114841 ) ),
		CVector2( LSREAL( -0.146336 ), LSREAL( -0.259194 ) ),
		CVector2( LSREAL( -0.139439 ), LSREAL( -0.888668 ) ),
		CVector2( LSREAL( 0.0116886 ), LSREAL( 0.326395 ) ),
		CVector2( LSREAL( 0.0380566 ), LSREAL( 0.625477 ) ),
		CVector2( LSREAL( 0.0625935 ), LSREAL( -0.50853 ) ),
		CVector2( LSREAL( 0.125584 ), LSREAL( 0.0469069 ) ),
		CVector2( LSREAL( 0.169469 ), LSREAL( -0.997253 ) ),
		CVector2( LSREAL( 0.320597 ), LSREAL( 0.291055 ) ),
		CVector2( LSREAL( 0.359172 ), LSREAL( -0.633717 ) ),
		CVector2( LSREAL( 0.435713 ), LSREAL( -0.250832 ) ),
		CVector2( LSREAL( 0.507797 ), LSREAL( -0.916562 ) ),
		CVector2( LSREAL( 0.545763 ), LSREAL( 0.730216 ) ),
		CVector2( LSREAL( 0.56859 ), LSREAL( 0.11655 ) ),
		CVector2( LSREAL( 0.743156 ), LSREAL( -0.505173 ) ),
		CVector2( LSREAL( 0.736442 ), LSREAL( -0.189734 ) ),
		CVector2( LSREAL( 0.843562 ), LSREAL( 0.357036 ) ),
		CVector2( LSREAL( 0.865413 ), LSREAL( 0.763726 ) ),
		CVector2( LSREAL( 0.872005 ), LSREAL( -0.927 ) ),
	};

	/**
	 * 64-point Poisson filter from -1 to 1 on both axes.
	 */
	const CVector2 CVector2::m_vPoisson64[64] = {
		CVector2( LSREAL( -0.934812 ), LSREAL( 0.366741 ) ),
		CVector2( LSREAL( -0.918943 ), LSREAL( -0.0941496 ) ),
		CVector2( LSREAL( -0.873226 ), LSREAL( 0.62389 ) ),
		CVector2( LSREAL( -0.8352 ), LSREAL( 0.937803 ) ),
		CVector2( LSREAL( -0.822138 ), LSREAL( -0.281655 ) ),
		CVector2( LSREAL( -0.812983 ), LSREAL( 0.10416 ) ),
		CVector2( LSREAL( -0.786126 ), LSREAL( -0.767632 ) ),
		CVector2( LSREAL( -0.739494 ), LSREAL( -0.535813 ) ),
		CVector2( LSREAL( -0.681692 ), LSREAL( 0.284707 ) ),
		CVector2( LSREAL( -0.61742 ), LSREAL( -0.234535 ) ),
		CVector2( LSREAL( -0.601184 ), LSREAL( 0.562426 ) ),
		CVector2( LSREAL( -0.607105 ), LSREAL( 0.847591 ) ),
		CVector2( LSREAL( -0.581835 ), LSREAL( -0.00485244 ) ),
		CVector2( LSREAL( -0.554247 ), LSREAL( -0.771111 ) ),
		CVector2( LSREAL( -0.483383 ), LSREAL( -0.976928 ) ),
		CVector2( LSREAL( -0.476669 ), LSREAL( -0.395672 ) ),
		CVector2( LSREAL( -0.439802 ), LSREAL( 0.362407 ) ),
		CVector2( LSREAL( -0.409772 ), LSREAL( -0.175695 ) ),
		CVector2( LSREAL( -0.367534 ), LSREAL( 0.102451 ) ),
		CVector2( LSREAL( -0.35313 ), LSREAL( 0.58153 ) ),
		CVector2( LSREAL( -0.341594 ), LSREAL( -0.737541 ) ),
		CVector2( LSREAL( -0.275979 ), LSREAL( 0.981567 ) ),
		CVector2( LSREAL( -0.230811 ), LSREAL( 0.305094 ) ),
		CVector2( LSREAL( -0.221656 ), LSREAL( 0.751152 ) ),
		CVector2( LSREAL( -0.214393 ), LSREAL( -0.0592364 ) ),
		CVector2( LSREAL( -0.204932 ), LSREAL( -0.483566 ) ),
		CVector2( LSREAL( -0.183569 ), LSREAL( -0.266274 ) ),
		CVector2( LSREAL( -0.123936 ), LSREAL( -0.754448 ) ),
		CVector2( LSREAL( -0.0859096 ), LSREAL( 0.118625 ) ),
		CVector2( LSREAL( -0.0610675 ), LSREAL( 0.460555 ) ),
		CVector2( LSREAL( -0.0234687 ), LSREAL( -0.962523 ) ),
		CVector2( LSREAL( -0.00485244 ), LSREAL( -0.373394 ) ),
		CVector2( LSREAL( 0.0213324 ), LSREAL( 0.760247 ) ),
		CVector2( LSREAL( 0.0359813 ), LSREAL( -0.0834071 ) ),
		CVector2( LSREAL( 0.0877407 ), LSREAL( -0.730766 ) ),
		CVector2( LSREAL( 0.14597 ), LSREAL( 0.281045 ) ),
		CVector2( LSREAL( 0.18186 ), LSREAL( -0.529649 ) ),
		CVector2( LSREAL( 0.188208 ), LSREAL( -0.289529 ) ),
		CVector2( LSREAL( 0.212928 ), LSREAL( 0.063509 ) ),
		CVector2( LSREAL( 0.23661 ), LSREAL( 0.566027 ) ),
		CVector2( LSREAL( 0.266579 ), LSREAL( 0.867061 ) ),
		CVector2( LSREAL( 0.320597 ), LSREAL( -0.883358 ) ),
		CVector2( LSREAL( 0.353557 ), LSREAL( 0.322733 ) ),
		CVector2( LSREAL( 0.404157 ), LSREAL( -0.651479 ) ),
		CVector2( LSREAL( 0.410443 ), LSREAL( -0.413068 ) ),
		CVector2( LSREAL( 0.413556 ), LSREAL( 0.123325 ) ),
		CVector2( LSREAL( 0.46556 ), LSREAL( -0.176183 ) ),
		CVector2( LSREAL( 0.49266 ), LSREAL( 0.55388 ) ),
		CVector2( LSREAL( 0.506333 ), LSREAL( 0.876888 ) ),
		CVector2( LSREAL( 0.535875 ), LSREAL( -0.885556 ) ),
		CVector2( LSREAL( 0.615894 ), LSREAL( 0.0703452 ) ),
		CVector2( LSREAL( 0.637135 ), LSREAL( -0.637623 ) ),
		CVector2( LSREAL( 0.677236 ), LSREAL( -0.174291 ) ),
		CVector2( LSREAL( 0.67626 ), LSREAL( 0.7116 ) ),
		CVector2( LSREAL( 0.686331 ), LSREAL( -0.389935 ) ),
		CVector2( LSREAL( 0.691031 ), LSREAL( 0.330729 ) ),
		CVector2( LSREAL( 0.715629 ), LSREAL( 0.999939 ) ),
		CVector2( LSREAL( 0.8493 ), LSREAL( -0.0485549 ) ),
		CVector2( LSREAL( 0.863582 ), LSREAL( -0.85229 ) ),
		CVector2( LSREAL( 0.890622 ), LSREAL( 0.850581 ) ),
		CVector2( LSREAL( 0.898068 ), LSREAL( 0.633778 ) ),
		CVector2( LSREAL( 0.92053 ), LSREAL( -0.355693 ) ),
		CVector2( LSREAL( 0.933348 ), LSREAL( -0.62981 ) ),
		CVector2( LSREAL( 0.95294 ), LSREAL( 0.156896 ) ),
	};

}	// namespace lsm
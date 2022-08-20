/**
 * Copyright L. Spiro 2012
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A layer of animations is a collection of tracks.
 */

#include "LSXAnimationLayer.h"


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CAnimationLayer::CAnimationLayer() {
	}

	// == Functions.
	/**
	 * Loads an animation layer from an FBX animation layer and a node.  Internally called
	 *	recursively.
	 *
	 * \param _pfasStack The animation layer to load.
	 * \param _pfnNode The node from which to load animation data.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CAnimationLayer::LoadLayer( FbxAnimLayer * _pfalLayer, FbxNode * _pfnNode ) {
		FbxAnimCurve * pfacCurve = NULL;
		m_sName = _pfalLayer->GetName();

		// Translation.
		pfacCurve = _pfnNode->LclTranslation.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_X );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_TRANS_X ) ) { return false; }
        }
		pfacCurve = _pfnNode->LclTranslation.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_Y );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_TRANS_Y ) ) { return false; }
        }
		pfacCurve = _pfnNode->LclTranslation.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_Z );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_TRANS_Z ) ) { return false; }
        }

		// Rotation.
		pfacCurve = _pfnNode->LclRotation.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_X );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_ROT_X ) ) { return false; }
        }
		pfacCurve = _pfnNode->LclRotation.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_Y );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_ROT_Y ) ) { return false; }
        }
		pfacCurve = _pfnNode->LclRotation.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_Z );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_ROT_Z ) ) { return false; }
        }

		// Scale.
		pfacCurve = _pfnNode->LclScaling.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_X );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_SCALE_X ) ) { return false; }
        }
		pfacCurve = _pfnNode->LclScaling.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_Y );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_SCALE_Y ) ) { return false; }
        }
		pfacCurve = _pfnNode->LclScaling.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COMPONENT_Z );
		if ( pfacCurve ) {
			if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
			if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LOCAL_TRANSFORM | LSD_AA_SCALE_Z ) ) { return false; }
        }


		FbxNodeAttribute * pfnaAttribute = _pfnNode->GetNodeAttribute();
		if ( pfnaAttribute ) {
			// Color.
			pfacCurve = pfnaAttribute->Color.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COLOR_RED );
			if ( pfacCurve ) {
				if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
				if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_RED ) ) { return false; }
			}
			pfacCurve = pfnaAttribute->Color.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COLOR_GREEN );
			if ( pfacCurve ) {
				if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
				if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_GREEN ) ) { return false; }
			}
			pfacCurve = pfnaAttribute->Color.GetCurve( _pfalLayer, FBXSDK_CURVENODE_COLOR_BLUE );
			if ( pfacCurve ) {
				if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
				if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_BLUE ) ) { return false; }
			}

			// Lights.
			FbxLight * pflLight = _pfnNode->GetLight();
			if ( pflLight ) {
				pfacCurve = pflLight->Intensity.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LIGHT_INTENSITY ) ) { return false; }
				}
				pfacCurve = pflLight->OuterAngle.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LIGHT_CONEANGLE ) ) { return false; }
				}
				pfacCurve = pflLight->Fog.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_LIGHT_FOG ) ) { return false; }
				}
			}

			// Cameras.
			FbxCamera * pfcCamera = _pfnNode->GetCamera();
			if ( pfcCamera ) {
				pfacCurve = pfcCamera->FieldOfView.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_FIELDOFVIEW ) ) { return false; }
				}
				pfacCurve = pfcCamera->FieldOfViewX.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_FIELDOFVIEWX ) ) { return false; }
				}
				pfacCurve = pfcCamera->FieldOfViewY.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_FIELDOFVIEWY ) ) { return false; }
				}
				pfacCurve = pfcCamera->OpticalCenterX.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_OPTICALCENTERX ) ) { return false; }
				}
				pfacCurve = pfcCamera->OpticalCenterY.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_OPTICALCENTERY ) ) { return false; }
				}
				pfacCurve = pfcCamera->Roll.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_ROLL ) ) { return false; }
				}
				pfacCurve = pfcCamera->TurnTable.GetCurve( _pfalLayer );
				if ( pfacCurve ) {
					if ( !m_vTracks.Push( CAnimationTrack() ) ) { return false; }
					if ( !m_vTracks[m_vTracks.Length()-1UL].Load( pfacCurve, _pfnNode, LSD_AA_CAMERA_TURNTABLE ) ) { return false; }
				}
			}
		}

		for ( LSINT32 I = 0; I < _pfnNode->GetChildCount(); ++I ) {
			if ( !LoadLayer( _pfalLayer, _pfnNode->GetChild( I ) ) ) { return false; }
		}
		return true;
	}

	/**
	 * Save the data to a buffer.
	 *
	 * \param _bBuffer The buffer to which to save our data.
	 * \return Returns true only if all data is written to the buffer.
	 */
	LSBOOL LSE_CALL CAnimationLayer::SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		// Save the name of the layer.
		if ( !CMisc::WriteString( m_sName, _bBuffer ) ) { return false; }

		// Write the number of tracks.
		if ( !CMisc::WriteUInt( m_vTracks.Length(), _bBuffer ) ) { return false; }
		for ( LSUINT32 I = 0UL; I < m_vTracks.Length(); ++I ) {
			if ( !m_vTracks[I].SaveToBuffer( _bBuffer ) ) { return false; }
		}
		return true;
	}

}	// namespace lsx

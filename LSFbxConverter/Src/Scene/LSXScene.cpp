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
 * Description: An FBX scene.
 */

#include "LSXScene.h"
#include "../Misc/LSXMisc.h"
#include <iostream>


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CScene::CScene() :
		m_pfsmManager( NULL ),
		m_pfsScene( NULL ) {
		// Create the memory manager.
		m_pfsmManager = FbxManager::Create();

		// Create an FbxIOSettings object.
		FbxIOSettings * piosSettings = FbxIOSettings::Create( m_pfsmManager, IOSROOT );
		m_pfsmManager->SetIOSettings( piosSettings );
	}
	LSE_CALLCTOR CScene::~CScene() {
		Reset();
		// Destroy the memory manager.
		if ( m_pfsmManager ) {
			m_pfsmManager->Destroy();
			m_pfsmManager = NULL;
		}
	}

	// == Functions.
	/**
	 * Load a scene.  The path must be in UTF-8 format.  The existing file, if any, is unloaded.
	 *
	 * \param _pcPath The full path to the file to load, in UTF-8 format.
	 * \param _pcPassword The file password, if any.
	 * \param _ui32Options Options.
	 * \return Returns true if the file was loaded.
	 */
	LSBOOL CScene::LoadFile( const LSUTF8 * _pcPath, const LSCHAR * _pcPassword, LSUINT32 _ui32Options ) {
		Reset();	// Deletes all allocated resources except m_pfsmManager.  This includes m_pfsScene.

		if ( !m_pfsmManager ) { return false; }

		// Create a new scene.
		m_pfsScene = FbxScene::Create( m_pfsmManager, "" );
		if ( !m_pfsScene ) { return false; }

		// Create an importer.
		FbxImporter * pfiImporter = FbxImporter::Create( m_pfsmManager, "" );
		if ( !pfiImporter ) {
			Reset();
			return false;
		}

		// Attempt to load the file.
		LSBOOL bOpened = pfiImporter->Initialize( reinterpret_cast<const char *>(_pcPath), -1, m_pfsmManager->GetIOSettings() );
		if ( !bOpened ) {
			switch ( pfiImporter->GetStatus().GetCode() ) {
				case FbxStatus::eInvalidFileVersion : {
					// Get the version number of the FBX file format.
					int iFileMajor, iFileMinor, iFileRevision;
					pfiImporter->GetFileVersion( iFileMajor, iFileMinor, iFileRevision );
					::printf( "Error opening file \"%s\": Invalid version (%u.%u.%u).\r\n",
						_pcPath,
						iFileMajor, iFileMinor, iFileRevision );
					break;
				}
				default : {
					::printf( "Error opening file \"%s\": Unknown error.\r\n", _pcPath );
					break;
				}
			}
			pfiImporter->Destroy();

			Reset();
			return false;
		}

		if ( pfiImporter->IsFBX() ) {
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_MATERIAL, true );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_TEXTURE, true );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_LINK, true );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_SHAPE, true );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_GOBO, true );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_ANIMATION, true );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );
		}

		// Import into the scene.
		LSBOOL bImported = pfiImporter->Import( m_pfsScene );

		// If it failed due to password.
		if ( !bImported && pfiImporter->GetStatus().GetCode() == FbxStatus::ePasswordError ) {
			m_pfsmManager->GetIOSettings()->SetStringProp( IMP_FBX_PASSWORD, _pcPassword );
			m_pfsmManager->GetIOSettings()->SetBoolProp( IMP_FBX_PASSWORD_ENABLE, true );

			bImported = pfiImporter->Import( m_pfsScene );
			// If it failed again due to password then the given password is wrong.
			if ( !bImported && pfiImporter->GetStatus().GetCode() == FbxStatus::ePasswordError ) {
				::printf( "Error opening file \"%s\": Invalid password.\r\n", _pcPath );
			}

		}

		pfiImporter->Destroy();

		::printf( "Imported \"%s\".\r\n", _pcPath );

		return bImported;
	}

	/**
	 * Save the data to the given file.
	 *
	 * \param _pcPath File to which to save the scene data.
	 * \return Returns an error code indicating success or failure.
	 */
	LSSTD_ERRORS LSE_CALL CScene::SaveTo( const LSUTF8 * _pcPath ) {
		LSF_FILE fFile = CFileLib::OpenFile( reinterpret_cast<const LSCHAR *>(_pcPath), LSF_OF_WRITE, true );
		if ( !fFile ) {
			return LSSTD_E_FILENOTFOUND;
		}

#define return CFileLib::CloseFile( fFile ); return
		lsc::CMisc::LSC_BUFFER bBuffer;
		// Header.
		if ( !CMisc::WriteUInt( LSX_LSM_HEADER, bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }
		if ( !CMisc::WriteUInt( LSX_LSM_VERSION, bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }

		lsc::CMisc::LSC_BUFFER bTemp;
		LSUINT32 ui32Before = bBuffer.Length();
		LSUINT32 ui32TotalTextureSize = 0;
		// Total textures referenced.
		if ( !CMisc::WriteUInt( m_taTextures.Length(), bBuffer ) ) {
			return LSSTD_E_OUTOFMEMORY;
		}
		// Save each texture.
		for ( LSUINT32 I = 0; I < m_taTextures.Length(); ++I ) {
			if ( !m_taTextures[I].SaveTexture( bBuffer ) ) {
				return LSSTD_E_OUTOFMEMORY;
			}
			ui32TotalTextureSize += (bBuffer.Length() - ui32Before);
			if ( !CFileLib::WriteFile( fFile, &bBuffer[0], bBuffer.Length() ) ) {
				CFileLib::CloseFile( fFile );
				return LSSTD_E_NODISKSPACE;
			}
			bBuffer.ResetNoDealloc();

			ui32Before = bBuffer.Length();
		}
		::printf( "Texture data: %.13f megabytes.\r\n", ui32TotalTextureSize * (1.0f / 1024.0f / 1024.0f) );
		m_taTextures.Reset();	// Remove unused data.
		bBuffer.Reset();


		// Total materials used.
		if ( !CMisc::WriteUInt( m_vMaterials.Length(), bBuffer ) ) {
			return LSSTD_E_OUTOFMEMORY;
		}
		// Save each material.
		for ( LSUINT32 I = 0; I < m_vMaterials.Length(); ++I ) {
			if ( !m_vMaterials[I].SaveMaterial( bBuffer, m_trsReferencedTextures ) ) {
				return LSSTD_E_OUTOFMEMORY;
			}
		}


		// Unique meshes.
		LSUINT32 ui32Total = 0;
		for ( LSUINT32 I = 0; I < m_vMeshes.Length(); ++I ) {
			LSBOOL bAdded = false;
			for ( LSINT32 J = I; --J >= 0; ) {
				if ( m_vMeshes[I].GetMesh() == m_vMeshes[J].GetMesh() ) {
					// Already added.  Save its index.
					m_vMeshes[I].GetMesh().SetIndex( m_vMeshes[J].GetMesh().GetIndex() );
					bAdded = true;
					break;
				}
			}
			if ( !bAdded ) {
				if ( !m_vMeshes[I].GetMesh().SaveMesh( bTemp, ui32Total++ ) ) { return LSSTD_E_OUTOFMEMORY; }
			}
		}

		// Number of unique meshes.
		if ( !CMisc::WriteUInt( ui32Total, bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }
		// Pad to 16 bytes.
		//if ( !CMisc::Pad( bBuffer, 16 ) ) { return LSSTD_E_OUTOFMEMORY; }
		// Append meshes.
		if ( bTemp.Length() ) {
			if ( !CMisc::WriteBytes( &bTemp[0], bTemp.Length(), bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }
			bTemp.ResetNoDealloc();
		}


		// Number of mesh instances.
		if ( !CMisc::WriteUInt( m_vMeshes.Length(), bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }
		// Mesh instances.
		for ( LSUINT32 I = 0; I < m_vMeshes.Length(); ++I ) {
			if ( !m_vMeshes[I].SaveMeshInstance( bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }
		}
		//if ( !CMisc::Pad( bBuffer, 16 ) ) { return LSSTD_E_OUTOFMEMORY; }


		// Hierarchy.
		if ( !WriteNode( m_pfsScene->GetRootNode(), bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }
		//if ( !CMisc::Pad( bBuffer, 16 ) ) { return LSSTD_E_OUTOFMEMORY; }


		// Animations.
		if ( !m_aAnimation.SaveToBuffer( bBuffer ) ) { return LSSTD_E_OUTOFMEMORY; }

		
		if ( !CFileLib::WriteFile( fFile, &bBuffer[0], bBuffer.Length() ) ) {
			CFileLib::CloseFile( fFile );
			return LSSTD_E_NODISKSPACE;
		}

		CFileLib::CloseFile( fFile );

#undef return
		return LSSTD_E_SUCCESS;
	}

	/**
	 * Reset everything to scratch.
	 */
	LSVOID CScene::Reset() {
		m_trsReferencedTextures.Reset();
		m_trsReferencedNormalTextures.Reset();
		m_sReferencedMaterials.Reset();
		m_vMaterials.Reset();
		m_vMeshes.Reset();
		if ( m_pfsScene ) {
			m_pfsScene->Destroy();
			m_pfsScene = NULL;
		}
	}

	/**
	 * Loads mesh data.
	 *
	 * \return Returns true if mesh data was loaded.
	 */
	LSBOOL LSE_CALL CScene::LoadMeshes() {
		// Load the skeleton.
		if ( !m_sSkeleton.ExtractSkeleton( m_pfsScene ) ) { return false; }
		m_vMeshes.Reset();
		lsx::CMisc::LSX_NODE_ARRAY naMeshes;
		lsx::CMisc::GatherNodes( m_pfsScene->GetRootNode(), FbxNodeAttribute::eMesh, naMeshes, true );
		m_vMeshes.Resize( naMeshes.Length() );
		::printf( "Loading %u meshes.\r\n", m_vMeshes.Length() );
		LSUINT32 ui32TotalTriangles = 0;
		for ( LSUINT32 I = 0; I < m_vMeshes.Length(); ++I ) {
			if ( !m_vMeshes[I].CreateMeshInstance( naMeshes[I], m_pfsScene, m_sReferencedMaterials, m_sSkeleton ) ) { return false; }
			ui32TotalTriangles += m_vMeshes[I].GetMesh().Triangles();
		}
		::printf( "%u triangles.\r\n", ui32TotalTriangles );

		for ( LSUINT32 I = 0; I < m_vMeshes.Length(); ++I ) {
			for ( LSUINT32 J = I + 1; J < m_vMeshes.Length(); ++J ) {
				if ( m_vMeshes[I].GetMesh() == m_vMeshes[J].GetMesh() ) {
					::printf( "Matched %u %u.\r\n", I, J );
				}
			}
		}

		if ( !LoadMaterials() ) { return false; }
		//::printf( "Referenced %u textures.\r\n", m_trsReferencedTextures.Length() );

		if ( !m_aAnimation.LoadAnimations( m_pfsScene ) ) { return false; }

		return true;
	}

	/**
	 * Loads referenced materials.
	 *
	 * \return Returns true if material data was loaded.
	 */
	LSBOOL LSE_CALL CScene::LoadMaterials() {
		if ( !m_vMaterials.Resize( m_sReferencedMaterials.Length() ) ) { return false; }
		m_trsReferencedTextures.AllocateAtLeast( m_sReferencedMaterials.Length() );
		for ( LSUINT32 I = 0; I < m_sReferencedMaterials.Length(); ++I ) {
			if ( !m_vMaterials[I].LoadMaterial( m_sReferencedMaterials.GetByIndex( I ), m_trsReferencedTextures, m_trsReferencedNormalTextures ) ) {
				return false;
			}
		}
		LSSTD_ERRORS eError = FinalizeTextures();
		if ( eError != LSSTD_E_SUCCESS ) {
			CStd::PrintError( eError );
			return false;
		}
		return true;
	}

	/**
	 * Loads and finalizes all of the referenced textures.
	 *
	 * \return Returns an error code indicating success or failure.
	 */
	LSSTD_ERRORS LSE_CALL CScene::FinalizeTextures() {
		m_taTextures.Allocate( m_trsReferencedTextures.Length() + m_trsReferencedNormalTextures.Length() );
		// *************************************
		// STANDARD TEXTURES
		// *************************************
		// For each texture.
		for ( LSUINT32 I = 0; I < m_trsReferencedTextures.Length(); ++I ) {
			// Non-const reference so we can add a redirect directly the texture reference, but this
			//	does not change its position in the set so it is safe.
			CTexture::LSX_TEXTURE_REF & trRef = const_cast<CTexture::LSX_TEXTURE_REF &>(m_trsReferencedTextures.GetByIndex( I ));

			// Find if the image is both:
			//	#1: Already loaded.
			//	#2: Has the same flags.
			// If already loaded but different flags, a copy can be made in-memory rather than reloading it from the file.
			CTexture * ptCopyMe = NULL;
			CTexture * ptMatch = NULL;
			for ( LSUINT32 J = 0; J < m_taTextures.Length(); ++J ) {
				if ( CStd::StrCmp( m_taTextures[J].Path(), trRef.pftTexture->GetFileName() ) == 0 ) {
					ptCopyMe = &m_taTextures[J];
					// Both must be either sRGB or not sRGB, but only if we are dealing with the RGB channels.
					if ( ((m_taTextures[J].Flags() & LSX_TEX_sRGB) == (trRef.ui16Flags & LSX_TEX_sRGB)) ||	// Using RGB channels and in sRGB mode.
						(trRef.ui16Component == CTexture::LSX_C_A) ) {										// Using alpha so sRGB does not matter.
						ptCopyMe = NULL;
						ptMatch = &m_taTextures[J];
						break;
					}
				}
			}
			// If we found a match, add an access flag to it and we are done with this texture reference.
			if ( ptMatch ) {
				if ( trRef.ui16Component == CTexture::LSX_C_RGB ) {
					ptMatch->RefRgb();
				}
				else {
					ptMatch->RefA();
				}
				trRef.ui32TextureId = ptMatch->Id();
				continue;
			}
			// If ptCopyMe is not NULL, a matching path was found but wrong sRGB flags.  Make a copy.
			if ( ptCopyMe ) {
				if ( !m_taTextures.Push( CTexture() ) ) { return LSSTD_E_OUTOFMEMORY; }
				CTexture & tThis = m_taTextures[m_taTextures.Length()-1];
				LSSTD_ERRORS eError = tThis.CreateFromTexture( (*ptCopyMe), trRef.ui16Flags );
				if ( eError != LSSTD_E_SUCCESS ) { return eError; }

				if ( trRef.ui16Component == CTexture::LSX_C_RGB ) {
					tThis.RefRgb();
				}
				else {
					tThis.RefA();
				}
				trRef.ui32TextureId = tThis.Id();
				continue;
			}

			// Neither a match nor copy was found.  Add a new texture.
			if ( !m_taTextures.Push( CTexture() ) ) { return LSSTD_E_OUTOFMEMORY; }
			CTexture & tThis = m_taTextures[m_taTextures.Length()-1];
			LSSTD_ERRORS eError = tThis.CreateFromFile( trRef.pftTexture->GetFileName(), trRef.ui16Flags );
			if ( eError != LSSTD_E_SUCCESS ) { return eError; }

			if ( trRef.ui16Component == CTexture::LSX_C_RGB ) {
				tThis.RefRgb();
			}
			else {
				tThis.RefA();
			}
			trRef.ui32TextureId = tThis.Id();
			//::printf( "Image: \"%s\".\r\n", trRef.pftTexture->GetFileName() );
		}

		// Remove channels that do not belong.
		// This is just for disk space and really does not have to be done.
#if 0
		for ( LSUINT32 I = 0; I < m_taTextures.Length(); ++I ) {
			if ( !CImageLib::IsCompressed( m_taTextures[I].Image().GetFormat() ) ) {
				// Only working with uncompressed images here.

				// No access to RGB?
				if ( (m_taTextures[I].Flags() & LSX_TEX_RGB_ACCESS) == 0 ) {
					try {
						m_taTextures[I].Image().ConvertToFormatInPlace( LSI_PF_A8 );
					}
					catch ( ... ) {
						return LSSTD_E_OUTOFMEMORY;
					}
				}
				// No access to alpha?
				if ( (m_taTextures[I].Flags() & LSX_TEX_ALPHA_ACCESS) == 0 ) {
					// Only some formats matter.
					if ( m_taTextures[I].Image().GetFormat() == LSI_PF_R8G8B8A8 ) {
						try {
							m_taTextures[I].Image().ConvertToFormatInPlace( LSI_PF_R8G8B8 );
						}
						catch ( ... ) {
							return LSSTD_E_OUTOFMEMORY;
						}
						break;
					}

				}
			}
		}
#endif	// #if 0

		// Create channels that need to belong but do not (specifically alpha channels).
		for ( LSUINT32 I = 0; I < m_taTextures.Length(); ++I ) {
			if ( !CImageLib::IsCompressed( m_taTextures[I].Image().GetFormat() ) ) {
				// Only working with uncompressed images here.
				if ( (m_taTextures[I].Flags() & LSX_TEX_ALPHA_ACCESS) != 0 ) {
					// Alpha channel accessed.
					if ( CImageLib::GetComponentBits( m_taTextures[I].Image().GetFormat(), LSI_PC_A ) == 0 ) {
						// No alpha.
						// Add it by merging the RGB values.
						// We can take a shortcut because we know that all advanced formats include an alpha channel,
						//	so we know this format can be safely converted to LSI_PF_R8G8B8A8.

						try {
							m_taTextures[I].Image().ConvertToFormatInPlace( LSI_PF_R8G8B8A8 );
						}
						catch ( ... ) {
							return LSSTD_E_OUTOFMEMORY;
						}

						// Add alpha in-place.
						::printf( "Creating alpha for image: \"%s\".\r\n", m_taTextures[I].Path() );
						CImage & iTemp = m_taTextures[I].Image();
						for ( LSUINT32 Y = 0; Y < iTemp.GetHeight(); ++Y ) {
							for ( LSUINT32 X = 0; X < iTemp.GetWidth(); ++X ) {
								LSUINT64 ui64Texel = iTemp.GetTexelAt( LSI_PF_R8G8B8A8, X, Y );
								LSUINT32 ui32R = LSI_R( ui64Texel );
								LSUINT32 ui32G = LSI_G( ui64Texel );
								LSUINT32 ui32B = LSI_B( ui64Texel );
								LSFLOAT fR = ui32R / 255.0f * LSI_R_WEIGHT;
								LSFLOAT fG = ui32G / 255.0f * LSI_G_WEIGHT;
								LSFLOAT fB = ui32B / 255.0f * LSI_B_WEIGHT;
								LSUINT32 ui32A = static_cast<LSUINT32>(CStd::Clamp<LSUINT32>( static_cast<LSUINT32>((fR + fG + fB) * 255.0f + 0.5f), 0, 255 ));

								iTemp.SetTexelAt( LSI_MAKE_RGBA( ui32R, ui32G, ui32B, ui32A ), LSI_PF_R8G8B8A8, X, Y );
							}
						}

					}
				}
			}
		}


		// *************************************
		// NORMAL-MAP/BUMP-MAP TEXTURES
		// *************************************
		// For each texture.
		for ( LSUINT32 I = 0; I < m_trsReferencedNormalTextures.Length(); ++I ) {
			// Non-const reference so we can add a redirect directly the texture reference, but this
			//	does not change its position in the set so it is safe.
			CTexture::LSX_TEXTURE_REF & trRef = const_cast<CTexture::LSX_TEXTURE_REF &>(m_trsReferencedNormalTextures.GetByIndex( I ));

			// Does it already exist?
			CTexture * ptMatch = NULL;
			for ( LSUINT32 J = 0; J < m_taTextures.Length(); ++J ) {
				if ( CStd::StrCmp( m_taTextures[J].Path(), trRef.pftTexture->GetFileName() ) == 0 ) {
					ptMatch = &m_taTextures[J];
					break;
				}
			}
			if ( ptMatch ) { continue; }

			// Not found.  Add it.
			if ( !m_taTextures.Push( CTexture() ) ) { return LSSTD_E_OUTOFMEMORY; }
			CTexture & tThis = m_taTextures[m_taTextures.Length()-1];
			LSSTD_ERRORS eError = tThis.CreateFromFile( trRef.pftTexture->GetFileName(), trRef.ui16Flags | LSX_TEX_NORMALIZE_MIPS );
			if ( eError != LSSTD_E_SUCCESS ) { return eError; }
			trRef.ui32TextureId = tThis.Id();


			// Add it to the other list.
			if ( !m_trsReferencedTextures.Insert( trRef ) ) { return LSSTD_E_OUTOFMEMORY; }

			// If it is compressed, nothing we can do.
			if ( CImageLib::IsCompressed( tThis.Image().GetFormat() ) ) { continue; }

			// If it is black and white, convert to a normal map.
			if ( tThis.Image().IsBlackAndWhite() ) {
				LSCHAR szPath[LSF_MAX_PATH];
				CStd::StrNCpy( szPath, tThis.Path(), LSF_MAX_PATH );
				CFileLib::NoPath( szPath );
				::printf( "Converting \"%s\" to normal map.\r\n", szPath );
				tThis.Image().ConvertToNormalMap( 0.6f );
			}
			else {
				LSBOOL bIsDxtN = (tThis.Image().IsChannelSetTo( LSI_PC_R, 1.0f ) || tThis.Image().IsChannelSetTo( LSI_PC_R, 0.0f )) &&
					(tThis.Image().IsChannelSetTo( LSI_PC_B, 1.0f ) || tThis.Image().IsChannelSetTo( LSI_PC_B, 0.0f ));
				if ( bIsDxtN ) {
					tThis.Image().SwapChannels( LSI_PC_R, LSI_PC_A );
					tThis.Image().AddNormalMapZ();
				}
			}

			
		}
		// No longer needed.
		m_trsReferencedNormalTextures.Reset();


		// Generate mipmaps.
		for ( LSUINT32 I = 0; I < m_taTextures.Length(); ++I ) {
			if ( !CImageLib::IsCompressed( m_taTextures[I].Image().GetFormat() ) ) {
				LSCHAR szPath[LSF_MAX_PATH];
				CStd::StrNCpy( szPath, m_taTextures[I].Path(), LSF_MAX_PATH );
				CFileLib::NoPath( szPath );
				::printf( "Generating mipmaps for image: \"%s\". %u/%u.\r\n", szPath, I + 1, m_taTextures.Length() );
				if ( !m_taTextures[I].Image().GenerateMipMaps() ) { return LSSTD_E_OUTOFMEMORY; }
				if ( m_taTextures[I].Flags() & LSX_TEX_NORMALIZE_MIPS ) {
					m_taTextures[I].Image().Normalize();
				}
				else if ( CImageLib::GetComponentBits( m_taTextures[I].Image().GetFormat(), LSI_PC_A ) ) {
					// Pre-multiply alpha.
					m_taTextures[I].Image().PreMultiplyAlpha();
				}
			}
		}



		return LSSTD_E_SUCCESS;
	}

	/**
	 * Exports a node and all of its children recursively.  The exported data includes the node ID, its type, and
	 *	full transform information, including pivots.
	 *
	 * \param _pfNode The node to export.
	 * \param _bBuffer The buffer to which to export the node.
	 * \return Returns false if there is not enough memory to write the full node tree to the buffer.
	 */
	LSBOOL LSE_CALL LSE_CALL CScene::WriteNode( FbxNode * _pfNode, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		const LSCHAR * pcName = _pfNode->GetName();
#define LSX_TO_VEC( VAL )				CVector3( static_cast<LSFLOAT>(_pfNode->VAL.Get()[0]), static_cast<LSFLOAT>(_pfNode->VAL.Get()[1]), static_cast<LSFLOAT>(_pfNode->VAL.Get()[2]) )
		CVector3 vScale = LSX_TO_VEC( LclScaling );
		CVector3 vRot = LSX_TO_VEC( LclRotation );
		CVector3 vTrans = LSX_TO_VEC( LclTranslation );
#undef LSX_TO_VEC

		LSUINT64 ui64Id = reinterpret_cast<LSUINT64>(_pfNode);
		LSUINT64 ui64ParentId = reinterpret_cast<LSUINT64>(_pfNode->GetParent());
		if ( !CMisc::WriteUInt64( ui64Id, _bBuffer ) ) { return false; }
		if ( !CMisc::WriteUInt64( ui64ParentId, _bBuffer ) ) { return false; }
		FbxNodeAttribute * pfnaAttr = _pfNode->GetNodeAttribute();
		FbxNodeAttribute::EType eType = pfnaAttr ? pfnaAttr->GetAttributeType() : FbxNodeAttribute::eNull;
		if ( !CMisc::WriteUInt16( static_cast<LSUINT16>(CMisc::FbxNodeToModelLibraryNode( eType )), _bBuffer ) ) { return false; }
		if ( !CMisc::WriteUInt16( static_cast<LSUINT16>(CMisc::CountVisibleChildNodes( _pfNode )), _bBuffer ) ) { return false; }

		// Transform data.  Exported in order of use in the equation to determine the final local transform.
		// Scale pivot.
#define LSX_WRITE_VEC( VAL )																							\
		if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(_pfNode->VAL.Get()[0]), _bBuffer ) ) { return false; }			\
		if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(_pfNode->VAL.Get()[1]), _bBuffer ) ) { return false; }			\
		if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(_pfNode->VAL.Get()[2]), _bBuffer ) ) { return false; }

		// World = ParentWorld * T * Roff * Rp * Rpre * R * Rpost * Rp-1 * Soff * Sp * S * Sp-1
		LSX_WRITE_VEC( ScalingPivot )	// Inverse.
		LSX_WRITE_VEC( LclScaling )
		//LSX_WRITE_VEC( ScalingPivot )
		LSX_WRITE_VEC( ScalingOffset )

		LSX_WRITE_VEC( RotationPivot )	// Inverse.
		LSX_WRITE_VEC( PostRotation )
		LSX_WRITE_VEC( LclRotation )
		LSX_WRITE_VEC( PreRotation )
		//LSX_WRITE_VEC( RotationPivot )
		LSX_WRITE_VEC( RotationOffset )

		LSX_WRITE_VEC( LclTranslation )

		// Each child.
		for ( int I = 0; I < _pfNode->GetChildCount(); ++I ) {
			if ( _pfNode->GetChild( I )->GetVisibility() ) {
				if ( !WriteNode( _pfNode->GetChild( I ), _bBuffer ) ) {
					return false; 
				}
			}
		}
		return true;
#undef LSX_WRITE_VEC
	}

}	// namespace lsx

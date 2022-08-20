/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "LSITgt.h"


namespace lsi {

	// == Functions.
	/**
	 * Creates a tag-tree.
	 *
	 * \param _i32NumLeafSh Width of the array of leafs of the tree.
	 * \param _i32NumLeafSv Height of the array of leafs of the tree.
	 * \return Returns a new tag-tree if successful, returns NULL otherwise.
	 */
	CTgt::opj_tgt_tree_t * LSE_CALL CTgt::tgt_create( int32_t _i32NumLeafSh, int32_t _i32NumLeafSv ) {
		int32_t i32NpLh[32];
		int32_t i32NpLv[32];
		opj_tgt_node_t * ptnNode = NULL;
		opj_tgt_node_t * ptnParentNode = NULL;
		opj_tgt_node_t * parentnode0 = NULL;
		opj_tgt_tree_t * ptnTree = NULL;
		int32_t I, J, i32K;
		int32_t i32NumLvls;
		int32_t i32N;

		ptnTree = static_cast<opj_tgt_tree_t *>(CMemLib::MAlloc( sizeof( opj_tgt_tree_t ) ));
		if ( !ptnTree ) return NULL;
		ptnTree->numleafsh = _i32NumLeafSh;
		ptnTree->numleafsv = _i32NumLeafSv;

		i32NumLvls = 0;
		i32NpLh[0] = _i32NumLeafSh;
		i32NpLv[0] = _i32NumLeafSv;
		ptnTree->numnodes = 0;
		do {
			i32N = i32NpLh[i32NumLvls] * i32NpLv[i32NumLvls];
			i32NpLh[i32NumLvls + 1] = (i32NpLh[i32NumLvls] + 1) / 2;
			i32NpLv[i32NumLvls + 1] = (i32NpLv[i32NumLvls] + 1) / 2;
			ptnTree->numnodes += i32N;
			++i32NumLvls;
		} while ( i32N > 1 );
		
		if ( ptnTree->numnodes == 0 ) {
			CMemLib::Free( ptnTree );
			return NULL;
		}

		ptnTree->nodes = static_cast<opj_tgt_node_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(ptnTree->numnodes) * sizeof( opj_tgt_node_t ) ));
		if ( !ptnTree->nodes ) {
			CMemLib::Free( ptnTree );
			return NULL;
		}

		ptnNode = ptnTree->nodes;
		ptnParentNode = &ptnTree->nodes[ptnTree->numleafsh * ptnTree->numleafsv];
		parentnode0 = ptnParentNode;
		
		for ( I = 0; I < i32NumLvls - 1; ++I ) {
			for ( J = 0; J < i32NpLv[I]; ++J ) {
				i32K = i32NpLh[I];
				while ( --i32K >= 0 ) {
					ptnNode->parent = ptnParentNode;
					++ptnNode;
					if ( --i32K >= 0 ) {
						ptnNode->parent = ptnParentNode;
						++ptnNode;
					}
					++ptnParentNode;
				}
				if ( (J & 1) || J == i32NpLv[I] - 1 ) {
					parentnode0 = ptnParentNode;
				}
				else {
					ptnParentNode = parentnode0;
					parentnode0 += i32NpLh[I];
				}
			}
		}
		ptnNode->parent = 0;
		
		tgt_reset( ptnTree );
		
		return ptnTree;
	}

	/**
	 * Destroy a tag-tree, liberating memory.
	 *
	 * \param _pttTree Tag-tree to destroy.
	 */
	void LSE_CALL CTgt::tgt_destroy( opj_tgt_tree_t * _pttTree ) {
		CMemLib::Free( _pttTree->nodes );
		CMemLib::Free( _pttTree );
	}

	/**
	 * Reset a tag-tree (set all leaves to 0).
	 *
	 * \param _pttTree Tag-tree to reset.
	 */
	void LSE_CALL CTgt::tgt_reset( opj_tgt_tree_t * _pttTree ) {
		int32_t I;

		if ( NULL == _pttTree ) { return; }
		
		for ( I = 0; I < _pttTree->numnodes; I++ ) {
			_pttTree->nodes[I].value = 999;
			_pttTree->nodes[I].low = 0;
			_pttTree->nodes[I].known = 0;
		}
	}

	/**
	 * Sets the value of a leaf of a tag-tree.
	 *
	 * \param _pttTree Tag-tree to modify.
	 * \param _i32LeafNo Number that identifies the leaf to modify.
	 * \param _i32Value New value of the leaf.
	 */
	void LSE_CALL CTgt::tgt_setvalue( opj_tgt_tree_t * _pttTree, int32_t _i32LeafNo, int32_t _i32Value ) {
		opj_tgt_node_t * ptnNode = &_pttTree->nodes[_i32LeafNo];
		while ( ptnNode && ptnNode->value > _i32Value ) {
			ptnNode->value = _i32Value;
			ptnNode = ptnNode->parent;
		}
	}

	/**
	 * Encode the value of a leaf of the tag-tree up to a given threshold.
	 *
	 * \param _pbBio Pointer to a BIO handle.
	 * \param _pttTree Tag-tree to modify.
	 * \param _i32LeafNo Number that identifies the leaf to encode.
	 * \param _i32Threshold Threshold to use when encoding value of the leaf.
	 */
	void LSE_CALL CTgt::tgt_encode( CBio::opj_bio_t * _pbBio, opj_tgt_tree_t * _pttTree, int32_t _i32LeafNo, int32_t _i32Threshold ) {
		opj_tgt_node_t * ptnStk[31];
		opj_tgt_node_t ** pptnStkPtr;
		opj_tgt_node_t * ptnNode;
		int32_t i32Low;

		pptnStkPtr = ptnStk;
		ptnNode = &_pttTree->nodes[_i32LeafNo];
		while ( ptnNode->parent ) {
			(*pptnStkPtr++) = ptnNode;
			ptnNode = ptnNode->parent;
		}
		
		i32Low = 0;
		for ( ; ; ) {
			if ( i32Low > ptnNode->low ) {
				ptnNode->low = i32Low;
			}
			else {
				i32Low = ptnNode->low;
			}
			
			while ( i32Low < _i32Threshold ) {
				if ( i32Low >= ptnNode->value ) {
					if ( !ptnNode->known ) {
						CBio::bio_write( _pbBio, 1, 1 );
						ptnNode->known = 1;
					}
					break;
				}
				CBio::bio_write( _pbBio, 0, 1 );
				++i32Low;
			}
			
			ptnNode->low = i32Low;
			if ( pptnStkPtr == ptnStk ) { break; }
			ptnNode = (*--pptnStkPtr);
		}
	}

	/**
	 * Decode the value of a leaf of the tag-tree up to a given _i32Threshold.
	 *
	 * \param _pbBio Pointer to a BIO handle.
	 * \param _pttTree Tag-tree to decode.
	 * \param _i32LeafNo Number that identifies the leaf to decode.
	 * \param _i32Threshold Threshold to use when decoding value of the leaf.
	 * \return Returns 1 if the node's value < threshold, returns 0 otherwise.
	 */
	int32_t LSE_CALL CTgt::tgt_decode( CBio::opj_bio_t * _pbBio, opj_tgt_tree_t * _pttTree, int32_t _i32LeafNo, int32_t _i32Threshold ) {
		opj_tgt_node_t * ptnStk[31];
		opj_tgt_node_t ** pptnStkPtr;
		opj_tgt_node_t * ptnNode;
		int32_t i32Low;

		pptnStkPtr = ptnStk;
		ptnNode = &_pttTree->nodes[_i32LeafNo];
		while ( ptnNode->parent ) {
			(*pptnStkPtr++) = ptnNode;
			ptnNode = ptnNode->parent;
		}
		
		i32Low = 0;
		for ( ; ; ) {
			if ( i32Low > ptnNode->low ) {
				ptnNode->low = i32Low;
			}
			else {
				i32Low = ptnNode->low;
			}
			while ( i32Low < _i32Threshold && i32Low < ptnNode->value ) {
				if ( CBio::bio_read( _pbBio, 1 ) ) {
					ptnNode->value = i32Low;
				}
				else {
					++i32Low;
				}
			}
			ptnNode->low = i32Low;
			if ( pptnStkPtr == ptnStk ) {
				break;
			}
			ptnNode = (*--pptnStkPtr);
		}
		
		return (ptnNode->value < _i32Threshold) ? 1 : 0;
	}

}	// namespace lsi

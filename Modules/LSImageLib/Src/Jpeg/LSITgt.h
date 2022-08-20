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


#ifndef __LSI_TGT_H__
#define __LSI_TGT_H__

#include "../LSIImageLib.h"
#include "LSIBio.h"

namespace lsi {

	/**
	 * Class CTgt
	 * \brief Implementation of a tag-tree coder (TGT).
	 *
	 * Description: Implementation of a tag-tree coder (TGT).
	 */
	class CTgt {
	public :
		// == Types.
		/**
		 * Tag node
		 */
		typedef struct opj_tgt_node_t {
			opj_tgt_node_t *				parent;
			int32_t							value;
			int32_t							low;
			int32_t							known;
		} * LPopj_tgt_node_t, * const LPCopj_tgt_node_t;

		/**
		 * Tag tree
		 */
		typedef struct opj_tgt_tree_t {
			int32_t							numleafsh;
			int32_t							numleafsv;
			int32_t							numnodes;
			opj_tgt_node_t *				nodes;
		} * LPopj_tgt_tree_t, * const LPCopj_tgt_tree_t;


		// == Functions.
		/**
		 * Creates a tag-tree.
		 *
		 * \param _i32NumLeafSh Width of the array of leafs of the tree.
		 * \param _i32NumLeafSv Height of the array of leafs of the tree.
		 * \return Returns a new tag-tree if successful, returns NULL otherwise.
		 */
		static opj_tgt_tree_t * LSE_CALL	tgt_create( int32_t _i32NumLeafSh, int32_t _i32NumLeafSv );

		/**
		 * Destroy a tag-tree, liberating memory.
		 *
		 * \param _pttTree Tag-tree to destroy.
		 */
		static void LSE_CALL				tgt_destroy( opj_tgt_tree_t * _pttTree );

		/**
		 * Reset a tag-tree (set all leaves to 0).
		 *
		 * \param _pttTree Tag-tree to reset.
		 */
		static void LSE_CALL				tgt_reset( opj_tgt_tree_t * _pttTree );

		/**
		 * Sets the value of a leaf of a tag-tree.
		 *
		 * \param _pttTree Tag-tree to modify.
		 * \param _i32LeafNo Number that identifies the leaf to modify.
		 * \param _i32Value New value of the leaf.
		 */
		static void LSE_CALL				tgt_setvalue( opj_tgt_tree_t * _pttTree, int32_t _i32LeafNo, int32_t _i32Value );

		/**
		 * Encode the value of a leaf of the tag-tree up to a given threshold.
		 *
		 * \param _pbBio Pointer to a BIO handle.
		 * \param _pttTree Tag-tree to modify.
		 * \param _i32LeafNo Number that identifies the leaf to encode.
		 * \param _i32Threshold Threshold to use when encoding value of the leaf.
		 */
		static void LSE_CALL				tgt_encode( CBio::opj_bio_t * _pbBio, opj_tgt_tree_t * _pttTree, int32_t _i32LeafNo, int32_t _i32Threshold );

		/**
		 * Decode the value of a leaf of the tag-tree up to a given _i32Threshold.
		 *
		 * \param _pbBio Pointer to a BIO handle.
		 * \param _pttTree Tag-tree to decode.
		 * \param _i32LeafNo Number that identifies the leaf to decode.
		 * \param _i32Threshold Threshold to use when decoding value of the leaf.
		 * \return Returns 1 if the node's value < threshold, returns 0 otherwise.
		 */
		static int32_t LSE_CALL				tgt_decode( CBio::opj_bio_t * _pbBio, opj_tgt_tree_t * _pttTree, int32_t _i32LeafNo, int32_t _i32Threshold );


	protected :


	};

}	// namespace lsi

#endif	// __LSI_TGT_H__

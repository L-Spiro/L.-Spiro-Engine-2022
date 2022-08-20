/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2007, Callum Lerwick <seg@haxxed.com>
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

#include "LSIT1.h"
#include "LSIDwt.h"
#include "LSIJpegFixed.h"
#include "LSIJpegInt.h"
#include "LSIMct.h"
#include "LSIMqc.h"


namespace lsi {

	// == Members.
	/**
	 * Undocumented OpenJpeg Library data.
	 */
	char CT1::lut_ctxno_zc[1024] = {
		0, 1, 1, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 1, 1, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 1, 1, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 3, 3, 6, 3, 6, 6, 8, 3, 6, 6, 8, 6, 8, 8, 8, 1, 4, 4, 7, 4, 7, 7, 8, 4, 7, 7, 8, 7, 8, 8, 8,
		1, 4, 4, 7, 4, 7, 7, 8, 4, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8,
		1, 4, 4, 7, 4, 7, 7, 8, 4, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8,
		2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8,
		1, 4, 4, 7, 4, 7, 7, 8, 4, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8,
		2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8,
		2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8,
		2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8, 2, 5, 5, 7, 5, 7, 7, 8, 5, 7, 7, 8, 7, 8, 8, 8
	};

	/**
	 * Undocumented OpenJpeg Library data.
	 */
	char CT1::lut_ctxno_sc[256] = {
		0x9, 0xa, 0xc, 0xd, 0xa, 0xa, 0xd, 0xd, 0xc, 0xd, 0xc, 0xd, 0xd, 0xd, 0xd, 0xd,
		0x9, 0xa, 0xc, 0xb, 0xa, 0x9, 0xd, 0xc, 0xc, 0xb, 0xc, 0xb, 0xd, 0xc, 0xd, 0xc,
		0x9, 0xa, 0xc, 0xb, 0xa, 0xa, 0xb, 0xb, 0xc, 0xd, 0x9, 0xa, 0xd, 0xd, 0xa, 0xa,
		0x9, 0xa, 0xc, 0xd, 0xa, 0x9, 0xb, 0xc, 0xc, 0xb, 0x9, 0xa, 0xd, 0xc, 0xa, 0x9,
		0x9, 0xa, 0xc, 0xd, 0xa, 0x9, 0xb, 0xc, 0xc, 0xd, 0xc, 0xd, 0xb, 0xc, 0xb, 0xc,
		0x9, 0xa, 0xc, 0xb, 0xa, 0xa, 0xb, 0xb, 0xc, 0xb, 0xc, 0xb, 0xb, 0xb, 0xb, 0xb,
		0x9, 0xa, 0xc, 0xb, 0xa, 0x9, 0xd, 0xc, 0xc, 0xd, 0x9, 0xa, 0xb, 0xc, 0xa, 0x9,
		0x9, 0xa, 0xc, 0xd, 0xa, 0xa, 0xd, 0xd, 0xc, 0xb, 0x9, 0xa, 0xb, 0xb, 0xa, 0xa,
		0x9, 0xa, 0xc, 0xd, 0xa, 0xa, 0xd, 0xd, 0xc, 0xb, 0x9, 0xa, 0xb, 0xb, 0xa, 0xa,
		0x9, 0xa, 0xc, 0xb, 0xa, 0x9, 0xd, 0xc, 0xc, 0xd, 0x9, 0xa, 0xb, 0xc, 0xa, 0x9,
		0x9, 0xa, 0xc, 0xb, 0xa, 0xa, 0xb, 0xb, 0xc, 0xb, 0xc, 0xb, 0xb, 0xb, 0xb, 0xb,
		0x9, 0xa, 0xc, 0xd, 0xa, 0x9, 0xb, 0xc, 0xc, 0xd, 0xc, 0xd, 0xb, 0xc, 0xb, 0xc,
		0x9, 0xa, 0xc, 0xd, 0xa, 0x9, 0xb, 0xc, 0xc, 0xb, 0x9, 0xa, 0xd, 0xc, 0xa, 0x9,
		0x9, 0xa, 0xc, 0xb, 0xa, 0xa, 0xb, 0xb, 0xc, 0xd, 0x9, 0xa, 0xd, 0xd, 0xa, 0xa,
		0x9, 0xa, 0xc, 0xb, 0xa, 0x9, 0xd, 0xc, 0xc, 0xb, 0xc, 0xb, 0xd, 0xc, 0xd, 0xc,
		0x9, 0xa, 0xc, 0xd, 0xa, 0xa, 0xd, 0xd, 0xc, 0xd, 0xc, 0xd, 0xd, 0xd, 0xd, 0xd
	};

	/**
	 * Undocumented OpenJpeg Library data.
	 */
	char CT1::lut_spb[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0,
		0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 
		0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};

	/**
	 * Undocumented OpenJpeg Library data.
	 */
	int16_t CT1::lut_nmsedec_sig[1<<T1_NMSEDEC_BITS] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0180, 0x0300, 0x0480, 0x0600, 0x0780, 0x0900, 0x0a80,
		0x0c00, 0x0d80, 0x0f00, 0x1080, 0x1200, 0x1380, 0x1500, 0x1680,
		0x1800, 0x1980, 0x1b00, 0x1c80, 0x1e00, 0x1f80, 0x2100, 0x2280,
		0x2400, 0x2580, 0x2700, 0x2880, 0x2a00, 0x2b80, 0x2d00, 0x2e80,
		0x3000, 0x3180, 0x3300, 0x3480, 0x3600, 0x3780, 0x3900, 0x3a80,
		0x3c00, 0x3d80, 0x3f00, 0x4080, 0x4200, 0x4380, 0x4500, 0x4680,
		0x4800, 0x4980, 0x4b00, 0x4c80, 0x4e00, 0x4f80, 0x5100, 0x5280,
		0x5400, 0x5580, 0x5700, 0x5880, 0x5a00, 0x5b80, 0x5d00, 0x5e80,
		0x6000, 0x6180, 0x6300, 0x6480, 0x6600, 0x6780, 0x6900, 0x6a80,
		0x6c00, 0x6d80, 0x6f00, 0x7080, 0x7200, 0x7380, 0x7500, 0x7680
	};

	/**
	 * Undocumented OpenJpeg Library data.
	 */
	int16_t CT1::lut_nmsedec_sig0[1<<T1_NMSEDEC_BITS] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080, 0x0080,
		0x0080, 0x0080, 0x0100, 0x0100, 0x0100, 0x0180, 0x0180, 0x0200,
		0x0200, 0x0280, 0x0280, 0x0300, 0x0300, 0x0380, 0x0400, 0x0400,
		0x0480, 0x0500, 0x0580, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780,
		0x0800, 0x0880, 0x0900, 0x0980, 0x0a00, 0x0a80, 0x0b80, 0x0c00,
		0x0c80, 0x0d00, 0x0e00, 0x0e80, 0x0f00, 0x1000, 0x1080, 0x1180,
		0x1200, 0x1300, 0x1380, 0x1480, 0x1500, 0x1600, 0x1700, 0x1780,
		0x1880, 0x1980, 0x1a80, 0x1b00, 0x1c00, 0x1d00, 0x1e00, 0x1f00,
		0x2000, 0x2100, 0x2200, 0x2300, 0x2400, 0x2500, 0x2680, 0x2780,
		0x2880, 0x2980, 0x2b00, 0x2c00, 0x2d00, 0x2e80, 0x2f80, 0x3100,
		0x3200, 0x3380, 0x3480, 0x3600, 0x3700, 0x3880, 0x3a00, 0x3b00,
		0x3c80, 0x3e00, 0x3f80, 0x4080, 0x4200, 0x4380, 0x4500, 0x4680,
		0x4800, 0x4980, 0x4b00, 0x4c80, 0x4e00, 0x4f80, 0x5180, 0x5300,
		0x5480, 0x5600, 0x5800, 0x5980, 0x5b00, 0x5d00, 0x5e80, 0x6080,
		0x6200, 0x6400, 0x6580, 0x6780, 0x6900, 0x6b00, 0x6d00, 0x6e80,
		0x7080, 0x7280, 0x7480, 0x7600, 0x7800, 0x7a00, 0x7c00, 0x7e00
	};

	/**
	 * Undocumented OpenJpeg Library data.
	 */
	int16_t CT1::lut_nmsedec_ref[1<<T1_NMSEDEC_BITS] = {
		0x1800, 0x1780, 0x1700, 0x1680, 0x1600, 0x1580, 0x1500, 0x1480,
		0x1400, 0x1380, 0x1300, 0x1280, 0x1200, 0x1180, 0x1100, 0x1080,
		0x1000, 0x0f80, 0x0f00, 0x0e80, 0x0e00, 0x0d80, 0x0d00, 0x0c80,
		0x0c00, 0x0b80, 0x0b00, 0x0a80, 0x0a00, 0x0980, 0x0900, 0x0880,
		0x0800, 0x0780, 0x0700, 0x0680, 0x0600, 0x0580, 0x0500, 0x0480,
		0x0400, 0x0380, 0x0300, 0x0280, 0x0200, 0x0180, 0x0100, 0x0080,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0080, 0x0100, 0x0180, 0x0200, 0x0280, 0x0300, 0x0380,
		0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780,
		0x0800, 0x0880, 0x0900, 0x0980, 0x0a00, 0x0a80, 0x0b00, 0x0b80,
		0x0c00, 0x0c80, 0x0d00, 0x0d80, 0x0e00, 0x0e80, 0x0f00, 0x0f80,
		0x1000, 0x1080, 0x1100, 0x1180, 0x1200, 0x1280, 0x1300, 0x1380,
		0x1400, 0x1480, 0x1500, 0x1580, 0x1600, 0x1680, 0x1700, 0x1780
	};

	/**
	 * Undocumented OpenJpeg Library data.
	 */
	int16_t CT1::lut_nmsedec_ref0[1<<T1_NMSEDEC_BITS] = {
		0x2000, 0x1f00, 0x1e00, 0x1d00, 0x1c00, 0x1b00, 0x1a80, 0x1980, 
		0x1880, 0x1780, 0x1700, 0x1600, 0x1500, 0x1480, 0x1380, 0x1300,
		0x1200, 0x1180, 0x1080, 0x1000, 0x0f00, 0x0e80, 0x0e00, 0x0d00,
		0x0c80, 0x0c00, 0x0b80, 0x0a80, 0x0a00, 0x0980, 0x0900, 0x0880,
		0x0800, 0x0780, 0x0700, 0x0680, 0x0600, 0x0580, 0x0580, 0x0500,
		0x0480, 0x0400, 0x0400, 0x0380, 0x0300, 0x0300, 0x0280, 0x0280,
		0x0200, 0x0200, 0x0180, 0x0180, 0x0100, 0x0100, 0x0100, 0x0080,
		0x0080, 0x0080, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080, 0x0080,
		0x0080, 0x0080, 0x0100, 0x0100, 0x0100, 0x0180, 0x0180, 0x0200,
		0x0200, 0x0280, 0x0280, 0x0300, 0x0300, 0x0380, 0x0400, 0x0400,
		0x0480, 0x0500, 0x0580, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780,
		0x0800, 0x0880, 0x0900, 0x0980, 0x0a00, 0x0a80, 0x0b80, 0x0c00,
		0x0c80, 0x0d00, 0x0e00, 0x0e80, 0x0f00, 0x1000, 0x1080, 0x1180,
		0x1200, 0x1300, 0x1380, 0x1480, 0x1500, 0x1600, 0x1700, 0x1780,
		0x1880, 0x1980, 0x1a80, 0x1b00, 0x1c00, 0x1d00, 0x1e00, 0x1f00
	};

	// == Functions.
	/**
	 * Creates a new T1 handle and initialize the look-up tables of the Tier-1 coder/decoder.
	 *
	 * \param _cpInfo Undocumented parameter from the OpenJpeg Library.
	 * \return Returns a new T1 handle if successful, returns NULL otherwise.
	 * \see t1_init_luts
	 */
	CT1::opj_t1_t * LSE_CALL CT1::t1_create( CJpegCommonStructs::opj_common_ptr _cpInfo ) {
		opj_t1_t * ptT1 = static_cast<opj_t1_t *>(CMemLib::MAlloc( sizeof( opj_t1_t ) ));
		if ( !ptT1 ) { return NULL; }

		ptT1->cinfo = _cpInfo;
		// Create MQC and RAW handles.
		ptT1->mqc = CMqc::mqc_create();
		ptT1->raw = CRaw::raw_create();

		ptT1->data = NULL;
		ptT1->flags = NULL;
		ptT1->datasize = 0;
		ptT1->flagssize = 0;

		return ptT1;
	}

	/**
	 * Destroy a previously created T1 handle.
	 *
	 * \param _ptT1 T1 handle to destroy.
	 */
	void LSE_CALL CT1::t1_destroy( opj_t1_t * _ptT1 ) {
		if ( _ptT1 ) {
			// Destroy MQC and RAW handles.
			CMqc::mqc_destroy( _ptT1->mqc );
			CRaw::raw_destroy( _ptT1->raw );
			CMemLib::Free( _ptT1->data );
			CMemLib::Free( _ptT1->flags );
			CMemLib::Free( _ptT1 );
		}
	}

	/**
	 * Encode the code-blocks of a tile.
	 *
	 * \param _ptT1 T1 handle.
	 * \param _pttTile The tile to encode.
	 * \param _ptTcp Tile coding parameters.
	 */
	void LSE_CALL CT1::t1_encode_cblks( opj_t1_t * _ptT1, CTcd::opj_tcd_tile_t * _pttTile, CJ2k::opj_tcp_t * _ptTcp ) {
		int32_t i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo;

		_pttTile->distotile = 0;

		for ( i32CompNo = 0; i32CompNo < _pttTile->numcomps; ++i32CompNo ) {
			CTcd::opj_tcd_tilecomp_t * pttTileC = &_pttTile->comps[i32CompNo];
			CJ2k::opj_tccp_t * ptTccp = &_ptTcp->tccps[i32CompNo];
			int32_t i32TileW = pttTileC->x1 - pttTileC->x0;

			for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; ++i32ResNo ) {
				CTcd::opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];

				for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; ++i32BandNo ) {
					CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];

					for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; ++i32PrecNo ) {
						CTcd::opj_tcd_precinct_t * ptpPrec = &ptbBand->precincts[i32PrecNo];

						for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrec->cw * ptpPrec->ch; ++i32CodeBlockNo ) {
							CTcd::opj_tcd_cblk_enc_t * ptceCodeBlock = &ptpPrec->cblks.enc[i32CodeBlockNo];
							int32_t * pi32DataP;
							int32_t * pi32TileDp;
							int32_t i32CodeBlockW;
							int32_t i32CodeBlockH;
							int32_t I, J;

							int32_t i32X = ptceCodeBlock->x0 - ptbBand->x0;
							int32_t i32Y = ptceCodeBlock->y0 - ptbBand->y0;
							if ( ptbBand->bandno & 1 ) {
								CTcd::opj_tcd_resolution_t * ptrTemp = &pttTileC->resolutions[i32ResNo-1];
								i32X += ptrTemp->x1 - ptrTemp->x0;
							}
							if ( ptbBand->bandno & 2 ) {
								CTcd::opj_tcd_resolution_t * ptrTemp = &pttTileC->resolutions[i32ResNo-1];
								i32Y += ptrTemp->y1 - ptrTemp->y0;
							}

							if ( !allocate_buffers( 
								_ptT1,
								ptceCodeBlock->x1 - ptceCodeBlock->x0,
								ptceCodeBlock->y1 - ptceCodeBlock->y0 ) ) {
								return;
							}

							pi32DataP = _ptT1->data;
							i32CodeBlockW = _ptT1->w;
							i32CodeBlockH = _ptT1->h;

							pi32TileDp = &pttTileC->data[(i32Y*i32TileW)+i32X];
							if ( ptTccp->qmfbid == 1 ) {
								for ( J = 0; J < i32CodeBlockH; ++J ) {
									for ( I = 0; I < i32CodeBlockW; ++I ) {
										int32_t i32Temp = pi32TileDp[(J*i32TileW)+I];
										pi32DataP[(J*i32CodeBlockW)+I] = i32Temp << T1_NMSEDEC_FRACBITS;
									}
								}
							}
							else {			// if ( ptTccp->qmfbid == 0 ).
								for ( J = 0; J < i32CodeBlockH; ++J ) {
									for ( I = 0; I < i32CodeBlockW; ++I ) {
										int32_t i32Temp = pi32TileDp[(J*i32TileW)+I];
										pi32DataP[(J*i32CodeBlockW)+I] = CJpegFixed::Mul( i32Temp,
											8192 * 8192 / (static_cast<int32_t>(::floor( ptbBand->stepsize * 8192 )))) >> (11 - T1_NMSEDEC_FRACBITS);
									}
								}
							}

							t1_encode_cblk(
								_ptT1,
								ptceCodeBlock,
								ptbBand->bandno,
								i32CompNo,
								pttTileC->numresolutions - 1 - i32ResNo,
								ptTccp->qmfbid,
								ptbBand->stepsize,
								ptTccp->cblksty,
								_pttTile->numcomps,
								_ptTcp->mct,
								_pttTile );

						}	// for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrec->cw * ptpPrec->ch; ++i32CodeBlockNo ) {
					}	// for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; ++i32PrecNo ) {
				}	// for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; ++i32BandNo ) {
			}	// for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; ++i32ResNo ) {
		}	// for ( i32CompNo = 0; i32CompNo < _pttTile->numcomps; ++i32CompNo ) {
	}

	/**
	 * Decode the code-blocks of a tile.
	 *
	 * \param _ptT1 T1 handle.
	 * \param _pttTileC The tile to decode.
	 * \param _ptTccp Tile coding parameters.
	 */
	void LSE_CALL CT1::t1_decode_cblks( opj_t1_t * _ptT1, CTcd::opj_tcd_tilecomp_t * _pttTileC, CJ2k::opj_tccp_t * _ptTccp ) {
		int32_t i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo;

		int32_t i32TileW = _pttTileC->x1 - _pttTileC->x0;

		for ( i32ResNo = 0; i32ResNo < _pttTileC->numresolutions; ++i32ResNo ) {
			CTcd::opj_tcd_resolution_t * ptrRes = &_pttTileC->resolutions[i32ResNo];

			for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; ++i32BandNo ) {
				CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];

				for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; ++i32PrecNo ) {
					CTcd::opj_tcd_precinct_t * ptpPrec = &ptbBand->precincts[i32PrecNo];

					for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrec->cw * ptpPrec->ch; ++i32CodeBlockNo ) {
						CTcd::opj_tcd_cblk_dec_t * ptcdCodeBlock = &ptpPrec->cblks.dec[i32CodeBlockNo];
						int32_t * pui8DataP;
						int32_t i32CodeBlockW, i32CodeBlockH;
						int32_t i32X, i32Y;
						int32_t I, J;

						t1_decode_cblk( 
							_ptT1, 
							ptcdCodeBlock, 
							ptbBand->bandno, 
							_ptTccp->roishift, 
							_ptTccp->cblksty );

						i32X = ptcdCodeBlock->x0 - ptbBand->x0;
						i32Y = ptcdCodeBlock->y0 - ptbBand->y0;
						if ( ptbBand->bandno & 1 ) {
							CTcd::opj_tcd_resolution_t * ptrPres = &_pttTileC->resolutions[i32ResNo-1];
							i32X += ptrPres->x1 - ptrPres->x0;
						}
						if ( ptbBand->bandno & 2 ) {
							CTcd::opj_tcd_resolution_t * ptrPres = &_pttTileC->resolutions[i32ResNo-1];
							i32Y += ptrPres->y1 - ptrPres->y0;
						}

						pui8DataP = _ptT1->data;
						i32CodeBlockW = _ptT1->w;
						i32CodeBlockH = _ptT1->h;

						if ( _ptTccp->roishift ) {
							int32_t i32Thresh = 1 << _ptTccp->roishift;
							for ( J = 0; J < i32CodeBlockH; ++J ) {
								for ( I = 0; I < i32CodeBlockW; ++I ) {
									int32_t i32Val = pui8DataP[(J*i32CodeBlockW)+I];
									int32_t i32Mag = ::abs( i32Val );
									if ( i32Mag >= i32Thresh ) {
										i32Mag >>= _ptTccp->roishift;
										pui8DataP[(J*i32CodeBlockW)+I] = i32Val < 0 ? -i32Mag : i32Mag;
									}
								}
							}
						}

						if ( _ptTccp->qmfbid == 1 ) {
							int32_t * pi32TileDp = &_pttTileC->data[(i32Y*i32TileW)+i32X];
							for ( J = 0; J < i32CodeBlockH; ++J ) {
								for ( I = 0; I < i32CodeBlockW; ++I ) {
									int32_t i32Temp = pui8DataP[(J*i32CodeBlockW)+I];
									pi32TileDp[(J*i32TileW)+I] = i32Temp / 2;
								}
							}
						}
						else {			// if ( _ptTccp->qmfbid == 0 )
							float * pfTileDp = reinterpret_cast<float *>(&_pttTileC->data[(i32Y*i32TileW)+i32X]);
							for ( J = 0; J < i32CodeBlockH; ++J ) {
								float * pfTileDp2 = pfTileDp;
								for ( I = 0; I < i32CodeBlockW; ++I ) {
									float fTemp = (*pui8DataP) * ptbBand->stepsize;
									(*pfTileDp2) = fTemp;
									pui8DataP++;
									pfTileDp2++;
								}
								pfTileDp += i32TileW;
							}
						}
						CMemLib::Free( ptcdCodeBlock->data );
						CMemLib::Free( ptcdCodeBlock->segs );
					}	// for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrec->cw * ptpPrec->ch; ++i32CodeBlockNo ) {
					CMemLib::Free( ptpPrec->cblks.dec );
				}	// for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; ++i32PrecNo ) {
			}	// for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; ++i32BandNo ) {
		}	// for ( i32ResNo = 0; i32ResNo < _pttTileC->numresolutions; ++i32ResNo ) {
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _i32F Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	char LSE_CALL CT1::t1_getctxno_zc( int32_t _i32F, int32_t _i32Orient ) {
		return lut_ctxno_zc[(_i32Orient<<8)|(_i32F&T1_SIG_OTH)];
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _i32F Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	char LSE_CALL CT1::t1_getctxno_sc( int32_t _i32F ) {
		return lut_ctxno_sc[(_i32F&(T1_SIG_PRIM|T1_SGN))>>4];
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _i32F Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CT1::t1_getctxno_mag( int32_t _i32F ) {
		int32_t i32Temp1 = (_i32F & T1_SIG_OTH) ? T1_CTXNO_MAG + 1 : T1_CTXNO_MAG;
		int32_t i32Temp2 = (_i32F & T1_REFINE) ? T1_CTXNO_MAG + 2 : i32Temp1;
		return i32Temp2;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _i32F Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	char LSE_CALL CT1::t1_getspb( int32_t _i32F ) {
		return lut_spb[(_i32F&(T1_SIG_PRIM|T1_SGN))>>4];
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _i32X Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BitPos Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int16_t LSE_CALL CT1::t1_getnmsedec_sig( int32_t _i32X, int32_t _i32BitPos ) {
		if ( _i32BitPos > T1_NMSEDEC_FRACBITS ) {
			return lut_nmsedec_sig[(_i32X>>(_i32BitPos-T1_NMSEDEC_FRACBITS))&((1<<T1_NMSEDEC_BITS)-1)];
		}
		return lut_nmsedec_sig0[_i32X&((1<<T1_NMSEDEC_BITS)-1)];
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _i32X Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BitPos Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int16_t LSE_CALL CT1::t1_getnmsedec_ref( int32_t _i32X, int32_t _i32BitPos ) {
		if ( _i32BitPos > T1_NMSEDEC_FRACBITS ) {
			return lut_nmsedec_ref[(_i32X>>(_i32BitPos-T1_NMSEDEC_FRACBITS))&((1<<T1_NMSEDEC_BITS)-1)];
		}

		return lut_nmsedec_ref0[_i32X&((1<<T1_NMSEDEC_BITS)-1)];
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _i32S Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Stride Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_updateflags( flag_t * _pfFlags, int32_t _i32S, int32_t _i32Stride ) {
		flag_t * pfN = _pfFlags - _i32Stride;
		flag_t * pfS = _pfFlags + _i32Stride;

		static const flag_t fMod[] = {
			T1_SIG_S, T1_SIG_S | T1_SGN_S,
			T1_SIG_E, T1_SIG_E | T1_SGN_E,
			T1_SIG_W, T1_SIG_W | T1_SGN_W,
			T1_SIG_N, T1_SIG_N | T1_SGN_N
		};

		pfN[-1] |= T1_SIG_SE;
		pfN[0] |= fMod[_i32S];
		pfN[1] |= T1_SIG_SW;

		_pfFlags[-1] |= fMod[_i32S+2];
		_pfFlags[0] |= T1_SIG;
		_pfFlags[1] |= fMod[_i32S+4];

		pfS[-1] |= T1_SIG_NE;
		pfS[0] |= fMod[_i32S+6];
		pfS[1] |= T1_SIG_NW;
	}

	/**
	 * Encode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32One Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Type Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_enc_sigpass_step(
		opj_t1_t * _ptT1,
		flag_t * _pfFlags,
		int32_t * _pi32Data,
		int32_t _i32Orient,
		int32_t _i32BpNo,
		int32_t _i32One,
		int32_t * _pi32NmseDec,
		char _i32Type,
		int32_t _i32Vsc ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;		// MQC component.

		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( (i32Flag & T1_SIG_OTH) && !(i32Flag & (T1_SIG | T1_VISIT)) ) {
			i32V = CJpegInt::int_abs( (*_pi32Data) ) & _i32One ? 1 : 0;
			mqc_setcurctx( pmMqc, t1_getctxno_zc( i32Flag, _i32Orient ) );
			if ( _i32Type == T1_TYPE_RAW ) {
				CMqc::mqc_bypass_enc( pmMqc, i32V );
			}
			else {
				CMqc::mqc_encode( pmMqc, i32V );
			}
			if ( i32V ) {
				i32V = (*_pi32Data) < 0 ? 1 : 0;
				(*_pi32NmseDec) += t1_getnmsedec_sig( CJpegInt::int_abs( (*_pi32Data) ), _i32BpNo + T1_NMSEDEC_FRACBITS );
				mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
				if ( _i32Type == T1_TYPE_RAW ) {
					CMqc::mqc_bypass_enc( pmMqc, i32V );
				}
				else {
					CMqc::mqc_encode( pmMqc, i32V ^ t1_getspb( i32Flag ) );
				}
				t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
			}
			(*_pfFlags) |= T1_VISIT;
		}
	}

	/**
	 * Decode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_sigpass_step_raw( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t /*_i32Orient*/, 
		int32_t _i32OnePlusHalf, 
		int32_t _i32Vsc ) {
		int32_t i32V, i32Flag;
	
		CRaw::opj_raw_t * prRaw = _ptT1->raw;	// RAW component.
		
		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( (i32Flag & T1_SIG_OTH) && !(i32Flag & (T1_SIG | T1_VISIT)) ) {
			if ( CRaw::raw_decode( prRaw ) ) {
				i32V = CRaw::raw_decode( prRaw );
				(*_pi32Data) = i32V ? -_i32OnePlusHalf : _i32OnePlusHalf;
				t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
			}
			(*_pfFlags) |= T1_VISIT;
		}
	}

	/**
	 * Decode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_sigpass_step_mqc( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32Orient, 
		int32_t _i32OnePlusHalf ) {
		int32_t i32V, i32Flag;
		
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.
		
		i32Flag = *_pfFlags;
		if ( (i32Flag & T1_SIG_OTH) && !(i32Flag & (T1_SIG | T1_VISIT)) ) {
				mqc_setcurctx( pmMqc, t1_getctxno_zc( i32Flag, _i32Orient ) );
				if ( CMqc::mqc_decode( pmMqc ) ) {
					mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
					i32V = CMqc::mqc_decode( pmMqc ) ^ t1_getspb( i32Flag );
					(*_pi32Data) = i32V ? -_i32OnePlusHalf : _i32OnePlusHalf;
					t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
				}
			(*_pfFlags) |= T1_VISIT;
		}
	}

	/**
	 * Decode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_sigpass_step_mqc_vsc( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32Orient, 
		int32_t _i32OnePlusHalf, 
		int32_t _i32Vsc ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.
		
		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( (i32Flag & T1_SIG_OTH) && !(i32Flag & (T1_SIG | T1_VISIT)) ) {
			mqc_setcurctx( pmMqc, t1_getctxno_zc( i32Flag, _i32Orient ) );
			if ( CMqc::mqc_decode( pmMqc ) ) {
				mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
				i32V = CMqc::mqc_decode( pmMqc ) ^ t1_getspb( i32Flag );
				(*_pi32Data) = i32V ? -_i32OnePlusHalf : _i32OnePlusHalf;
				t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
			}
			(*_pfFlags) |= T1_VISIT;
		}
	}

	/**
	 * Encode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _cType Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_enc_sigpass( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32Orient, 
		int32_t * _pi32NmseDec, 
		char _cType, 
		int32_t _i32CodeBlockStY ) {
		(*_pi32NmseDec) = 0;
		int32_t i32One = 1 << (_i32BpNo + T1_NMSEDEC_FRACBITS);
		for ( int32_t K = 0; K < _ptT1->h; K += 4 ) {
			for ( int32_t I = 0; I < _ptT1->w; ++I ) {
				for ( int32_t J = K; J < K + 4 && J < _ptT1->h; ++J ) {
					int32_t i32Vsc = ((_i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC) && (J == K + 3 || J == _ptT1->h - 1)) ? 1 : 0;
					t1_enc_sigpass_step( 
						_ptT1, 
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						_i32Orient, 
						_i32BpNo, 
						i32One, 
						_pi32NmseDec, 
						_cType, 
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Decode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_sigpass_raw( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32Orient, 
		int32_t _i32CodeBlockStY ) {
		int32_t i32One = 1 << _i32BpNo;
		int32_t i32Half = i32One >> 1;
		int32_t i32OnePlusHalf = i32One | i32Half;
		for ( int32_t K = 0; K < _ptT1->h; K += 4 ) {
			for ( int32_t I = 0; I < _ptT1->w; ++I ) {
				for ( int32_t J = K; J < K + 4 && J < _ptT1->h; ++J ) {
					int32_t i32Vsc = ((_i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC) && (J == K + 3 || J == _ptT1->h - 1)) ? 1 : 0;
					t1_dec_sigpass_step_raw(
						_ptT1,
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						_i32Orient,
						i32OnePlusHalf,
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Decode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_sigpass_mqc( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32Orient ) {
		int32_t * pi32Data1 = _ptT1->data;
		flag_t * pfFlags1 = &_ptT1->flags[1];
		int32_t i32One = 1 << _i32BpNo;
		int32_t i32Half = i32One >> 1;
		int32_t i32OnePlusHalf = i32One | i32Half;
		int32_t K;
		for ( K = 0; K < (_ptT1->h & ~3); K += 4 ) {
			for ( int32_t I = 0; I < _ptT1->w; ++I ) {
				int32_t * pi32Data2 = pi32Data1 + I;
				flag_t * pfFlags2 = pfFlags1 + I;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_sigpass_step_mqc( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
				pi32Data2 += _ptT1->w;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_sigpass_step_mqc( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
				pi32Data2 += _ptT1->w;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_sigpass_step_mqc( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
				pi32Data2 += _ptT1->w;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_sigpass_step_mqc( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
				pi32Data2 += _ptT1->w;
			}
			pi32Data1 += _ptT1->w << 2;
			pfFlags1 += _ptT1->flags_stride << 2;
		}
		for ( int32_t I = 0; I < _ptT1->w; ++I ) {
			int32_t * pi32Data2 = pi32Data1 + I;
			flag_t * pfFlags2 = pfFlags1 + I;
			for ( int32_t J = K; J < _ptT1->h; ++J ) {
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_sigpass_step_mqc( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
				pi32Data2 += _ptT1->w;
			}
		}
	}

	/**
	 * Decode significant pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_sigpass_mqc_vsc( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32Orient ) {
		int32_t i32One = 1 << _i32BpNo;
		int32_t i32Half = i32One >> 1;
		int32_t i32OnePlusHalf = i32One | i32Half;
		for ( int32_t K = 0; K < _ptT1->h; K += 4 ) {
			for ( int32_t I = 0; I < _ptT1->w; ++I ) {
				for ( int32_t J = K; J < K + 4 && J < _ptT1->h; ++J ) {
					int32_t i32Vsc = (J == K + 3 || J == _ptT1->h - 1) ? 1 : 0;
					t1_dec_sigpass_step_mqc_vsc(
						_ptT1,
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						_i32Orient,
						i32OnePlusHalf,
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Encode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32One Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _cType Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_enc_refpass_step( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32BpNo, 
		int32_t _i32One, 
		int32_t * _pi32NmseDec, 
		char _cType, 
		int32_t _i32Vsc ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.
		
		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( (i32Flag & (T1_SIG | T1_VISIT)) == T1_SIG ) {
			(*_pi32NmseDec) += t1_getnmsedec_ref( CJpegInt::int_abs( (*_pi32Data) ), _i32BpNo + T1_NMSEDEC_FRACBITS );
			i32V = CJpegInt::int_abs( *_pi32Data ) & _i32One ? 1 : 0;
			mqc_setcurctx( pmMqc, t1_getctxno_mag( i32Flag ) );
			if ( _cType == T1_TYPE_RAW ) {
				CMqc::mqc_bypass_enc( pmMqc, i32V );
			}
			else {
				CMqc::mqc_encode( pmMqc, i32V );
			}
			(*_pfFlags) |= T1_REFINE;
		}
	}

	/**
	 * Decode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32PosHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32NegHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_refpass_step_raw( 
		opj_t1_t * _ptT1,
		flag_t * _pfFlags,
		int32_t * _pi32Data,
		int32_t _i32PosHalf,
		int32_t _i32NegHalf,
		int32_t _i32Vsc ) {
		int32_t i32V, i32T, i32Flag;
		
		CRaw::opj_raw_t * prRaw = _ptT1->raw;	// RAW component.
		
		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( (i32Flag & (T1_SIG | T1_VISIT)) == T1_SIG ) {
			i32V = CRaw::raw_decode( prRaw );
			i32T = i32V ? _i32PosHalf : _i32NegHalf;
			(*_pi32Data) += (*_pi32Data) < 0 ? -i32T : i32T;
			(*_pfFlags) |= T1_REFINE;
		}
	}

	/**
	 * Decode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32PosHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32NegHalf Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_refpass_step_mqc( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32PosHalf, 
		int32_t _i32NegHalf ) {
		int32_t i32V, i32T, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.
		
		i32Flag = (*_pfFlags);
		if ( (i32Flag & (T1_SIG | T1_VISIT)) == T1_SIG ) {
			mqc_setcurctx( pmMqc, t1_getctxno_mag( i32Flag ) );
			i32V = CMqc::mqc_decode( pmMqc );
			i32T = i32V ? _i32PosHalf : _i32NegHalf;
			(*_pi32Data) += (*_pi32Data) < 0 ? -i32T : i32T;
			(*_pfFlags) |= T1_REFINE;
		}
	}

	/**
	 * Decode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32PosHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32NegHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_refpass_step_mqc_vsc( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32PosHalf, 
		int32_t _i32NegHalf, 
		int32_t _i32Vsc ) {
		int32_t i32V, i32T, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	/* MQC component */
		
		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( (i32Flag & (T1_SIG | T1_VISIT)) == T1_SIG ) {
			mqc_setcurctx( pmMqc, t1_getctxno_mag( i32Flag ) );
			i32V = CMqc::mqc_decode( pmMqc );
			i32T = i32V ? _i32PosHalf : _i32NegHalf;
			(*_pi32Data) += (*_pi32Data) < 0 ? -i32T : i32T;
			(*_pfFlags) |= T1_REFINE;
		}
	}

	/**
	 * Encode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _cType Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_enc_refpass( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t * _pi32NmseDec, 
		char _cType, 
		int32_t _i32CodeBlockStY ) {
		(*_pi32NmseDec) = 0;
		int32_t i32One = 1 << (_i32BpNo + T1_NMSEDEC_FRACBITS);
		for ( int32_t K = 0; K < _ptT1->h; K += 4 ) {
			for ( int32_t I = 0; I < _ptT1->w; ++I ) {
				for ( int32_t J = K; J < K + 4 && J < _ptT1->h; ++J ) {
					int32_t i32Vsc = ((_i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC) && (J == K + 3 || J == _ptT1->h - 1)) ? 1 : 0;
					t1_enc_refpass_step(
						_ptT1,
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						_i32BpNo,
						i32One,
						_pi32NmseDec,
						_cType,
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Decode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_refpass_raw( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32CodeBlockStY ) {
		int32_t I, J, K, i32One, i32PosHalf, i32NegHalf;
		int32_t i32Vsc;
		i32One = 1 << _i32BpNo;
		i32PosHalf = i32One >> 1;
		i32NegHalf = _i32BpNo > 0 ? -i32PosHalf : -1;
		for ( K = 0; K < _ptT1->h; K += 4 ) {
			for ( I = 0; I < _ptT1->w; ++I ) {
				for ( J = K; J < K + 4 && J < _ptT1->h; ++J ) {
					i32Vsc = ((_i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC) && (J == K + 3 || J == _ptT1->h - 1)) ? 1 : 0;
					t1_dec_refpass_step_raw(
						_ptT1,
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						i32PosHalf,
						i32NegHalf,
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Decode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_refpass_mqc( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo ) {

		int32_t I, J, K, i32One, i32PosHalf, i32NegHalf;
		int32_t * pi32Data1 = _ptT1->data;
		flag_t * pfFlags1 = &_ptT1->flags[1];
		i32One = 1 << _i32BpNo;
		i32PosHalf = i32One >> 1;
		i32NegHalf = _i32BpNo > 0 ? -i32PosHalf : -1;
		for ( K = 0; K < (_ptT1->h & ~3); K += 4 ) {
			for ( I = 0; I < _ptT1->w; ++I ) {
				int32_t * pi32Data2 = pi32Data1 + I;
				flag_t * pfFlags2 = pfFlags1 + I;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_refpass_step_mqc( _ptT1, pfFlags2, pi32Data2, i32PosHalf, i32NegHalf );
				pi32Data2 += _ptT1->w;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_refpass_step_mqc( _ptT1, pfFlags2, pi32Data2, i32PosHalf, i32NegHalf );
				pi32Data2 += _ptT1->w;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_refpass_step_mqc( _ptT1, pfFlags2, pi32Data2, i32PosHalf, i32NegHalf );
				pi32Data2 += _ptT1->w;
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_refpass_step_mqc( _ptT1, pfFlags2, pi32Data2, i32PosHalf, i32NegHalf );
				pi32Data2 += _ptT1->w;
			}
			pi32Data1 += _ptT1->w << 2;
			pfFlags1 += _ptT1->flags_stride << 2;
		}
		for ( I = 0; I < _ptT1->w; ++I ) {
			int32_t * pi32Data2 = pi32Data1 + I;
			flag_t * pfFlags2 = pfFlags1 + I;
			for ( J = K; J < _ptT1->h; ++J ) {
				pfFlags2 += _ptT1->flags_stride;
				t1_dec_refpass_step_mqc( _ptT1, pfFlags2, pi32Data2, i32PosHalf, i32NegHalf );
				pi32Data2 += _ptT1->w;
			}
		}
	}

	/**
	 * Decode refinement pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_refpass_mqc_vsc( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo ) {
		int32_t i32One = 1 << _i32BpNo;
		int32_t i32PosHalf = i32One >> 1;
		int32_t i32NegHalf = _i32BpNo > 0 ? -i32PosHalf : -1;
		for ( int32_t K = 0; K < _ptT1->h; K += 4 ) {
			for ( int32_t I = 0; I < _ptT1->w; ++I ) {
				for ( int32_t J = K; J < K + 4 && J < _ptT1->h; ++J ) {
					int32_t i32Vsc = ((J == K + 3 || J == _ptT1->h - 1)) ? 1 : 0;
					t1_dec_refpass_step_mqc_vsc(
						_ptT1,
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						i32PosHalf,
						i32NegHalf,
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Encode clean-up pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32One Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Partial Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_enc_clnpass_step( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32Orient, 
		int32_t _i32BpNo, 
		int32_t _i32One, 
		int32_t * _pi32NmseDec, 
		int32_t _i32Partial, 
		int32_t _i32Vsc ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	/* MQC component */
		
		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( _i32Partial ) { goto Partial; }
		if ( !(*_pfFlags & (T1_SIG | T1_VISIT)) ) {
			mqc_setcurctx( pmMqc, t1_getctxno_zc( i32Flag, _i32Orient ) );
			i32V = CJpegInt::int_abs( *_pi32Data ) & _i32One ? 1 : 0;
			CMqc::mqc_encode( pmMqc, i32V );
			if ( i32V ) {
	Partial :
				*_pi32NmseDec += t1_getnmsedec_sig( CJpegInt::int_abs( (*_pi32Data) ), _i32BpNo + T1_NMSEDEC_FRACBITS );
				mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
				i32V = (*_pi32Data) < 0 ? 1 : 0;
				CMqc::mqc_encode( pmMqc, i32V ^ t1_getspb( i32Flag ) );
				t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
			}
		}
		(*_pfFlags) &= ~T1_VISIT;
	}

	/**
	 * Decode clean-up pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_clnpass_step_partial( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t /*_i32Orient*/, 
		int32_t _i32OnePlusHalf ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	/* MQC component */
		
		i32Flag = (*_pfFlags);
		mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
		i32V = CMqc::mqc_decode( pmMqc ) ^ t1_getspb( i32Flag );
		(*_pi32Data) = i32V ? -_i32OnePlusHalf : _i32OnePlusHalf;
		t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
		(*_pfFlags) &= ~T1_VISIT;
	}

	/**
	 * Decode clean-up pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_clnpass_step( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t * _pi32Data, 
		int32_t _i32Orient, 
		int32_t _i32OnePlusHalf ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	/* MQC component */
		
		i32Flag = (*_pfFlags);
		if ( !( i32Flag & ( T1_SIG | T1_VISIT ) ) ) {
			mqc_setcurctx( pmMqc, t1_getctxno_zc( i32Flag, _i32Orient ) );
			if ( CMqc::mqc_decode( pmMqc ) ) {
				mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
				i32V = CMqc::mqc_decode( pmMqc ) ^ t1_getspb( i32Flag );
				(*_pi32Data) = i32V ? -_i32OnePlusHalf : _i32OnePlusHalf;
				t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
			}
		}
		(*_pfFlags) &= ~T1_VISIT;
	}

	/**
	 * Decode clean-up pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Partial Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_clnpass_step_vsc( 
		opj_t1_t * _ptT1, 
		flag_t * _pfFlags, 
		int32_t *_pi32Data, 
		int32_t _i32Orient, 
		int32_t _i32OnePlusHalf, 
		int32_t _i32Partial, 
		int32_t _i32Vsc ) {
		int32_t i32V, i32Flag;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.

		i32Flag = _i32Vsc ? ((*_pfFlags) & (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW | T1_SGN_S))) : (*_pfFlags);
		if ( _i32Partial ) {
			goto Partial;
		}
		if ( !(i32Flag & (T1_SIG | T1_VISIT)) ) {
			mqc_setcurctx( pmMqc, t1_getctxno_zc( i32Flag, _i32Orient ) );
			if ( CMqc::mqc_decode( pmMqc ) ) {
	Partial :
				mqc_setcurctx( pmMqc, t1_getctxno_sc( i32Flag ) );
				i32V = CMqc::mqc_decode( pmMqc ) ^ t1_getspb( i32Flag );
				(*_pi32Data) = i32V ? -_i32OnePlusHalf : _i32OnePlusHalf;
				t1_updateflags( _pfFlags, i32V, _ptT1->flags_stride );
			}
		}
		(*_pfFlags) &= ~T1_VISIT;
	}

	/**
	 * Encode clean-up pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_enc_clnpass( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32Orient, 
		int32_t * _pi32NmseDec, 
		int32_t _i32CodeBlockStY ) {
		int32_t I, J, K, i32One, i32Agg, i32RunLen, i32Vsc;
	
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.
		
		(*_pi32NmseDec) = 0;
		i32One = 1 << (_i32BpNo + T1_NMSEDEC_FRACBITS);
		for ( K = 0; K < _ptT1->h; K += 4 ) {
			for ( I = 0; I < _ptT1->w; ++I ) {
				if ( K + 3 < _ptT1->h ) {
					if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC ) {
						i32Agg = !(MACRO_t1_flags( 1 + K, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 1, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 2, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| (MACRO_t1_flags( 1 + K + 3, 1 + I ) 
							& (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW |	T1_SGN_S))) & (T1_SIG | T1_VISIT | T1_SIG_OTH));
					}
					else {
						i32Agg = !(MACRO_t1_flags( 1 + K, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 1, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 2, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 3, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH));
					}
				}
				else {
					i32Agg = 0;
				}
				if ( i32Agg ) {
					for ( i32RunLen = 0; i32RunLen < 4; ++i32RunLen ) {
						if ( CJpegInt::int_abs( _ptT1->data[((K+i32RunLen)*_ptT1->w)+I] ) & i32One ) { break; }
					}
					mqc_setcurctx( pmMqc, T1_CTXNO_AGG );
					CMqc::mqc_encode( pmMqc, i32RunLen != 4 );
					if ( i32RunLen == 4 ) { continue; }
					mqc_setcurctx( pmMqc, T1_CTXNO_UNI );
					CMqc::mqc_encode( pmMqc, i32RunLen >> 1 );
					CMqc::mqc_encode( pmMqc, i32RunLen & 1 );
				}
				else { i32RunLen = 0; }
				for ( J = K + i32RunLen; J < K + 4 && J < _ptT1->h; ++J ) {
					i32Vsc = ((_i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC) && (J == K + 3 || J == _ptT1->h - 1)) ? 1 : 0;
					t1_enc_clnpass_step(
						_ptT1,
						&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
						&_ptT1->data[(J*_ptT1->w)+I],
						_i32Orient,
						_i32BpNo,
						i32One,
						_pi32NmseDec,
						i32Agg && (J == K + i32RunLen), 
						i32Vsc );
				}
			}
		}
	}

	/**
	 * Decode clean-up pass.
	 *
	 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_dec_clnpass( 
		opj_t1_t * _ptT1, 
		int32_t _i32BpNo, 
		int32_t _i32Orient, 
		int32_t _i32CodeBlockStY ) {
		int32_t I, J, K, i32One, i32Half, i32OnePlusHalf, i32Agg, i32RunLen, i32Vsc;
		int32_t i32SegSym = _i32CodeBlockStY & J2K_CCP_CBLKSTY_SEGSYM;
		
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.
		
		i32One = 1 << _i32BpNo;
		i32Half = i32One >> 1;
		i32OnePlusHalf = i32One | i32Half;
		if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC ) {
			for ( K = 0; K < _ptT1->h; K += 4 ) {
				for ( I = 0; I < _ptT1->w; ++I ) {
					if ( K + 3 < _ptT1->h ) {
						i32Agg = !(MACRO_t1_flags( 1 + K, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 1, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| MACRO_t1_flags( 1 + K + 2, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
							|| (MACRO_t1_flags( 1 + K + 3, 1 + I ) 
							& (~(T1_SIG_S | T1_SIG_SE | T1_SIG_SW |	T1_SGN_S))) & (T1_SIG | T1_VISIT | T1_SIG_OTH));
					}
					else {
						i32Agg = 0;
					}
					if ( i32Agg ) {
						mqc_setcurctx( pmMqc, T1_CTXNO_AGG );
						if ( !CMqc::mqc_decode( pmMqc ) ) {
							continue;
						}
						mqc_setcurctx( pmMqc, T1_CTXNO_UNI );
						i32RunLen = CMqc::mqc_decode( pmMqc );
						i32RunLen = (i32RunLen << 1) | CMqc::mqc_decode( pmMqc );
					}
					else {
						i32RunLen = 0;
					}
					for ( J = K + i32RunLen; J < K + 4 && J < _ptT1->h; ++J ) {
						i32Vsc = (J == K + 3 || J == _ptT1->h - 1) ? 1 : 0;
						t1_dec_clnpass_step_vsc(
							_ptT1,
							&_ptT1->flags[((J+1)*_ptT1->flags_stride)+I+1],
							&_ptT1->data[(J*_ptT1->w)+I],
							_i32Orient,
							i32OnePlusHalf,
							i32Agg && (J == K + i32RunLen),
							i32Vsc );
					}
				}
			}
		}
		else {
			int32_t * pi32Data1 = _ptT1->data;
			flag_t * pfFlags1 = &_ptT1->flags[1];
			for ( K = 0; K < (_ptT1->h & ~3); K += 4 ) {
				for ( I = 0; I < _ptT1->w; ++I ) {
					int32_t * pi32Data2 = pi32Data1 + I;
					flag_t * pfFlags2 = pfFlags1 + I;
					i32Agg = !(MACRO_t1_flags( 1 + K, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
						|| MACRO_t1_flags( 1 + K + 1, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
						|| MACRO_t1_flags( 1 + K + 2, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH)
						|| MACRO_t1_flags( 1 + K + 3, 1 + I ) & (T1_SIG | T1_VISIT | T1_SIG_OTH));
					if ( i32Agg ) {
						mqc_setcurctx( pmMqc, T1_CTXNO_AGG );
						if ( !CMqc::mqc_decode( pmMqc ) ) { continue; }
						mqc_setcurctx( pmMqc, T1_CTXNO_UNI );
						i32RunLen = CMqc::mqc_decode( pmMqc );
						i32RunLen = (i32RunLen << 1) | CMqc::mqc_decode( pmMqc );
						pfFlags2 += i32RunLen * _ptT1->flags_stride;
						pi32Data2 += i32RunLen * _ptT1->w;
						for ( J = K + i32RunLen; J < K + 4 && J < _ptT1->h; ++J ) {
							pfFlags2 += _ptT1->flags_stride;
							if ( i32Agg && ( J == K + i32RunLen ) ) {
								t1_dec_clnpass_step_partial( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
							}
							else {
								t1_dec_clnpass_step( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
							}
							pi32Data2 += _ptT1->w;
						}
					}
					else {
						pfFlags2 += _ptT1->flags_stride;
						t1_dec_clnpass_step( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
						pi32Data2 += _ptT1->w;
						pfFlags2 += _ptT1->flags_stride;
						t1_dec_clnpass_step( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
						pi32Data2 += _ptT1->w;
						pfFlags2 += _ptT1->flags_stride;
						t1_dec_clnpass_step( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
						pi32Data2 += _ptT1->w;
						pfFlags2 += _ptT1->flags_stride;
						t1_dec_clnpass_step( _ptT1, pfFlags2, pi32Data2, _i32Orient, i32OnePlusHalf );
						pi32Data2 += _ptT1->w;
					}
				}
				pi32Data1 += _ptT1->w << 2;
				pfFlags1 += _ptT1->flags_stride << 2;
			}
			for ( I = 0; I < _ptT1->w; ++I ) {
				int32_t * pi32Data2 = pi32Data1 + I;
				flag_t * flags2 = pfFlags1 + I;
				for ( J = K; J < _ptT1->h; ++J ) {
					flags2 += _ptT1->flags_stride;
					t1_dec_clnpass_step( _ptT1, flags2, pi32Data2, _i32Orient, i32OnePlusHalf );
					pi32Data2 += _ptT1->w;
				}
			}
		}

		if ( i32SegSym ) {
			int32_t i32V = 0;
			mqc_setcurctx( pmMqc, T1_CTXNO_UNI );
			i32V = CMqc::mqc_decode( pmMqc );
			i32V = (i32V << 1) | CMqc::mqc_decode( pmMqc );
			i32V = (i32V << 1) | CMqc::mqc_decode( pmMqc );
			i32V = (i32V << 1) | CMqc::mqc_decode( pmMqc );
		}
	}

	/**
	 * Decode clean-up pass.
	 *
	 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CompNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Level Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
	 * \param _i32QmfbId Undocumented parameter from the OpenJpeg Library.
	 * \param _dStepSize Undocumented parameter from the OpenJpeg Library.
	 * \param _i32NumComps Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Mct Undocumented parameter from the OpenJpeg Library.
	 */
	double LSE_CALL CT1::t1_getwmsedec( 
		int32_t _pi32NmseDec, 
		int32_t _i32CompNo, 
		int32_t _i32Level, 
		int32_t _i32Orient, 
		int32_t _i32BpNo, 
		int32_t _i32QmfbId, 
		double _dStepSize, 
		int32_t _i32NumComps, 
		int32_t _i32Mct ) {
		double dW1, dW2, dWmSeDec;
		if ( _i32QmfbId == 1 ) {
			dW1 = (_i32Mct && _i32NumComps == 3) ? CMct::mct_getnorm( _i32CompNo ) : 1.0;
			dW2 = CDwt::dwt_getnorm( _i32Level, _i32Orient );
		}
		else {
			dW1 = (_i32Mct && _i32NumComps == 3) ? CMct::mct_getnorm_real( _i32CompNo ) : 1.0;
			dW2 = CDwt::dwt_getnorm_real( _i32Level, _i32Orient );
		}
		dWmSeDec = dW1 * dW2 * _dStepSize * (1 << _i32BpNo);
		dWmSeDec *= dWmSeDec * _pi32NmseDec / 8192.0;
		
		return dWmSeDec;
	}

	/**
	 * Encode 1 code-block.
	 *
	 * \param _ptT1 T1 handle.
	 * \param _ptceCodeBlock Code-block coding parameters.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CompNo Component number.
	 * \param _i32Level Undocumented parameter from the OpenJpeg Library.
	 * \param _i32QmfbId Undocumented parameter from the OpenJpeg Library.
	 * \param _dStepSize Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Code-block style.
	 * \param _i32NumComps Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Mct Undocumented parameter from the OpenJpeg Library.
	 * \param _pttTile Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT1::t1_encode_cblk( 
		opj_t1_t * _ptT1, 
		CTcd::opj_tcd_cblk_enc_t * _ptceCodeBlock, 
		int32_t _i32Orient, 
		int32_t _i32CompNo, 
		int32_t _i32Level, 
		int32_t _i32QmfbId, 
		double _dStepSize, 
		int32_t _i32CodeBlockStY, 
		int32_t _i32NumComps, 
		int32_t _i32Mct, 
		CTcd::opj_tcd_tile_t * _pttTile ) {
		double cumwmsedec = 0.0;

		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.

		int32_t i32PassNo, i32BpNo, i32PassType;
		int32_t i32NmSeDec = 0;
		int32_t I, i32Max;
		char cType = T1_TYPE_MQ;
		double dTempWmSeDec;

		i32Max = 0;
		for ( I = 0; I < _ptT1->w * _ptT1->h; ++I ) {
			int32_t i32Temp = ::abs( _ptT1->data[I] );
			i32Max = CJpegInt::int_max( i32Max, i32Temp );
		}

		_ptceCodeBlock->numbps = i32Max ? (CJpegInt::int_floorlog2( i32Max ) + 1) - T1_NMSEDEC_FRACBITS : 0;
		
		i32BpNo = _ptceCodeBlock->numbps - 1;
		i32PassType = 2;
		
		CMqc::mqc_resetstates( pmMqc );
		CMqc::mqc_setstate( pmMqc, T1_CTXNO_UNI, 0, 46 );
		CMqc::mqc_setstate( pmMqc, T1_CTXNO_AGG, 0, 3 );
		CMqc::mqc_setstate( pmMqc, T1_CTXNO_ZC, 0, 4 );
		CMqc::mqc_init_enc( pmMqc, _ptceCodeBlock->data );
		
		for ( i32PassNo = 0; i32BpNo >= 0; ++i32PassNo ) {
			CTcd::opj_tcd_pass_t * ptpPass = &_ptceCodeBlock->passes[i32PassNo];
			int32_t i32Correction = 3;
			cType = ((i32BpNo < (_ptceCodeBlock->numbps - 4)) && (i32PassType < 2) && (_i32CodeBlockStY & J2K_CCP_CBLKSTY_LAZY)) ? T1_TYPE_RAW : T1_TYPE_MQ;
			
			switch ( i32PassType ) {
				case 0 : {
					t1_enc_sigpass( _ptT1, i32BpNo, _i32Orient, &i32NmSeDec, cType, _i32CodeBlockStY );
					break;
				}
				case 1 : {
					t1_enc_refpass( _ptT1, i32BpNo, &i32NmSeDec, cType, _i32CodeBlockStY );
					break;
				}
				case 2 : {
					t1_enc_clnpass( _ptT1, i32BpNo, _i32Orient, &i32NmSeDec, _i32CodeBlockStY );
					if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_SEGSYM ) {
						CMqc::mqc_segmark_enc( pmMqc );
					}
					break;
				}
			}

			
			// Fixed_quality.
			dTempWmSeDec = t1_getwmsedec( i32NmSeDec, _i32CompNo, _i32Level, _i32Orient, i32BpNo, _i32QmfbId, _dStepSize, _i32NumComps, _i32Mct );
			cumwmsedec += dTempWmSeDec;
			_pttTile->distotile += dTempWmSeDec;
			
			// Code switch "RESTART" (I.E. TERMALL).
			if ( ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_TERMALL )	&& !( ( i32PassType == 2 ) && ( i32BpNo - 1 < 0 ) ) ) {
				if ( cType == T1_TYPE_RAW ) {
					CMqc::mqc_flush( pmMqc );
					i32Correction = 1;
				}
				else {
					CMqc::mqc_flush( pmMqc );
					i32Correction = 1;
				}
				ptpPass->term = 1;
			}
			else {
				if ( ((i32BpNo < (_ptceCodeBlock->numbps - 4) && (i32PassType > 0)) 
					|| ((i32BpNo == (_ptceCodeBlock->numbps - 4)) && (i32PassType == 2))) && (_i32CodeBlockStY & J2K_CCP_CBLKSTY_LAZY) ) {
					if ( cType == T1_TYPE_RAW ) {
						CMqc::mqc_flush( pmMqc );
						i32Correction = 1;
					}
					else {
						CMqc::mqc_flush( pmMqc );
						i32Correction = 1;
					}
					ptpPass->term = 1;
				}
				else {
					ptpPass->term = 0;
				}
			}
			
			if ( ++i32PassType == 3 ) {
				i32PassType = 0;
				i32BpNo--;
			}
			
			if ( ptpPass->term && i32BpNo > 0 ) {
				cType = ((i32BpNo < (_ptceCodeBlock->numbps - 4)) && (i32PassType < 2) && (_i32CodeBlockStY & J2K_CCP_CBLKSTY_LAZY)) ? T1_TYPE_RAW : T1_TYPE_MQ;
				if ( cType == T1_TYPE_RAW ) {
					CMqc::mqc_bypass_init_enc( pmMqc );
				}
				else {
					CMqc::mqc_restart_init_enc( pmMqc );
				}
			}
			
			ptpPass->distortiondec = cumwmsedec;
			ptpPass->rate = CMqc::mqc_numbytes( pmMqc ) + i32Correction;
			
			// Code-switch "RESET".
			if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_RESET ) {
				CMqc::mqc_reset_enc( pmMqc );
			}
		}
		
		// Code switch "ERTERM" (I.E. PTERM).
		if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_PTERM ) {
			CMqc::mqc_erterm_enc( pmMqc );
		}
		else if ( !( _i32CodeBlockStY & J2K_CCP_CBLKSTY_LAZY ) ) {
			CMqc::mqc_flush( pmMqc );
		}
		
		_ptceCodeBlock->totalpasses = i32PassNo;

		for ( i32PassNo = 0; i32PassNo < _ptceCodeBlock->totalpasses; i32PassNo++ ) {
			CTcd::opj_tcd_pass_t * ptpPass = &_ptceCodeBlock->passes[i32PassNo];
			if ( ptpPass->rate > CMqc::mqc_numbytes( pmMqc ) ) {
				ptpPass->rate = CMqc::mqc_numbytes( pmMqc );
			}
			// Preventing generation of FF as last data byte of a pass.
			if ( (ptpPass->rate > 1) && (_ptceCodeBlock->data[ptpPass->rate-1] == 0xFF) ) {
				ptpPass->rate--;
			}
			ptpPass->len = ptpPass->rate - (i32PassNo == 0 ? 0 : _ptceCodeBlock->passes[i32PassNo-1].rate);
		}
	}

	/**
	 * Decode 1 code-block.
	 *
	 * \param _ptT1 T1 handle.
	 * \param _ptceCodeBlock Code-block coding parameters.
	 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
	 * \param _i32RoiShift Region of interest shifting value.
	 * \param _i32CodeBlockStY Code-block style.
	 */
	void LSE_CALL CT1::t1_decode_cblk( 
		opj_t1_t * _ptT1,
		CTcd::opj_tcd_cblk_dec_t * _ptceCodeBlock, 
		int32_t _i32Orient, 
		int32_t _i32RoiShift, 
		int32_t _i32CodeBlockStY ) {
		CRaw::opj_raw_t * prRaw = _ptT1->raw;	// RAW component.
		CMqc::opj_mqc_t * pmMqc = _ptT1->mqc;	// MQC component.

		int32_t i32BpNo, i32PassType;
		int32_t i32SegNo, i32PassNo;
		char cType = T1_TYPE_MQ;	// BYPASS mode.

		if ( !allocate_buffers( 
			_ptT1, 
			_ptceCodeBlock->x1 - _ptceCodeBlock->x0, 
			_ptceCodeBlock->y1 - _ptceCodeBlock->y0 ) ) {
			return;
		}

		i32BpNo = _i32RoiShift + _ptceCodeBlock->numbps - 1;
		i32PassType = 2;
		
		CMqc::mqc_resetstates( pmMqc );
		CMqc::mqc_setstate( pmMqc, T1_CTXNO_UNI, 0, 46 );
		CMqc::mqc_setstate( pmMqc, T1_CTXNO_AGG, 0, 3 );
		CMqc::mqc_setstate( pmMqc, T1_CTXNO_ZC, 0, 4 );
		
		for ( i32SegNo = 0; i32SegNo < _ptceCodeBlock->numsegs; ++i32SegNo ) {
			CTcd::opj_tcd_seg_t * ptsSeg = &_ptceCodeBlock->segs[i32SegNo];
			
			// BYPASS mode.
			cType = ((i32BpNo <= (_ptceCodeBlock->numbps - 1) - 4) && (i32PassType < 2) && (_i32CodeBlockStY & J2K_CCP_CBLKSTY_LAZY)) ? T1_TYPE_RAW : T1_TYPE_MQ;
			// FIXME: slviewer gets here with a null pointer. Why? Partially downloaded and/or corrupt textures?
			if ( ptsSeg->data == NULL ) { continue; }
			if ( cType == T1_TYPE_RAW ) {
				CRaw::raw_init_dec( prRaw, (*ptsSeg->data) + ptsSeg->dataindex, ptsSeg->len );
			}
			else {
				CMqc::mqc_init_dec( pmMqc, (*ptsSeg->data) + ptsSeg->dataindex, ptsSeg->len );
			}
			
			for ( i32PassNo = 0; i32PassNo < ptsSeg->numpasses; ++i32PassNo ) {
				switch ( i32PassType ) {
					case 0 : {
						if ( cType == T1_TYPE_RAW ) {
							t1_dec_sigpass_raw( _ptT1, i32BpNo + 1, _i32Orient, _i32CodeBlockStY );
						}
						else {
							if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC ) {
								t1_dec_sigpass_mqc_vsc( _ptT1, i32BpNo + 1, _i32Orient );
							}
							else {
								t1_dec_sigpass_mqc( _ptT1, i32BpNo+1, _i32Orient );
							}
						}
						break;
					}
					case 1 : {
						if ( cType == T1_TYPE_RAW ) {
							t1_dec_refpass_raw( _ptT1, i32BpNo + 1, _i32CodeBlockStY );
						}
						else {
							if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_VSC ) {
								t1_dec_refpass_mqc_vsc( _ptT1, i32BpNo + 1 );
							}
							else {
								t1_dec_refpass_mqc( _ptT1, i32BpNo + 1 );
							}
						}
						break;
					}
					case 2 : {
						t1_dec_clnpass( _ptT1, i32BpNo + 1, _i32Orient, _i32CodeBlockStY );
						break;
					}
				}
				
				if ( (_i32CodeBlockStY & J2K_CCP_CBLKSTY_RESET) && cType == T1_TYPE_MQ ) {
					CMqc::mqc_resetstates( pmMqc );
					CMqc::mqc_setstate( pmMqc, T1_CTXNO_UNI, 0, 46 );				
					CMqc::mqc_setstate( pmMqc, T1_CTXNO_AGG, 0, 3 );
					CMqc::mqc_setstate( pmMqc, T1_CTXNO_ZC, 0, 4 );
				}
				if ( ++i32PassType == 3 ) {
					i32PassType = 0;
					i32BpNo--;
				}
			}
		}
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _ptT1 T1 handle.
	 * \param _i32W Undocumented parameter from the OpenJpeg Library.
	 * \param _i32H Undocumented parameter from the OpenJpeg Library.
	 */
	LSBOOL LSE_CALL CT1::allocate_buffers( 
		opj_t1_t * _ptT1, 
		int32_t _i32W, 
		int32_t _i32H ) {
		int32_t i32DataSize = _i32W * _i32H;
		int32_t i32FlagsSize;

		if ( i32DataSize > _ptT1->datasize ) {
			CMemLib::Free( _ptT1->data );
			_ptT1->data = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32DataSize) * sizeof( int32_t ) ));
			if ( !_ptT1->data ) { return false; }
			_ptT1->datasize = i32DataSize;
		}
		CStd::MemSet( _ptT1->data, 0, static_cast<uint32_t>(i32DataSize) * sizeof( int32_t ) );

		_ptT1->flags_stride = _i32W + 2;
		i32FlagsSize = _ptT1->flags_stride * (_i32H + 2);

		if ( i32FlagsSize > _ptT1->flagssize ) {
			CMemLib::Free( _ptT1->flags );
			_ptT1->flags = static_cast<flag_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32FlagsSize) * sizeof( flag_t ) ));
			if ( !_ptT1->flags ) { return false; }
			_ptT1->flagssize = i32FlagsSize;
		}
		CStd::MemSet( _ptT1->flags, 0, static_cast<uint32_t>(i32FlagsSize) * sizeof( flag_t ) );

		_ptT1->w = _i32W;
		_ptT1->h = _i32H;

		return true;
	}

}	// namespace lsi

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
 * Description: DESCRIPTION.
 */


#ifndef __LSI_JPEGCOMMONSTRUCTS_H__
#define __LSI_JPEGCOMMONSTRUCTS_H__

#include "../LSIImageLib.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define OPJ_PATH_LEN				4096							/**< Maximum allowed size for filenames. */

#define J2K_MAXRLVLS				33								/**< Number of maximum resolution level authorized. */
#define J2K_MAXBANDS				(3*J2K_MAXRLVLS-2 )				/**< Number of maximum sub-band linked to number of resolution level. */

#define JPWL_MAX_NO_TILESPECS		16								/**< Maximum number of tile parts expected by JPWL: increase at your will. */
#define JPWL_MAX_NO_PACKSPECS		16								/**< Maximum number of packet parts expected by JPWL: increase at your will. */
#define JPWL_MAX_NO_MARKERS			512								/**< Maximum number of JPWL markers: increase at your will. */
#define JPWL_PRIVATEINDEX_NAME		"jpwl_index_privatefilename"	/**< index file name used when JPWL is on. */
#define JPWL_EXPECTED_COMPONENTS	3								/**< Expect this number of components, so you'll find better the first EPB. */
#define JPWL_MAXIMUM_TILES			8192							/**< Expect this maximum number of tiles, to avoid some crashes. */
#define JPWL_MAXIMUM_HAMMING		2								/**< Expect this maximum number of bit errors in marker id's. */
#define JPWL_MAXIMUM_EPB_ROOM		65450							/**< Expect this maximum number of bytes for composition of EPBs. */

/*
 * Stream open flags.
 */
/** The stream was opened for reading. */
#define OPJ_STREAM_READ				0x0001
/** The stream was opened for writing. */
#define OPJ_STREAM_WRITE			0x0002

/** Common fields between JPEG-2000 compression and decompression master structs. */
#define opj_common_fields \
	opj_event_mgr_t *				event_mgr;						/**< pointer to the event manager. */\
	void *						client_data;					/**< Available for use by application. */\
	LSBOOL							is_decompressor;				/**< So common code can tell which is which. */\
	OPJ_CODEC_FORMAT				codec_format;					/**< selected codec. */\
	void *						j2k_handle;						/**< pointer to the J2K codec. */\
	void *						jp2_handle;						/**< pointer to the JP2 codec. */\
	void *						mj2_handle						/**< pointer to the MJ2 codec. */

namespace lsi {

	// == Enumerations.
	/** 
	 * Rsiz capabilities.
	 */
	typedef enum RSIZ_CAPABILITIES {
		STD_RSIZ = 0, 				/** Standard JPEG2000 profile. */
		CINEMA2K = 3, 				/** Profile name for a 2K image. */
		CINEMA4K = 4				/** Profile name for a 4K image. */
	} OPJ_RSIZ_CAPABILITIES;

	/** 
	 * Digital cinema operation mode.
	 */
	typedef enum CINEMA_MODE {
		OFF = 0, 					/** Not Digital Cinema. */
		CINEMA2K_24 = 1, 			/** 2K Digital Cinema at 24 fps. */
		CINEMA2K_48 = 2, 			/** 2K Digital Cinema at 48 fps. */
		CINEMA4K_24 = 3				/** 4K Digital Cinema at 24 fps. */
	}OPJ_CINEMA_MODE;

	/** 
	 * Progression order.
	 */
	typedef enum PROG_ORDER {
		PROG_UNKNOWN = -1, 			/**< Place-holder. */
		LRCP = 0, 					/**< Layer-resolution-component-precinct order. */
		RLCP = 1, 					/**< Resolution-layer-component-precinct order. */
		RPCL = 2, 					/**< Resolution-precinct-component-layer order. */
		PCRL = 3, 					/**< Precinct-component-resolution-layer order. */
		CPRL = 4					/**< Component-precinct-resolution-layer order. */
	} OPJ_PROG_ORDER;

	/**
	 * Supported codec.
	 */
	typedef enum CODEC_FORMAT {
		CODEC_UNKNOWN = -1, 		/**< Place-holder. */
		CODEC_J2K = 0, 				/**< JPEG-2000 codestream: read/write. */
		CODEC_JPT = 1, 				/**< JPT-stream (JPEG 2000, JPIP): read only. */
		CODEC_JP2 = 2				/**< JPEG-2000 file format: read/write. */
	} OPJ_CODEC_FORMAT;

	/** 
	 * Limit decoding to certain portions of the codestream.
	 */
	typedef enum LIMIT_DECODING {
		NO_LIMITATION = 0, 			/**< No limitation for the decoding. The entire codestream will de decoded. */
		LIMIT_TO_MAIN_HEADER = 1, 	/**< The decoding is limited to the Main Header. */
		DECODE_ALL_BUT_PACKETS = 2	/**< Decode everything except the JPEG 2000 packets. */
	} OPJ_LIMIT_DECODING;

	/**
	 * Class CJpegCommonStructs
	 * \brief Common structures within the library.
	 *
	 * Description: Common structures within the library.
	 */
	class CJpegCommonStructs {
	public :
		// == Types.
		/**
		 * Callback function prototype for events
		 *
		 * \param msg Event message
		 * \param client_data 
		 */
		typedef void (LSE_CALL *									opj_msg_callback)( const char * msg, void * client_data );

		/**
		 * Message handler object used for 
		 * <ul>
		 * <li>Error messages
		 * <li>Warning messages
		 * <li>Debugging messages
		 * </ul>
		 */
		typedef struct opj_event_mgr {
			/** Error message callback if available, NULL otherwise. */
			opj_msg_callback										error_handler;
			/** Warning message callback if available, NULL otherwise. */
			opj_msg_callback										warning_handler;
			/** Debug message callback if available, NULL otherwise. */
			opj_msg_callback										info_handler;
		} opj_event_mgr_t;

		/**
		 * Progression order changes.
		 */
		typedef struct opj_poc {
			/** Resolution num start, Component num start, given by POC. */
			int32_t													resno0, compno0;
			/** Layer num end, Resolution num end, Component num end, given by POC. */
			int32_t													layno1, resno1, compno1;
			/** Layer num start, Precinct num start, Precinct num end. */
			int32_t													layno0, precno0, precno1;
			/** Progression order enum*/
			OPJ_PROG_ORDER											prg1, prg;
			/** Progression order string*/
			char													progorder[5];
			/** Tile number. */
			int32_t													tile;
			/** Start and end values for Tile width and height*/
			int32_t													tx0, tx1, ty0, ty1;
			/** Start value, initialised in pi_initialise_encode*/
			int32_t													layS, resS, compS, prcS;
			/** End value, initialised in pi_initialise_encode. */
			int32_t													layE, resE, compE, prcE;
			/** Start and end values of Tile width and height, initialised in pi_initialise_encode*/
			int32_t													txS, txE, tyS, tyE, dx, dy;
			/** Temporary values for Tile parts, initialised in pi_create_encode. */
			int32_t													lay_t, res_t, comp_t, prc_t, tx0_t, ty0_t;
		} opj_poc_t;

		/**
		 * Compression parameters.
		 */
		typedef struct opj_cparameters {
			/** Size of tile: tile_size_on = false (not in argument) or = true (in argument). */
			LSBOOL													tile_size_on;
			/** XTOsiz. */
			int32_t													cp_tx0;
			/** YTOsiz. */
			int32_t													cp_ty0;
			/** XTsiz. */
			int32_t													cp_tdx;
			/** YTsiz. */
			int32_t													cp_tdy;
			/** Allocation by rate/distortion. */
			int32_t													cp_disto_alloc;
			/** Allocation by fixed layer. */
			int32_t													cp_fixed_alloc;
			/** Adds fixed_quality. */
			int32_t													cp_fixed_quality;
			/** Fixed layer. */
			int32_t *												cp_matrice;
			/** Comment for coding. */
			char *												cp_comment;
			/** Coding style. */
			int32_t													csty;
			/** Progression order (default LRCP). */
			OPJ_PROG_ORDER											prog_order;
			/** Progression order changes. */
			opj_poc_t												POC[32];
			/** Number of progression order changes (POC), default to 0. */
			int32_t													numpocs;
			/** Number of layers. */
			int32_t													tcp_numlayers;
			/** Rates of layers. */
			float													tcp_rates[100];
			/** Different psnr for successive layers. */
			float													tcp_distoratio[100];
			/** Number of resolutions. */
			int32_t													numresolution;
			/** Initial code block width, default to 64. */
 			int32_t													cblockw_init;
			/** Initial code block height, default to 64. */
			int32_t													cblockh_init;
			/** Mode switch (cblk_style). */
			int32_t													mode;
			/** 1: Use the irreversible DWT 9-7, 0: Use lossless compression (default). */
			int32_t													irreversible;
			/** Region of interest: affected component in [0..3], -1 means no ROI. */
			int32_t													roi_compno;
			/** Region of interest: upshift value. */
			int32_t													roi_shift;
			/** Number of precinct size specifications. */
			int32_t													res_spec;
			/** Initial precinct width. */
			int32_t													prcw_init[J2K_MAXRLVLS];
			/** Initial precinct height. */
			int32_t													prch_init[J2K_MAXRLVLS];

			/** Input file name. */
			char													infile[OPJ_PATH_LEN];
			/** Output file name. */
			char													outfile[OPJ_PATH_LEN];
			/** DEPRECATED. Index generation is now handeld with the opj_encode_with_info() function. Set to NULL. */
			int32_t													index_on;
			/** DEPRECATED. Index generation is now handeld with the opj_encode_with_info() function. Set to NULL. */
			char													index[OPJ_PATH_LEN];
			/** Subimage encoding: origin image offset in x direction. */
			int32_t													image_offset_x0;
			/** Subimage encoding: origin image offset in y direction. */
			int32_t													image_offset_y0;
			/** Subsampling value for dx. */
			int32_t													subsampling_dx;
			/** Subsampling value for dy. */
			int32_t													subsampling_dy;
			/** Input file format 0: PGX, 1: PxM, 2: BMP 3:TIF. */
			int32_t													decod_format;
			/** Output file format 0: J2K, 1: JP2, 2: JPT. */
			int32_t													cod_format;


			/** Enables writing of EPC in MH, thus activating JPWL. */
			LSBOOL													jpwl_epc_on;
			/** Error protection method for MH (0, 1, 16, 32, 37-128). */
			int32_t													jpwl_hprot_MH;
			/** Tile number of header protection specification (>= 0). */
			int32_t													jpwl_hprot_TPH_tileno[JPWL_MAX_NO_TILESPECS];
			/** Error protection methods for TPHs (0, 1, 16, 32, 37-128). */
			int32_t													jpwl_hprot_TPH[JPWL_MAX_NO_TILESPECS];
			/** Tile number of packet protection specification (>= 0). */
			int32_t													jpwl_pprot_tileno[JPWL_MAX_NO_PACKSPECS];
			/** Packet number of packet protection specification (>= 0). */
			int32_t													jpwl_pprot_packno[JPWL_MAX_NO_PACKSPECS];
			/** Error protection methods for packets (0, 1, 16, 32, 37-128). */
			int32_t													jpwl_pprot[JPWL_MAX_NO_PACKSPECS];
			/** Enables writing of ESD, (0 = no/1/2 bytes). */
			int32_t													jpwl_sens_size;
			/** Sensitivity addressing size (0 = auto/2/4 bytes). */
			int32_t													jpwl_sens_addr;
			/** Sensitivity range (0-3). */
			int32_t													jpwl_sens_range;
			/** Sensitivity method for MH (-1 = no, 0-7). */
			int32_t													jpwl_sens_MH;
			/** Tile number of sensitivity specification ( >= 0). */
			int32_t													jpwl_sens_TPH_tileno[JPWL_MAX_NO_TILESPECS];
			/** Sensitivity methods for TPHs (-1 = no, 0-7). */
			int32_t													jpwl_sens_TPH[JPWL_MAX_NO_TILESPECS];

			/** Digital Cinema compliance 0-not compliant, 1-compliant. */
			OPJ_CINEMA_MODE											cp_cinema;
			/** Maximum rate for each component.  If == 0, component size limitation is not considered. */
			int32_t													max_comp_size;
			/** Profile name. */
			OPJ_RSIZ_CAPABILITIES									cp_rsiz;
			/** Tile part generation. */
			char													tp_on;
			/** Flag for Tile part generation. */
			char													tp_flag;
			/** MCT (multiple component transform). */
			char													tcp_mct;
		} opj_cparameters_t;

		/**
		 * Decompression parameters.
		 */
		typedef struct opj_dparameters {
			/** 
			 * Sets the number of highest resolution levels to be discarded. 
			 * The image resolution is effectively divided by 2 to the power of the number of discarded levels. 
			 * The reduce factor is limited by the smallest total number of decomposition levels among tiles.
			 *	if != 0, then original dimension divided by 2^(reduce); 
			 *	if == 0 or not used, image is decoded to the full resolution 
			 */
			int32_t													cp_reduce;
			/** 
			 * Sets the maximum number of quality layers to decode. 
			 * If there are less quality layers than the specified number, all the quality layers are decoded.
			 *	if != 0, then only the first "layer" layers are decoded; 
			 *	if == 0 or not used, all the quality layers are decoded 
			*/
			int32_t													cp_layer;

			/** Input file name. */
			char													infile[OPJ_PATH_LEN];
			/** Output file name. */
			char													outfile[OPJ_PATH_LEN];
			/** Input file format 0: J2K, 1: JP2, 2: JPT. */
			int32_t													decod_format;
			/** Output file format 0: PGX, 1: PxM, 2: BMP. */
			int32_t													cod_format;

			/** Activates the JPWL correction capabilities. */
			LSBOOL													jpwl_correct;
			/** Expected number of components. */
			int32_t													jpwl_exp_comps;
			/** Maximum number of tiles. */
			int32_t													jpwl_max_tiles;

			/** 
			 * Specify whether the decoding should be done on the entire codestream, or be limited to the main header
			 *	Limiting the decoding to the main header makes it possible to extract the characteristics of the codestream
			 *	if == NO_LIMITATION, the entire codestream is decoded; 
			 *	if == LIMIT_TO_MAIN_HEADER, only the main header is decoded; 
			 */
			OPJ_LIMIT_DECODING										cp_limit_decoding;

		} opj_dparameters_t;

		/**
		 * Routines that are to be used by both halves of the library are declared
		 *	to receive a pointer to this structure.  There are no actual instances of
		 *	opj_common_struct_t, only of opj_cinfo_t and opj_dinfo_t.
		 */
		typedef struct opj_common_struct {
			/* Fields common to both master struct types. */
			opj_common_fields;
			/* Additional fields follow in an actual opj_cinfo_t or
			 * opj_dinfo_t.  All three structs must agree on these
			 * initial fields!  (This would be a lot cleaner in C++.)
			 */
		} opj_common_struct_t;

		typedef opj_common_struct_t *								opj_common_ptr;

		/**
		 * Compression context info.
		 */
		typedef struct opj_cinfo {
			/** Fields shared with opj_dinfo_t. */
			opj_common_fields;	
			/* Other specific fields go here. */
		} opj_cinfo_t;

		/**
		 * Decompression context info
		 */
		typedef struct opj_dinfo {
			/** Fields shared with opj_cinfo_t. */
			opj_common_fields;	
			/* Other specific fields go here. */
		} opj_dinfo_t;

		/**
		 * Index structure: Information concerning a packet inside tile.
		 */
		typedef struct opj_packet_info {
			/** Packet start position (including SOP marker if it exists). */
			int32_t													start_pos;
			/** End of packet header position (including EPH marker if it exists)*/
			int32_t													end_ph_pos;
			/** Packet end position. */
			int32_t													end_pos;
			/** Packet distorsion. */
			double												disto;
		} opj_packet_info_t;

		/**
		 * Index structure: Information concerning tile-parts.
		 */
		typedef struct opj_tp_info {
			/** Start position of tile part. */
			int32_t													tp_start_pos;
			/** End position of tile part header. */
			int32_t													tp_end_header;
			/** End position of tile part. */
			int32_t													tp_end_pos;
			/** Start packet of tile part. */
			int32_t													tp_start_pack;
			/** Number of packets of tile part. */
			int32_t													tp_numpacks;
		} opj_tp_info_t;

		/**
		 * Index structure: information regarding tiles.
		 */
		typedef struct opj_tile_info {
			/** Value of thresh for each layer by tile cfr. Marcela. */
			double *												thresh;
			/** Number of tile. */
			int32_t													tileno;
			/** Start position. */
			int32_t													start_pos;
			/** End position of the header. */
			int32_t													end_header;
			/** End position. */
			int32_t													end_pos;
			/** Precinct number for each resolution level (width). */
			int32_t													pw[33];
			/** Precinct number for each resolution level (height). */
			int32_t													ph[33];
			/** Precinct size (in power of 2), in X for each resolution level. */
			int32_t													pdx[33];
			/** Precinct size (in power of 2), in Y for each resolution level. */
			int32_t													pdy[33];
			/** Information concerning packets inside tile. */
			opj_packet_info_t *										packet;
			/** Adds fixed_quality. */
			int32_t													numpix;
			/** Adds fixed_quality. */
			double												distotile;
			/** Number of tile parts. */
			int32_t													num_tps;
			/** Information concerning tile parts. */
			opj_tp_info_t *											tp;
		} opj_tile_info_t;

		/**
		 * Marker structure.
		 */
		typedef struct opj_marker_info_t {
			/** Marker type. */
			uint16_t												type;
			/** Position in codestream. */
			int32_t													pos;
			/** Length, marker val included. */
			int32_t													len;
		} opj_marker_info_t;

		/**
		 * Index structure of the codestream.
		*/
		typedef struct opj_codestream_info {
			/** Maximum distortion reduction on the whole image (add for Marcela). */
			double												D_max;
			/** Packet number. */
			int32_t													packno;
			/** Writing the packet in the index with t2_encode_packets. */
			int32_t													index_write;
			/** Image width. */
			int32_t													image_w;
			/** Image height. */
			int32_t													image_h;
			/** Progression order. */
			OPJ_PROG_ORDER											prog;
			/** Tile size in x. */
			int32_t													tile_x;
			/** Tile size in y. */
			int32_t													tile_y;
			int32_t													tile_Ox;
			int32_t													tile_Oy;
			/** Number of tiles in X. */
			int32_t													tw;
			/** Number of tiles in Y. */
			int32_t													th;
			/** Component numbers. */
			int32_t													numcomps;
			/** Number of layer. */
			int32_t													numlayers;
			/** Number of decomposition for each component. */
			int32_t *												numdecompos;
			/** Number of markers. */
			int32_t													marknum;
			/** List of markers. */
			opj_marker_info_t *										marker;
			/** Actual size of markers array. */
			int32_t													maxmarknum;
			/** Main header position. */
			int32_t													main_head_start;
			/** Main header position. */
			int32_t													main_head_end;
			/** Codestream's size. */
			int32_t													codestream_size;
			/** Information regarding tiles inside image. */
			opj_tile_info_t *										tile;
		} opj_codestream_info_t;
	};

}	// namespace lsi

#endif	// __LSI_JPEGCOMMONSTRUCTS_H__

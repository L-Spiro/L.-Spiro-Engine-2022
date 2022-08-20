/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Interface with the Vulkan API.
 */


#ifndef __LSG_VULKAN_H__
#define __LSG_VULKAN_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_VKN

#ifdef LSE_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif	// #ifdef LSE_WINDOWS

#include "../Gfx/LSGGfxBase.h"
#include "../VulkanCommandBuffer/LSGVulkanCommandBuffer.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "Map/LSTLLinearMap.h"

#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.h>

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Enumerations.
	/** The blend types we support.  These are for blending to the backbuffer only. */
	enum LSG_BLEND_MODE {
		LSG_BM_ZERO									= VK_BLEND_FACTOR_ZERO,							/**< Blend factor is (0, 0, 0, 0). */
		LSG_BM_ONE									= VK_BLEND_FACTOR_ONE,							/**< Blend factor is (1, 1, 1, 1). */
		LSG_BM_SRC_COLOR							= VK_BLEND_FACTOR_SRC_COLOR,					/**< Blend factor is (Rs, Gs, Bs, As). */
		LSG_BM_INV_SRC_COLOR						= VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,			/**< Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		LSG_BM_DST_COLOR							= VK_BLEND_FACTOR_DST_COLOR,					/**< Blend factor is (Rd, Gd, Bd, Ad). */
		LSG_BM_INV_DST_COLOR						= VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,			/**< Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		LSG_BM_SRC_ALPHA							= VK_BLEND_FACTOR_SRC_ALPHA,					/**< Blend factor is (As, As, As, As). */
		LSG_BM_INV_SRC_ALPHA						= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,			/**< Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		LSG_BM_DST_ALPHA							= VK_BLEND_FACTOR_DST_ALPHA,					/**< Blend factor is (Ad Ad Ad Ad). */
		LSG_BM_INV_DEST_ALPHA						= VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,			/**< Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		LSG_BM_SRC_ALPHA_SAT						= VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,			/**< Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */

		LSG_BM_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** The blend operations we support. */
	enum LSG_BLEND_OP {
		LSG_BO_ADD									= VK_BLEND_OP_ADD,								/**< Add source 1 and source 2. */
		LSG_BO_SUBTRACT								= VK_BLEND_OP_SUBTRACT,							/**< Subtract source 1 and source 2. */
		LSG_BO_REV_SUBTRACT							= VK_BLEND_OP_REVERSE_SUBTRACT,					/**< Subtract source 2 from source 1. */
		LSG_BO_MIN									= VK_BLEND_OP_MIN,								/**< Find the minimum of source 1 and source 2. */
		LSG_BO_MAX									= VK_BLEND_OP_MAX,								/**< Find the maximum of source 1 and source 2. */

		LSG_BO_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** Buffers. */
	enum LSG_BUFFER {
		LSG_B_COLOR_BUFFER							= 1 << 0,										/**< Clear a render target. */
		LSG_B_STENCIL_BUFFER						= 1 << 1,										/**< Clear the stencil buffer. */
		LSG_B_DEPTH_BUFFER							= 1 << 2,										/**< Clear the depth buffer. */
		LSG_B_ALL									= LSG_B_COLOR_BUFFER | LSG_B_STENCIL_BUFFER |
			LSG_B_DEPTH_BUFFER,																		/**< Clear all buffers. */
	};

	/** The color-write modes. */
	enum LSG_COLOR_WRITE_ENABLE {
		LSG_CWM_RED									= VK_COLOR_COMPONENT_R_BIT,						/**< Allow data to be stored in the red component. */
		LSG_CWM_GREEN								= VK_COLOR_COMPONENT_G_BIT,						/**< Allow data to be stored in the green component. */
		LSG_CWM_BLUE								= VK_COLOR_COMPONENT_B_BIT,						/**< Allow data to be stored in the blue component. */
		LSG_CWM_ALPHA								= VK_COLOR_COMPONENT_A_BIT,						/**< Allow data to be stored in the alpha component. */
		LSG_CWM_ALL									= LSG_CWM_RED | LSG_CWM_GREEN | LSG_CWM_BLUE |
			LSG_CWM_ALPHA,																			/**< Allow data to be stored in all components. */
	};

	/** Comparison functions. */
	enum LSG_COMPARISON_FUNC {
		LSG_CF_NEVER								= VK_COMPARE_OP_NEVER,							/**< Never pass the comparison. */
		LSG_CF_ALWAYS								= VK_COMPARE_OP_ALWAYS,							/**< Always pass the comparison. */
		LSG_CF_LESS									= VK_COMPARE_OP_LESS,							/**< If the source data is less than the destination data, the comparison passes. */
		LSG_CF_EQUAL								= VK_COMPARE_OP_EQUAL,							/**< If the source data is equal to the destination data, the comparison passes. */
		LSG_CF_NOTEQUAL								= VK_COMPARE_OP_NOT_EQUAL,						/**< If the source data is not equal to the destination data, the comparison passes. */
		LSG_CF_LESSEQUAL							= VK_COMPARE_OP_LESS_OR_EQUAL,					/**< If the source data is less than or equal to the destination data, the comparison passes. */
		LSG_CF_GREATER								= VK_COMPARE_OP_GREATER,						/**< If the source data is greater than the destination data, the comparison passes. */
		LSG_CF_GREATEREQUAL							= VK_COMPARE_OP_GREATER_OR_EQUAL,				/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	};

	/** Culling modes. */
	enum LSG_CULL_MODE {
		LSG_CM_NONE									= VK_CULL_MODE_NONE,							/**< No culling. */
		LSG_CM_FRONT								= VK_CULL_MODE_FRONT_BIT,						/**< Cull front faces. */
		LSG_CM_BACK									= VK_CULL_MODE_BACK_BIT,						/**< Cull back faces. */
	};

	/** Depth write masks. */
	enum LSG_DEPTH_WRITE_MASK {
		LSG_DRM_ZERO								= 0,											/**< Turn off writes to the depth-stencil buffer. */
		LSG_DRM_ALL									= 1,											/**< Turn on writes to the depth-stencil buffer. */
	};

	/** Fill modes. */
	enum LSG_FILL_MODE {
		LSG_FM_SOLID								= VK_POLYGON_MODE_FILL,							/**< Solid filling mode. */
		LSG_FM_WIRE									= VK_POLYGON_MODE_LINE,							/**< Wire-frame filling mode. */
	};

	/** Filters. */
	enum LSG_FILTER {
		/** Use point sampling for minification, magnification, and mip-level sampling. */
		LSG_F_MIN_MAG_MIP_POINT						= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_POINT, LSG_FILTER_POINT, false ),
		/** Use point sampling for minification and magnification; use linear interpolation for mip-level sampling. */
		LSG_F_MIN_MAG_POINT_MIP_LINEAR				= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_POINT, LSG_FILTER_LINEAR, false ),
		/** Use point sampling for minification; use linear interpolation for magnification; use point sampling for mip-level sampling. */
		LSG_F_MIN_POINT_MAG_LINEAR_MIP_POINT		= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_LINEAR, LSG_FILTER_POINT, false ),
		/** Use point sampling for minification; use linear interpolation for magnification and mip-level sampling. */
		LSG_F_MIN_POINT_MAG_MIP_LINEAR				= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, false ),
		/** Use linear interpolation for minification; use point sampling for magnification and mip-level sampling. */
		LSG_F_MIN_LINEAR_MAG_MIP_POINT				= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_POINT, LSG_FILTER_POINT, false ),
		/** Use linear interpolation for minification; use point sampling for magnification; use linear interpolation for mip-level sampling. */
		LSG_F_MIN_LINEAR_MAG_POINT_MIP_LINEAR		= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_POINT, LSG_FILTER_LINEAR, false ),
		/** Use linear interpolation for minification and magnification; use point sampling for mip-level sampling. */
		LSG_F_MIN_MAG_LINEAR_MIP_POINT				= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, LSG_FILTER_POINT, false ),
		/** Use linear interpolation for minification, magnification, and mip-level sampling. */
		LSG_F_MIN_MAG_MIP_LINEAR					= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, false ),
		/** Use anisotropic interpolation for minification, magnification, and mip-level sampling. */
		LSG_F_ANISOTROPIC							= LSG_ENCODE_ANISOTROPIC_FILTER( false ),
	};

	/** Stencil operations. */
	enum LSG_STENCIL_OP {
		LSG_SO_KEEP									= VK_STENCIL_OP_KEEP,							/**< Keep the existing stencil data. */
		LSG_SO_ZERO									= VK_STENCIL_OP_ZERO,							/**< Set the stencil data to 0. */
		LSG_SO_REPLACE								= VK_STENCIL_OP_REPLACE,						/**< Set the stencil data to the reference value. */
		LSG_SO_INC_SAT								= VK_STENCIL_OP_INCREMENT_AND_CLAMP,			/**< Increment the stencil value by 1, and clamp the result. */
		LSG_SO_DEC_SAT								= VK_STENCIL_OP_DECREMENT_AND_CLAMP,			/**< Decrement the stencil value by 1, and clamp the result. */
		LSG_SO_INVERT								= VK_STENCIL_OP_INVERT,							/**< Invert the stencil data. */
		LSG_SO_INC									= VK_STENCIL_OP_INCREMENT_AND_WRAP,				/**< Increment the stencil value by 1, and wrap the result if necessary. */
		LSG_SO_DEC									= VK_STENCIL_OP_DECREMENT_AND_WRAP,				/**< Decrement the stencil value by 1, and wrap the result if necessary. */
	};

	/** Texture addressing modes. */
	enum LSG_TEXTURE_ADDRESS_MODE {
		LSG_TAM_WRAP								= VK_SAMPLER_ADDRESS_MODE_REPEAT,				/**< Tile the texture at every (u,v) integer junction. */
		LSG_TAM_MIRROR								= VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,		/**< Flip the texture at every (u,v) integer junction. */
		LSG_TAM_CLAMP								= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,		/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		LSG_TAM_BORDER								= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,		/**< Texture coordinates outside the range [0.0, 1.0] are set to the border color. */
		LSG_TAM_MIRROR_ONCE							= VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,	/**< Similar to LSG_TAM_MIRROR and LSG_TAM_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */
	};

	/** Compile-time metrics.  For run-time, use the metrics structure returned by the graphics device. */
	enum LSG_METRICS {
		LSG_MAX_TEXTURE_UNITS						= 32,											/**< Total texture units bindable to a shader at a time. */
		LSG_MAX_SHADER_SAMPLERS						= 32,											/**< Total number of samplers that can be set on a shader at once. */
		LSG_MAX_RENDER_TARGET_COUNT					= 8,											/**< Total simultaneous render-target count. */
		LSG_MAX_VIEWPORTS_SCISSORS					= 1,											/**< Maximum number of viewports and scissors. */
		LSG_VIEWPORT_MIN							= 0,											/**< Minimum viewport bounds. */
		LSG_VIEWPORT_MAX							= 4096,											/**< Maximum viewport bounds. */
		LSG_MAX_VERTEX_ELEMENT_COUNT				= 16,											/**< Maximum number of inputs to a vertex shader. */
		LSG_MAX_VERTEX_STREAMS						= 16,											/**< Maximum number of simultaneous vertex buffers bound. */
		LSG_MAX_VERTEX_SEMANTIC						= 16,											/**< Maximum number of vertex buffer semantic indices. */
		
		LSG_FORCE_DWORD								= 0x7FFFFFFF
	};

	// Floating-point metrics.
#define LSG_MIN_DEPTH								(0.0f)											/**< Minimum depth value. */
#define LSG_MAX_DEPTH								(1.0f)											/**< Maximum depth value. */

	// Default values.
#define LSG_DEFAULT_STENCIL_READ_MASK				(1)												/**< Default stencil read mask. */
#define LSG_DEFAULT_STENCIL_WRITE_MASK				(1)												/**< Default stencil write mask. */

#define LSG_FENCE_TIMEOUT							100000000										/**< Nanoseconds to wait for a fence. */


	// == Types.
	class											CFramebuffer;
	typedef class CVulkanShaderManager				CShaderManager;
	class											CShader;
	class											CVertexBuffer;

#include "../Gfx/LSGDescStructs.h"

	/** The 1D texture type. */
	typedef VkImage									LSG_TEXTURE_1D;

	/** The 2D texture type. */
	typedef VkImage									LSG_TEXTURE_2D;

	/** The 3D texture type. */
	typedef VkImage									LSG_TEXTURE_3D;

	/** The sampler type. */
	typedef LSG_SAMPLER_DESC						LSG_SAMPLER_STATE;

	/** The shader resource view type. */
	typedef VkImageView								LSG_SHADER_RESOURCE;

	/** The render target type. */
	typedef VkImage									LSG_RENDER_TARGET;

	/** The depth/stencil type. */
	typedef VkImage									LSG_DEPTH_STENCIL;

	/** The rasterizer state type. */
	typedef LSG_RASTERIZER_DESC						LSG_RASTERIZER_STATE;

	/** The blend state type. */
	typedef LSG_BLEND_DESC							LSG_BLEND_STATE;

	/** The depth/stencil state. */
	typedef LSG_DEPTH_STENCIL_DESC					LSG_DEPTH_STENCIL_STATE;

	/** The viewport type. */
	typedef VkViewport								LSG_VIEWPORT;

	/** The scissor rectangle type. */
	typedef struct LSG_RECT {
		int32_t										left;											/**< The x-coordinate of the upper-left corner of the rectangle. */
		int32_t										top;											/**< The y-coordinate of the upper-left corner of the rectangle. */
		uint32_t									right;											/**< The x-coordinate of the lower-right corner of the rectangle. */
		uint32_t									bottom;											/**< The y-coordinate of the lower-right corner of the rectangle. */
	} * LPLSG_RECT, * const LPCLSG_RECT;

	/** The input layout type. */
	typedef void *								LSG_INPUT_LAYOUT;

	/** The vertex buffer type. */
	typedef struct LSG_VERTEX_BUFFER {
		const CVertexBuffer *						pvbVertBuffer;									/**< The vertex buffer pointer. */
		uint32_t									ui32Offset;										/**< Offset of the vertex buffer. */
	} * LPLSG_VERTEX_BUFFER, * const LPCLSG_VERTEX_BUFFER;

	// Create the render-state structure.
#include "../Gfx/LSGGfxState.h"

	/**
	 * Class CVulkan
	 * \brief Interface with the Vulkan API.
	 *
	 * Description: Interface with the Vulkan API.
	 */
	class CVulkan : public CGfxBase {
		friend class								CVulkanIndexBuffer;
		friend class								CVulkanTexture2d;
		friend class								CVulkanVertexBuffer;
	public :
		// == Types.
		/** Data needed to initialize the device. */
		typedef struct LSG_VULKAN_INIT {
#ifdef LSE_WINDOWS
			/** The window handle. */
			HWND									hWnd;

			/** The application instance. */
			HINSTANCE								hInst;
#endif	// #ifdef LSE_WINDOWS

			/** UTF-8 window name. */
			const char *							pcWindowName;

			/** Application version. */
			uint32_t								ui32AppVersion;
		} * LPLSG_VULKAN_INIT, * const LPCLSG_VULKAN_INIT;

		/** A wrapper for VkImage. */
		class CImage {
		public :
			// == Various constructors.
			LSE_CALLCTOR							CImage();
			LSE_CALLCTOR							~CImage();


			// == Operators.
			LSE_INLINE CImage & LSE_FCALL			operator = ( CImage &_iVal ) {
				DestroyImage( m_pacAllocator );
				m_iImage = _iVal.m_iImage;
				m_pacAllocator = _iVal.m_pacAllocator;

				_iVal.m_iImage = VK_NULL_HANDLE;
				_iVal.m_pacAllocator = NULL;
				return (*this);
			}
			
			
			// == Functions.
			/**
			 * Creates the image object.
			 *
			 * \param _piciCreateInfo A pointer to an instance of the VkImageCreateInfo structure containing parameters to be used to create the image.
			 * \param _pacAllocator Controls host memory allocation.
			 * \return Returns true if the VkImage object was created.
			 */
			LSBOOL LSE_CALL							CreateImage( const VkImageCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = NULL );

			/**
			 * Destroys the image object.
			 *
			 * \param _pacAllocator Controls host memory allocation.
			 */
			void LSE_CALL							DestroyImage( const VkAllocationCallbacks * _pacAllocator = NULL );

			/**
			 * Gets the VkImage object.
			 *
			 * \return Returns the VkImage object.
			 */
			LSE_INLINE VkImage LSE_CALL				Image() { return m_iImage; }

			
		protected :
			// == Members.
			VkImage									m_iImage;										/**< The image object to wrap. */
			const VkAllocationCallbacks *			m_pacAllocator;									/**< Last allocator used. */
		};

		/** A wrapper for VkDeviceMemory. */
		class CDeviceMemory {
		public :
			// == Various constructors.
			LSE_CALLCTOR							CDeviceMemory();
			LSE_CALLCTOR							~CDeviceMemory();


			// == Operators.
			LSE_INLINE CDeviceMemory & LSE_FCALL	operator = ( CDeviceMemory &_iVal ) {
				FreeMemory( m_pacAllocator );
				m_dmDeviceMemory = _iVal.m_dmDeviceMemory;
				m_pacAllocator = _iVal.m_pacAllocator;

				_iVal.m_dmDeviceMemory = VK_NULL_HANDLE;
				_iVal.m_pacAllocator = NULL;
				return (*this);
			}
			
			
			// == Functions.
			/**
			 * Creates the image object.
			 *
			 * \param _pmaiAllocateInfo A pointer to a structure of type VkMemoryAllocateInfo, which contains parameters of the allocation. A successful returned allocation must use the requested parameters--no substitution is permitted by the implementation.
			 * \param _pacAllocator Controls host memory allocation.
			 * \return Returns true if the VkDeviceMemory object was created.
			 */
			LSBOOL LSE_CALL							AllocateMemory( const VkMemoryAllocateInfo * _pmaiAllocateInfo, const VkAllocationCallbacks * _pacAllocator = NULL );

			/**
			 * Destroys the image object.
			 *
			 * \param _pacAllocator Controls host memory allocation.
			 */
			void LSE_CALL							FreeMemory( const VkAllocationCallbacks * _pacAllocator = NULL );

			/**
			 * Gets the VkDeviceMemory object.
			 *
			 * \return Returns the VkDeviceMemory object.
			 */
			LSE_INLINE VkDeviceMemory LSE_CALL		DeviceMemory() { return m_dmDeviceMemory; }

			
		protected :
			// == Members.
			VkDeviceMemory							m_dmDeviceMemory;								/**< The memory object to wrap. */
			const VkAllocationCallbacks *			m_pacAllocator;									/**< Last allocator used. */
		};

		/** A wrapper for VkBuffer. */
		class CBuffer {
		public :
			// == Various constructors.
			LSE_CALLCTOR							CBuffer();
			LSE_CALLCTOR							~CBuffer();


			// == Operators.
			LSE_INLINE CBuffer & LSE_FCALL	operator = ( CBuffer &_iVal ) {
				DestroyBuffer( m_pacAllocator );
				m_bBuffer = _iVal.m_bBuffer;
				m_pacAllocator = _iVal.m_pacAllocator;

				_iVal.m_bBuffer = VK_NULL_HANDLE;
				_iVal.m_pacAllocator = NULL;
				return (*this);
			}


			// == Functions.
			/**
			 * Creates the image object.
			 *
			 * \param _pbciCreateInfo A pointer to an instance of the VkBufferCreateInfo structure containing parameters affecting creation of the buffer.
			 * \param _pacAllocator Controls host memory allocation.
			 * \return Returns true if the VkDeviceMemory object was created.
			 */
			LSBOOL LSE_CALL							CreateBuffer( const VkBufferCreateInfo * _pbciCreateInfo, const VkAllocationCallbacks * _pacAllocator = NULL );

			/**
			 * Destroys the buffer object.
			 *
			 * \param _pacAllocator Controls host memory allocation.
			 */
			void LSE_CALL							DestroyBuffer( const VkAllocationCallbacks * _pacAllocator = NULL );

			/**
			 * Gets the VkBuffer object.
			 *
			 * \return Returns the VkBuffer object.
			 */
			LSE_INLINE VkBuffer LSE_CALL			Buffer() { return m_bBuffer; }

			/**
			 * Gets the VkBuffer object.
			 *
			 * \return Returns the VkBuffer object.
			 */
			LSE_INLINE const VkBuffer LSE_CALL		Buffer() const { return m_bBuffer; }


		protected :
			// == Members.
			VkBuffer								m_bBuffer;										/**< The memory object to wrap. */
			const VkAllocationCallbacks *			m_pacAllocator;									/**< Last allocator used. */
		};


		// == Functions.
		/**
		 * Initialize Vulkan.
		 *
		 * \param _lpdiInit Data required to initialize the device.
		 * \return Returns false if there are any problem during initialization.
		 */
		static LSBOOL LSE_CALL						InitVulkan( LSG_VULKAN_INIT * _pvInit );

		/**
		 * Secondary initialization of Vulkan, after a device has been created.
		 */
		static void LSE_CALL						PostInitVulkan();

		/**
		 * Destroy Vulkan one time at engine shutdown.
		 */
		static void LSE_CALL						DestroyVulkan();

		/**
		 * Gets the string form of a Vulkan error code.
		 *
		 * \param _rRes The error code whose string to get.
		 * \return Returns the string form of the given error code.
		 */
		static const char * LSE_CALL				VulkanErrorToString( VkResult _rRes );

		/**
		 * Prints a Vulkan error, if any.  If there is no error, false is returned.
		 *
		 * \param _pcPrefix Error prefix.
		 * \param _rRes The error code.
		 * \return Returns true if an error was printed.
		 */
		static LSBOOL LSE_CALL						PrintVulkanError( const char * _pcPrefix, VkResult _rRes );

		/**
		 * Gets the Vulkan device.
		 *
		 * \return Returns the Vulkan device.
		 */
		static LSE_INLINE VkDevice LSE_CALL			GetVulkanDevice();

		/**
		 * Gets the physical Vulkan device.
		 *
		 * \return Returns the physical Vulkan device.
		 */
		static LSE_INLINE VkPhysicalDevice LSE_CALL	GetVulkanPhysicalDevice();

		/**
		 * Gets the command pool.
		 *
		 * \param _cpPool On success, this holds the return value.
		 * \return Returns the Vulkan command pool.
		 */
		static LSE_INLINE VkCommandPool LSE_CALL	GetVulkanCommandPool();

		/**
		 * Gets the Vulkan queue.
		 *
		 * \return Returns the Vulkan queue.
		 */
		static LSE_INLINE VkQueue LSE_CALL			GetVulkanQueue();

		/**
		 * Gets the main command buffer.
		 *
		 * \return Returns the main command buffer.
		 */
		static LSE_INLINE CVulkanCommandBuffer & LSE_CALL
													GetVulkanCommandBuffer();

		/**
		 * Allocates memory.  Always use this instead of ::vkAllocateMemory().  Internally, this keeps track of all memory allocated to give statistical metrics.
		 *
		 * \param _pmaiAllocateInfo A pointer to a structure of type VkMemoryAllocateInfo, which contains the parameters of the allocation. A successful returned allocation must use the requested parameters--no substitution is permitted by the implementation.
		 * \param _pacAllocator Controls host memory allocation.
		 * \param _dmMemory A reference to a VkDeviceMemory structure in which information about the allocated memory is returned.
		 * \return Returns a Vulkan error code indicating success or failure.
		 */
		static LSE_INLINE VkResult LSE_CALL			AllocateMemory( const VkMemoryAllocateInfo * _pmaiAllocateInfo,
			const VkAllocationCallbacks * _pacAllocator,
			VkDeviceMemory &_dmMemory );

		/**
		 * Frees memory created by AllocateMemory().  Never call vkFreeMemory() directly, as this is used to keep track of total allocated memory.
		 *
		 * \param _dmMemory The VkDeviceMemory object to be freed.
		 * \param _pacAllocator Controls host memory allocation.
		 */
		static LSE_INLINE void LSE_CALL				FreeMemory( VkDeviceMemory &_dmMemory,
			const VkAllocationCallbacks * _pacAllocator );

		/**
		 * Gets the total amount of allocated graphics memory.
		 *
		 * \return Returns the total amount of allocated graphics memory.
		 */
		static LSE_INLINE uint64_t LSE_CALL			TotalAllocatedMemory();

		/**
		 * Gets the memory type based on the given properties.
		 *
		 * \param _ui32TypeBits Type bits returned by functions that get memory requirements of images, buffers, etc.
		 * \param _fRequirementsMask Mask of requirements.
		 * \param _ui32TypeIndex The return value.
		 * \return Returns true if the requirements are met for the given memory properties.
		 */
		static LSBOOL LSE_CALL						MemoryTypeFromProperties( uint32_t _ui32TypeBits,
			VkFlags _fRequirementsMask, uint32_t &_ui32TypeIndex );

		/**
		 * Sets default states for between-object rendering.  Does not change viewports, render targets, shader
		 *	resources, etc.
		 */
		static LSE_INLINE void LSE_FCALL			SetDefaultObjectRenderStates();

		/**
		 * Sets all default render states.
		 */
		static LSE_INLINE void LSE_FCALL			SetAllDefaultRenderStates();

		/**
		 * Sets and applies all default render states.
		 */
		static void LSE_CALL						ApplyDefaultStates();

		/**
		 * Applies all current render states.
		 *
		 * \param _bForce If true, all states are applied, even if they are redundant.
		 */
		static void LSE_CALL						ApplyRenderStates( LSBOOL _bForce = false );

		/**
		 * Sets the current shader.  A shader must be set in order to render.
		 *
		 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
		 *	without a shader is not permitted.
		 */
		static void LSE_FCALL						SetShader( CShader * _psShader );

		/**
		 * Gets the current shader.  If no shader is set, NULL is returned.
		 *
		 * \return Returns the current shader or NULL.
		 */
		static CShader * LSE_FCALL					GetShader();

		/**
		 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
		 *
		 * \return Returns true if the rendering device is ready.
		 */
		static LSE_INLINE LSBOOL LSE_FCALL			BeginRender();

		/**
		 * End rendering.  Must be called once per frame only, after all rendering is done.
		 */
		static LSE_INLINE void LSE_FCALL			EndRender();

		/**
		 * Present the backbuffer to the screen.
		 */
		static LSE_INLINE void LSE_FCALL			Present();

		/**
		 * Creates a perspective projection matrix based on a field of view.
		 *
		 * \param _mIn The output matrix.
		 * \param _fFov Field of view in the y direction, in radians.
		 * \param _fAspect Aspect ratio, defined as view space width divided by height.
		 * \param _fNear Z-value of the near view-plane.
		 * \param _fFar Z-value of the far view-plane.
		 * \return Returns the input matrix so that this function can be used as a parameter to another function.
		 */
		static LSE_INLINE CMatrix4x4 & LSE_CALL		PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _tFov, LSREAL _tAspect, LSREAL _tNear, LSREAL _tFar );

		/**
		 * Creates an orthogonal projection matrix.
		 *
		 * \param _mIn The output matrix.
		 * \param _fLeft Minimum x-value of the view volume.
		 * \param _fRight Maximum x-value of the view volume.
		 * \param _fBottom Minimum y-value of the view volume.
		 * \param _fTop Maximum y-value of the view volume.
		 * \param _fNear Minimum z-value of the view volume.
		 * \param _fFar Maximum z-value of the view volume.
		 * \return Returns the input matrix so that this function can be used as a parameter to another function.
		 */
		static LSE_INLINE CMatrix4x4 & LSE_CALL		OrthoOffCenter( CMatrix4x4 &_mIn,
			LSREAL _fLeft, LSREAL _fRight,
			LSREAL _fBottom, LSREAL _fTop,
			LSREAL _fNear, LSREAL _fFar );

		/**
		 * Extracts the frustum planes from the given projection matrix.
		 *
		 * \param _mMatrix Matrix from which to extract the frustum planes.
		 * \param _fFrustum The frustum to which the planes are extracted.
		 * \param _bNormalize If true, the planes are normalized.
		 * \return Returns the reference to _fFrustum.
		 */
		static LSE_INLINE CFrustum & LSE_CALL		ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize = true );

		/**
		 * Clear the active buffer of the given type(s).
		 *
		 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
		 */
		static void LSE_FCALL						Clear( uint32_t _ui32Mask );

		/**
		 * Clear all active buffers.
		 */
		static LSE_INLINE void LSE_FCALL			Clear();

		/**
		 * Sets the clear color.
		 *
		 * \param _fR Red value.
		 * \param _fG Green value.
		 * \param _fB Blue value.
		 * \param _fA Alpha value.
		 */
		static LSE_INLINE void LSE_CALL				SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA );

		/**
		 * Sets the depth clear value.
		 *
		 * \param _fValue The new depth clear value.
		 */
		static LSE_INLINE void LSE_FCALL			SetClearDepth( LSREAL _fValue );

		/**
		 * Sets the stencil clear value.
		 *
		 * \param _ui8Val The new stencil clear value.
		 */
		static LSE_INLINE void LSE_FCALL			SetClearStencil( uint8_t _ui8Val );

		/**
		 * Sets the viewport dimensions for the given render target.
		 *
		 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fWidth Width of the viewport.
		 * \param _fHeight Height of the viewport.
		 * \param _ui32Target Index of the viewport to set.
		 */
		static LSE_INLINE void LSE_CALL				SetViewport( LSREAL _fX, LSREAL _fY,
			LSREAL _fWidth, LSREAL _fHeight, uint32_t _ui32Target = 0 );

		/**
		 * Sets the viewport dimensions.
		 *
		 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fWidth Width of the viewport.
		 * \param _fHeight Height of the viewport.
		 * \param _fMin Minimum Z of the viewport.
		 * \param _fMax Maximum Z of the viewport.
		 * \param _ui32Target Index of the viewport to set.
		 */
		static LSE_INLINE void LSE_CALL				SetViewport( LSREAL _fX, LSREAL _fY,
			LSREAL _fWidth, LSREAL _fHeight,
			LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target = 0 );

		/**
		 * Sets the depth range (minimum and maximum Z) of the viewport.
		 *
		 * \param _fMin Minimum Z of the viewport.
		 * \param _fMax Maximum Z of the viewport.
		 * \param _ui32Target Index of the depth range to set.
		 */
		static LSE_INLINE void LSE_FCALL			SetDepthRange( LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target = 0 );

		/**
		 * Sets the total number of viewports.
		 *
		 * \param _ui32Total The total number of viewports rectangles set.
		 */
		static LSE_INLINE void LSE_FCALL			SetTotalViewports( uint32_t _ui32Total );


#include "../Gfx/LSGCreateStatesDecl.h"

		/**
		 * Gets the total number of vertex buffers set for read and write.
		 *
		 * \return Returns a reference to the currently set number of vertex buffers.
		 */
		static LSE_INLINE uint32_t & LSE_CALL		TotalActiveVertexBuffers();

		/**
		 * Gets the array of current vertex buffers for read and write.
		 *
		 * \return Returns the array of current vertex buffers for read and write.
		 */
		static LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL
													VertexBuffers();

#include "../Gfx/LSGDescFuncDecl.h"

	protected :
		// == Types.
		/** Render states specific to Vulkan. */
		typedef struct LSG_VULKAN_RENDER_STATE : public LSG_RENDER_STATE {
			/** The back-buffer width. */
			uint32_t								ui32BackBufferWidth;

			/** The back-buffer height. */
			uint32_t								ui32BackBufferHeight;

			/** The swap-buffer counter. */
			uint32_t								ui32CurBuffer;
		} * LPLSG_VULKAN_RENDER_STATE, * const LPCLSG_VULKAN_RENDER_STATE;

		/** Swap-chain image and view. */
		typedef struct LSG_VULKAN_SWAP_IMAGE {
			/** The actual image object. */
			VkImage									iImage;

			/** View to the image. */
			VkImageView								ivView;

			LSG_VULKAN_SWAP_IMAGE() :
				iImage( VK_NULL_HANDLE ),
				ivView( VK_NULL_HANDLE ) {
			}
		} * LPLSG_VULKAN_SWAP_IMAGE, * const LPCLSG_VULKAN_SWAP_IMAGE;

		/** Depth buffer image, memory, and view. */
		typedef struct LSG_VULKAN_DEPTH_BUFFER {
			/** The actual image object. */
			VkImage									iImage;

			/** View to the image. */
			VkImageView								ivView;

			/** Memory. */
			VkDeviceMemory							dmMemory;

			LSG_VULKAN_DEPTH_BUFFER() :
				iImage( VK_NULL_HANDLE ),
				ivView( VK_NULL_HANDLE ),
				dmMemory( VK_NULL_HANDLE ) {
			}
		} * LPLSG_VULKAN_DEPTH_BUFFER, * const LPCLSG_VULKAN_DEPTH_BUFFER;

#include "../Gfx/LSGStateRefDecl.h"

		// == Members.
		/** Vulkan instance. */
		static VkInstance							m_iInstance;

		/** Vulkan device. */
		static VkDevice								m_dDevice;

		/** Main surface. */
		static VkSurfaceKHR							m_sSurface;

		/** Swap chain. */
		static VkSwapchainKHR						m_sSwapChain;

		/** Swap-chain images. */
		static CVectorPoD<LSG_VULKAN_SWAP_IMAGE>	m_vSwapImages;

		/** Depth/stencil buffer. */
		static LSG_VULKAN_DEPTH_BUFFER				m_vdbDepthStencil;

		/** Command-buffer pool. */
		static VkCommandPool						m_cpCommandPool;

		/** Device information. */
		static VkPhysicalDevice						m_pdDevices[8];

		/** Number of devices. */
		static uint32_t								m_ui32NumDevices;

		/** Device properties. */
		static VkPhysicalDeviceProperties			m_pdpDeviceProperties[LSE_ELEMENTS( m_pdDevices )];

		/** Device memory properties (only the actual device being used. */
		static VkPhysicalDeviceMemoryProperties		m_pdmpMemProperties;

		/** Which device will we be using? */
		static uint32_t								m_ui32DeviceIndex;

		/** Family queue. */
		static uint32_t								m_ui32QueueFamilyIndex;

		/** Queue. */
		static VkQueue								m_qQueue;

		/** Main command buffer. */
		static CVulkanCommandBuffer					m_vcbCmdBuffer;

		/** Default sampler state. */
		static LSG_SAMPLER_STATE *					m_pssDefaultSamplerState;

		/** Default rasterizer state. */
		static LSG_RASTERIZER_STATE *				m_prsDefaultRasterizerState;

		/** Default blend state. */
		static LSG_BLEND_STATE *					m_pbsDefaultBlendState;

		/** Default depth/stencil state. */
		static LSG_DEPTH_STENCIL_STATE *			m_pdssDefaultDepthStencilState;

		/** The last render state. */
		static LSG_VULKAN_RENDER_STATE				m_rsLastRenderState;

		/** The current render state. */
		static LSG_VULKAN_RENDER_STATE				m_rsCurRenderState;

		/** Critical section for memory allocation. */
		static CCriticalSection						m_csAllocateMemoryCrit;

		/** Total allocated memory. */
		static uint64_t								m_ui64AllocatedMemory;

		/** Map of VkDeviceMemory objects to their sizes. */
		static CLinearMap<VkDeviceMemory, uint64_t>	m_lmDeviceMemorySizes;

		/** The shader manager. */
		static CShaderManager *						m_psmShaderMan;

#include "../Gfx/LSGCreateStatesMembersDecl.h"


		// == Functions.
		/**
		 * Release all resources and reset our internal state.
		 */
		static void LSE_CALL						ResetInternal();

		/**
		 * Gets the best physical device index.
		 *
		 * \param _sSurface The presentable surface.
		 * \param _ui32QueueIndex The returned index of the queue to use.
		 * \return Returns the best physical device index.
		 */
		static uint32_t LSE_CALL					GetBestGpuAndQueue( VkSurfaceKHR _sSurface, uint32_t &_ui32QueueIndex );

		/**
		 * Allocates an array of device queues.  Must be freed with LSEDELETE [].
		 *
		 * \param _pdDevice The device.
		 * \param _ui32Total Holds the number of queues to which the return value points.
		 * \return Returns an array of VkQueueFamilyProperties structures which must be freed with LSEDELETE [] when no longer in use.
		 */
		static VkQueueFamilyProperties * LSE_CALL	AllocateQueues( VkPhysicalDevice _pdDevice, uint32_t &_ui32Total );

		/**
		 * Gets the format of the back surface.
		 *
		 * \param _pdDevice The device.
		 * \param _sSurface The window/back surface.
		 * \return Finds a valid format for the back buffer.
		 */
		static VkFormat LSE_CALL					GetBackSurfaceFormat( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface );

		/**
		 * Gets the present mode.
		 *
		 * \param _pdDevice The device.
		 * \param _sSurface The window/back surface.
		 * \return Returns the present mode.
		 */
		static VkPresentModeKHR LSE_CALL			GetPresentMode( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface );

		/**
		 * Creates a command pool.
		 *
		 * \return Returns a new command pool.
		 */
		static LSE_INLINE LSBOOL LSE_CALL			CreateCommandPool( VkCommandPool &_cpPool );

		/**
		 * Creates the main depth buffer.  Command buffer must be open.
		 *
		 * \param _ui32Width Width of the texture to create.
		 * \param _ui32Height Height of the texture to create.
		 * \param _fFormat Format of the texture to create.
		 * \return Returns true if the creation of the depth buffer succeeded.
		 */
		static LSBOOL LSE_CALL						CreateDepthBuffer( uint32_t _ui32Width, uint32_t _ui32Height, VkFormat _fFormat );

		/**
		 * Destroys the system depth buffer.
		 */
		static void LSE_CALL						DestroyDepthBuffer();

		/**
		 * Tests the given depth format as being supported or not.
		 *
		 * \param _fFormat Format to test.
		 * \return Returns true or false to indicate the support of the given format.
		 */
		static LSE_INLINE LSBOOL LSE_CALL			TestDepthFormat( VkFormat _fFormat );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Gets the Vulkan device.
	 *
	 * \return Returns the Vulkan device.
	 */
	LSE_INLINE VkDevice LSE_CALL CVulkan::GetVulkanDevice() {
		return m_dDevice;
	}

	/**
	 * Gets the physical Vulkan device.
	 *
	 * \return Returns the physical Vulkan device.
	 */
	LSE_INLINE VkPhysicalDevice LSE_CALL CVulkan::GetVulkanPhysicalDevice() {
		return m_pdDevices[m_ui32DeviceIndex];
	}

	/**
	 * Gets the command pool.
	 *
	 * \return Returns the Vulkan command pool.
	 */
	LSE_INLINE VkCommandPool LSE_CALL CVulkan::GetVulkanCommandPool() {
		return m_cpCommandPool;
	}

	/**
	 * Gets the Vulkan queue.
	 *
	 * \return Returns the Vulkan queue.
	 */
	LSE_INLINE VkQueue LSE_CALL CVulkan::GetVulkanQueue() {
		return m_qQueue;
	}

	/**
	 * Gets the main command buffer.
	 *
	 * \return Returns the main command buffer.
	 */
	LSE_INLINE CVulkanCommandBuffer & LSE_CALL CVulkan::GetVulkanCommandBuffer() {
		return m_vcbCmdBuffer;
	}

	/**
	 * Allocates memory.  Always use this instead of ::vkAllocateMemory().  Internally, this keeps track of all memory allocated to give statistical metrics.
	 *
	 * \param _pmaiAllocateInfo A pointer to a structure of type VkMemoryAllocateInfo, which contains the parameters of the allocation. A successful returned allocation must use the requested parameters--no substitution is permitted by the implementation.
	 * \param _pacAllocator Controls host memory allocation.
	 * \param _dmMemory A reference to a VkDeviceMemory structure in which information about the allocated memory is returned.
	 * \return Returns a Vulkan error code indicating success or failure.
	 */
	LSE_INLINE VkResult LSE_CALL CVulkan::AllocateMemory( const VkMemoryAllocateInfo * _pmaiAllocateInfo,
		const VkAllocationCallbacks * _pacAllocator,
		VkDeviceMemory &_dmMemory ) {
		CCriticalSection::CLocker lLock( m_csAllocateMemoryCrit );
		VkResult rRes = ::vkAllocateMemory( GetVulkanDevice(), _pmaiAllocateInfo, _pacAllocator, &_dmMemory );
		if ( rRes == VK_SUCCESS ) {
			if ( m_lmDeviceMemorySizes.Insert( _dmMemory, _pmaiAllocateInfo->allocationSize ) ) {
				m_ui64AllocatedMemory += _pmaiAllocateInfo->allocationSize;
			}
		}
		return rRes;
	}

	/**
	 * Frees memory created by AllocateMemory().  Never call vkFreeMemory() directly, as this is used to keep track of total allocated memory.
	 *
	 * \param _dmMemory The VkDeviceMemory object to be freed.
	 * \param _pacAllocator Controls host memory allocation.
	 */
	LSE_INLINE void LSE_CALL CVulkan::FreeMemory( VkDeviceMemory &_dmMemory,
		const VkAllocationCallbacks * _pacAllocator ) {
		if ( _dmMemory != VK_NULL_HANDLE ) {
			CCriticalSection::CLocker lLock( m_csAllocateMemoryCrit );
			::vkFreeMemory( GetVulkanDevice(), _dmMemory, _pacAllocator );
			uint32_t ui32Index;
			if ( m_lmDeviceMemorySizes.ItemExists( _dmMemory, ui32Index ) ) {
				m_ui64AllocatedMemory -= m_lmDeviceMemorySizes.GetByIndex( ui32Index );
				m_lmDeviceMemorySizes.RemoveByIndexNoDealloc( ui32Index );
				if ( !m_lmDeviceMemorySizes.Length() ) {
					m_lmDeviceMemorySizes.Reset();
				}
			}
			_dmMemory = VK_NULL_HANDLE;
		}
	}

	/**
	 * Gets the total amount of allocated graphics memory.
	 *
	 * \return Returns the total amount of allocated graphics memory.
	 */
	LSE_INLINE uint64_t LSE_CALL CVulkan::TotalAllocatedMemory() {
		return m_ui64AllocatedMemory;
	}

	/**
	* Sets default states for between-object rendering.  Does not change viewports, render targets, shader
	*	resources, etc.
	*/
	LSE_INLINE void LSE_FCALL CVulkan::SetDefaultObjectRenderStates() {
		m_rsCurRenderState.prsRasterState = m_prsDefaultRasterizerState;
		m_rsCurRenderState.pbsBlendState = m_pbsDefaultBlendState;
		m_rsCurRenderState.pdssDepthStencilState = m_pdssDefaultDepthStencilState;
		m_rsCurRenderState.ui32StencilRef = 0;
		m_rsCurRenderState.fBlendFactors[0] = m_rsCurRenderState.fBlendFactors[1] =
			m_rsCurRenderState.fBlendFactors[2] = m_rsCurRenderState.fBlendFactors[3] = LSGREAL( 1.0 );
		m_rsCurRenderState.ui32SampleMask = 0xFFFFFFFFUL;
	}

	/**
	* Sets all default render states.
	*/
	LSE_INLINE void LSE_FCALL CVulkan::SetAllDefaultRenderStates() {
		// General states.
		SetDefaultObjectRenderStates();

		// Clear values.
		m_rsCurRenderState.fClearColor[0] = m_rsCurRenderState.fClearColor[1] =
			m_rsCurRenderState.fClearColor[2] = m_rsCurRenderState.fClearColor[3] = LSGREAL( 1.0 );
		m_rsCurRenderState.fClearDepth = LSGREAL( 1.0 );
		m_rsCurRenderState.ui8ClearStencil = 0;

		// Viewports and scissors.
		m_rsCurRenderState.ui8TotalViewportsAndScissors = 1;
		for ( uint32_t I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
			m_rsCurRenderState.vViewports[I].x = 0;
			m_rsCurRenderState.vViewports[I].y = 0;

			m_rsCurRenderState.vViewports[I].width = static_cast<float>(m_rsCurRenderState.ui32BackBufferWidth);
			m_rsCurRenderState.vViewports[I].height = static_cast<float>(m_rsCurRenderState.ui32BackBufferHeight);

			m_rsCurRenderState.vViewports[I].minDepth = static_cast<float>(LSG_MIN_DEPTH);
			m_rsCurRenderState.vViewports[I].maxDepth = static_cast<float>(LSG_MAX_DEPTH);


			m_rsCurRenderState.rScissors[I].left = 0;
			m_rsCurRenderState.rScissors[I].top = 0;

			m_rsCurRenderState.rScissors[I].right = m_rsCurRenderState.ui32BackBufferWidth;
			m_rsCurRenderState.rScissors[I].bottom = m_rsCurRenderState.ui32BackBufferHeight;
		}


		// Render targets.
		m_rsCurRenderState.ui8TotalRenderTargets = 1;
		for ( uint32_t I = 1; I < LSG_MAX_RENDER_TARGET_COUNT; ++I ) {
			m_rsCurRenderState.prtRenderTargets[I] = NULL;
		}
		/*m_rsCurRenderState.prtRenderTargets[0] = GetBackBufferSurface();
		m_rsCurRenderState.pdsDepthStencil = GetDepthStencilSurface();*/

		// Shader resources.
		for ( uint32_t I = LSG_MAX_TEXTURE_UNITS; I--; ) {
			m_rsCurRenderState.psrShaderResources[I] = NULL;
		}
		for ( uint32_t I = LSG_MAX_SHADER_SAMPLERS; I--; ) {
			m_rsCurRenderState.pssSamplers[I] = m_pssDefaultSamplerState;
		}

		// Vertex buffers.
		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			VertexBuffers()[I].pvbVertBuffer = NULL;
			VertexBuffers()[I].ui32Offset = 0;
		}
		m_rsCurRenderState.pilInputLayout = NULL;
		TotalActiveVertexBuffers() = 0;
	}

	/**
	 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
	 *
	 * \return Returns true if the rendering device is ready.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CVulkan::BeginRender() {
		return true;
	}

	/**
	 * End rendering.  Must be called once per frame only, after all rendering is done.
	 */
	LSE_INLINE void LSE_FCALL CVulkan::EndRender() {
	}

	/**
	 * Present the backbuffer to the screen.
	 */
	LSE_INLINE void LSE_FCALL CVulkan::Present() {
	}

	/**
	* Creates a perspective projection matrix based on a field of view.
	*
	* \param _mIn The output matrix.
	* \param _fFov Field of view in the y direction, in radians.
	* \param _fAspect Aspect ratio, defined as view space width divided by height.
	* \param _fNear Z-value of the near view-plane.
	* \param _fFar Z-value of the far view-plane.
	* \return Returns the input matrix so that this function can be used as a parameter to another function.
	*/
	LSE_INLINE CMatrix4x4 & LSE_CALL CVulkan::PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _fFov, LSREAL _fAspect, LSREAL _fNear, LSREAL _fFar ) {
		// Zero out the ones that need to be zero.
		_mIn._12 = _mIn._13 = _mIn._14 = LSM_ZERO;
		_mIn._21 = _mIn._23 = _mIn._24 = LSM_ZERO;
		_mIn._31 = _mIn._32 = LSM_ZERO;
		_mIn._41 = _mIn._42 = _mIn._44 = LSM_ZERO;

		LSREAL fInvDist = LSM_ONE / (_fNear - _fFar);

		// Set the remaining ones.
		_mIn._22 = LSM_ONE / CMathLib::Tan( LSM_HALF * _fFov );
		_mIn._11 = _mIn._22 / _fAspect;

		_mIn._33 = _fFar * fInvDist;
		_mIn._34 = -LSM_ONE;

		_mIn._43 = (_fFar * _fNear) * fInvDist;

		// Also see https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
		// Consider copying the OpenGL code and applying this suggested transform.
		return _mIn;
	}

	/**
	 * Creates an orthogonal projection matrix.
	 *
	 * \param _mIn The output matrix.
	 * \param _fLeft Minimum x-value of the view volume.
	 * \param _fRight Maximum x-value of the view volume.
	 * \param _fBottom Minimum y-value of the view volume.
	 * \param _fTop Maximum y-value of the view volume.
	 * \param _fNear Minimum z-value of the view volume.
	 * \param _fFar Maximum z-value of the view volume.
	 * \return Returns the input matrix so that this function can be used as a parameter to another function.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CVulkan::OrthoOffCenter( CMatrix4x4 &_mIn,
		LSREAL _fLeft, LSREAL _fRight,
		LSREAL _fBottom, LSREAL _fTop,
		LSREAL _fNear, LSREAL _fFar ) {
		// Left-handed.
		// Zero out the ones that need to be zero.
		_mIn._12 = _mIn._13 = _mIn._14 = LSM_ZERO;
		_mIn._21 = _mIn._23 = _mIn._24 = LSM_ZERO;
		_mIn._31 = _mIn._32 = _mIn._34 = LSM_ZERO;

		register LSREAL fWidth = _fRight - _fLeft;
		register LSREAL fHeight = _fTop - _fBottom;
		register LSREAL fDepth = _fFar - _fNear;

		_mIn._11 = LSM_TWO / fWidth;
		_mIn._22 = LSM_TWO / fHeight;
		_mIn._33 = LSM_ONE / fDepth;

		_mIn._41 = (_fRight + _fLeft) / -fWidth;
		_mIn._42 = (_fTop + _fBottom) / -fHeight;
		_mIn._43 = _fNear * -_mIn._33;
		_mIn._44 = LSM_ONE;

		return _mIn;
	}

	/**
	* Extracts the frustum planes from the given projection matrix.
	*
	* \param _mMatrix Matrix from which to extract the frustum planes.
	* \param _fFrustum The frustum to which the planes are extracted.
	* \param _bNormalize If true, the planes are normalized.
	* \return Returns the reference to _fFrustum.
	*/
	LSE_INLINE CFrustum & LSE_CALL CVulkan::ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize ) {
		// Left clipping plane.
		_fFrustum.m_pLeftPlane.n = _mMatrix.GetRow( 3 ) + _mMatrix.GetRow( 0 );
		_fFrustum.m_pLeftPlane.dist = -(_mMatrix._44 + _mMatrix._41);

		// Right clipping plane.
		_fFrustum.m_pRightPlane.n = _mMatrix.GetRow( 3 ) - _mMatrix.GetRow( 0 );
		_fFrustum.m_pRightPlane.dist = -(_mMatrix._44 - _mMatrix._41);

		// Bottom clipping plane.
		_fFrustum.m_pBottomPlane.n = _mMatrix.GetRow( 3 ) + _mMatrix.GetRow( 1 );
		_fFrustum.m_pBottomPlane.dist = -(_mMatrix._44 + _mMatrix._42);

		// Top clipping plane.
		_fFrustum.m_pTopPlane.n = _mMatrix.GetRow( 3 ) - _mMatrix.GetRow( 1 );
		_fFrustum.m_pTopPlane.dist = -(_mMatrix._44 - _mMatrix._42);

		// Near clipping plane.
		_fFrustum.m_pNearPlane.n = _mMatrix.GetRow( 2 );
		_fFrustum.m_pNearPlane.dist = -_mMatrix._43;

		// Far clipping plane.
		_fFrustum.m_pFarPlane.n = _mMatrix.GetRow( 3 ) - _mMatrix.GetRow( 2 );
		_fFrustum.m_pFarPlane.dist = -(_mMatrix._44 - _mMatrix._43);

		// It is not always nessesary to normalize
		//	the planes of the frustum.  Non-normalized
		//	planes can still be used for basic
		//	intersection tests.
		if ( _bNormalize ) {
			_fFrustum.m_pLeftPlane.Normalize();
			_fFrustum.m_pRightPlane.Normalize();
			_fFrustum.m_pBottomPlane.Normalize();
			_fFrustum.m_pTopPlane.Normalize();
			_fFrustum.m_pNearPlane.Normalize();
			_fFrustum.m_pFarPlane.Normalize();
		}
		return _fFrustum;
	}

	/**
	* Clear all active buffers.
	*/
	LSE_INLINE void LSE_FCALL CVulkan::Clear() {
		Clear( LSG_B_ALL );
	}

	/**
	 * Sets the clear color.
	 *
	 * \param _fR Red value.
	 * \param _fG Green value.
	 * \param _fB Blue value.
	 * \param _fA Alpha value.
	 */
	LSE_INLINE void LSE_CALL CVulkan::SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA ) {
		m_rsCurRenderState.fClearColor[0] = static_cast<LSGREAL>(_fR);
		m_rsCurRenderState.fClearColor[1] = static_cast<LSGREAL>(_fG);
		m_rsCurRenderState.fClearColor[2] = static_cast<LSGREAL>(_fB);
		m_rsCurRenderState.fClearColor[3] = static_cast<LSGREAL>(_fA);
	}

	/**
	 * Sets the depth clear value.
	 *
	 * \param _fValue The new depth clear value.
	 */
	LSE_INLINE void LSE_FCALL CVulkan::SetClearDepth( LSREAL _fValue ) {
		m_rsCurRenderState.fClearDepth = static_cast<LSGREAL>(_fValue);
	}

	/**
	 * Sets the stencil clear value.
	 *
	 * \param _ui8Val The new stencil clear value.
	 */
	LSE_INLINE void LSE_FCALL CVulkan::SetClearStencil( uint8_t _ui8Val ) {
		m_rsCurRenderState.ui8ClearStencil = _ui8Val;
	}

	/**
	 * Sets the viewport dimensions.
	 *
	 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fWidth Width of the viewport.
	 * \param _fHeight Height of the viewport.
	 * \param _ui32Target Index of the viewport to set.
	 */
	LSE_INLINE void LSE_CALL CVulkan::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<FLOAT>(_fX) >= static_cast<FLOAT>(LSG_VIEWPORT_MIN) && static_cast<FLOAT>(_fX) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );
		assert( static_cast<FLOAT>(_fY) >= static_cast<FLOAT>(LSG_VIEWPORT_MIN) && static_cast<FLOAT>(_fY) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );

		assert( static_cast<FLOAT>(_fWidth) >= 0.0f && static_cast<FLOAT>(_fX + _fWidth) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );
		assert( static_cast<FLOAT>(_fHeight) >= 0.0f && static_cast<FLOAT>(_fY + _fHeight) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );

		m_rsCurRenderState.vViewports[_ui32Target].x = static_cast<FLOAT>(_fX);
		m_rsCurRenderState.vViewports[_ui32Target].y = static_cast<FLOAT>(_fY);
		m_rsCurRenderState.vViewports[_ui32Target].width = static_cast<FLOAT>(_fWidth);
		m_rsCurRenderState.vViewports[_ui32Target].height = static_cast<FLOAT>(_fHeight);
	}

	/**
	 * Sets the viewport dimensions.
	 *
	 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fWidth Width of the viewport.
	 * \param _fHeight Height of the viewport.
	 * \param _fMin Minimum Z of the viewport.
	 * \param _fMax Maximum Z of the viewport.
	 * \param _ui32Target Index of the viewport to set.
	 */
	LSE_INLINE void LSE_CALL CVulkan::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight,
		LSREAL _fMin, LSREAL _fMax,
		uint32_t _ui32Target ) {
		SetViewport( _fX, _fY, _fWidth, _fHeight, _ui32Target );
		SetDepthRange( _fMin, _fMax, _ui32Target );
	}

	/**
	 * Sets the depth range (minimum and maximum Z) of the viewport.
	 *
	 * \param _fMin Minimum Z of the viewport.
	 * \param _fMax Maximum Z of the viewport.
	 * \param _ui32Target Index of the depth range to set.
	 */
	LSE_INLINE void LSE_FCALL CVulkan::SetDepthRange( LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<FLOAT>(_fMin) >= static_cast<FLOAT>(LSG_MIN_DEPTH) && static_cast<FLOAT>(_fMin) <= static_cast<FLOAT>(LSG_MAX_DEPTH) );
		assert( static_cast<FLOAT>(_fMax) >= static_cast<FLOAT>(LSG_MIN_DEPTH) && static_cast<FLOAT>(_fMax) <= static_cast<FLOAT>(LSG_MAX_DEPTH) );

		m_rsCurRenderState.vViewports[_ui32Target].minDepth = static_cast<float>(_fMin);
		m_rsCurRenderState.vViewports[_ui32Target].maxDepth = static_cast<float>(_fMax);
	}

	/**
	 * Sets the total number of viewports.
	 *
	 * \param _ui32Total The total number of viewports rectangles set.
	 */
	LSE_INLINE void LSE_FCALL CVulkan::SetTotalViewports( uint32_t _ui32Total ) {
		assert( _ui32Total < LSG_MAX_VIEWPORTS_SCISSORS );

		m_rsCurRenderState.ui8TotalViewportsAndScissors = static_cast<uint8_t>(_ui32Total);
	}

	/**
	* Gets the total number of vertex buffers set for read and write.
	*
	* \return Returns a reference to the currently set number of vertex buffers.
	*/
	LSE_INLINE uint32_t & LSE_CALL CVulkan::TotalActiveVertexBuffers() {
		return m_rsCurRenderState.ui32TotalVertexBuffers;
	}

	/**
	* Gets the array of current vertex buffers for read and write.
	*
	* \return Returns the array of current vertex buffers for read and write.
	*/
	LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL CVulkan::VertexBuffers() {
		return m_rsCurRenderState.pvbVertexBuffers;
	}

	/**
	 * Creates a command pool.
	 *
	 * \param _cpPool On success, this holds the return value.
	 * \return Returns a new command pool.
	 */
	LSE_INLINE LSBOOL LSE_CALL CVulkan::CreateCommandPool( VkCommandPool &_cpPool ) {
		VkCommandPoolCreateInfo cpciInfo = {};
		cpciInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cpciInfo.pNext = NULL;
		cpciInfo.queueFamilyIndex = m_ui32QueueFamilyIndex;
		cpciInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkCreateCommandPool( GetVulkanDevice(), &cpciInfo, NULL, &_cpPool ) ) ) {
			return false;
		}
		return true;
	}

	/**
	 * Tests the given depth format as being supported or not.
	 *
	 * \param _fFormat Format to test.
	 * \return Returns true or false to indicate the support of the given format.
	 */
	LSE_INLINE LSBOOL LSE_CALL CVulkan::TestDepthFormat( VkFormat _fFormat ) {
		VkFormatProperties fpProps;
		::vkGetPhysicalDeviceFormatProperties( GetVulkanPhysicalDevice(), _fFormat, &fpProps );
		return (fpProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ||
			(fpProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN

#endif	// __LSG_VULKAN_H__

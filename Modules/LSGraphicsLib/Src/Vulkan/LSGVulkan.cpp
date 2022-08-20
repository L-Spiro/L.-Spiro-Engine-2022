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

#include "LSGVulkan.h"

#ifdef LSG_VKN

#include "../Framebuffer/LSGFramebuffer.h"
#include "../VulkanShaders/LSGVulkanCompiledShaderManager.h"
#include "../VulkanShaders/LSGVulkanShaderManager.h"

namespace lsg {

	// == Various constructors.
	CVulkan::CImage::CImage() :
		m_iImage( VK_NULL_HANDLE ),
		m_pacAllocator( NULL ) {
	}
	CVulkan::CImage::~CImage() {
		DestroyImage( m_pacAllocator );
	}

	// == Functions.
	/**
	 * Creates the image object.
	 *
	 * \param _piciCreateInfo A pointer to an instance of the VkImageCreateInfo structure containing parameters to be used to create the image.
	 * \param _pacAllocator Controls host memory allocation.
	 * \return Returns true if the VkImage object was created.
	 */
	LSBOOL LSE_CALL CVulkan::CImage::CreateImage( const VkImageCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator ) {
		DestroyImage( m_pacAllocator );
		if ( ::vkCreateImage( CVulkan::GetVulkanDevice(), _piciCreateInfo, _pacAllocator, &m_iImage ) == VK_SUCCESS ) {
			m_pacAllocator = _pacAllocator;
			return true;
		}
		return false;
	}

	/**
	 * Destroys the image object.
	 *
	 * \param _pacAllocator Controls host memory allocation.
	 */
	void LSE_CALL CVulkan::CImage::DestroyImage( const VkAllocationCallbacks * _pacAllocator ) {
		if ( m_iImage != VK_NULL_HANDLE ) {
			::vkDestroyImage( CVulkan::GetVulkanDevice(), m_iImage, _pacAllocator );
			m_iImage = VK_NULL_HANDLE;
		}
		m_pacAllocator = NULL;
	}

	// == Various constructors.
	CVulkan::CDeviceMemory::CDeviceMemory() :
		m_dmDeviceMemory( VK_NULL_HANDLE ),
		m_pacAllocator( NULL ) {
	}
	CVulkan::CDeviceMemory::~CDeviceMemory() {
		FreeMemory( m_pacAllocator );
	}

	// == Functions.
	/**
	 * Creates the image object.
	 *
	 * \param _pmaiAllocateInfo A pointer to a structure of type VkMemoryAllocateInfo, which contains parameters of the allocation. A successful returned allocation must use the requested parameters--no substitution is permitted by the implementation.
	 * \param _pacAllocator Controls host memory allocation.
	 * \return Returns true if the VkDeviceMemory object was created.
	 */
	LSBOOL LSE_CALL CVulkan::CDeviceMemory::AllocateMemory( const VkMemoryAllocateInfo * _pmaiAllocateInfo, const VkAllocationCallbacks * _pacAllocator ) {
		FreeMemory( m_pacAllocator );
		if ( CVulkan::AllocateMemory( _pmaiAllocateInfo, _pacAllocator, m_dmDeviceMemory ) == VK_SUCCESS ) {
			m_pacAllocator = _pacAllocator;
			return true;
		}
		return false;
	}

	/**
	 * Destroys the image object.
	 *
	 * \param _pacAllocator Controls host memory allocation.
	 */
	void LSE_CALL CVulkan::CDeviceMemory::FreeMemory( const VkAllocationCallbacks * _pacAllocator ) {
		if ( m_dmDeviceMemory != VK_NULL_HANDLE ) {
			CVulkan::FreeMemory( m_dmDeviceMemory, _pacAllocator );
		}
		m_pacAllocator = NULL;
	}

	// == Various constructors.
	CVulkan::CBuffer::CBuffer() :
		m_bBuffer( VK_NULL_HANDLE ),
		m_pacAllocator( NULL ) {
	}
	CVulkan::CBuffer::~CBuffer() {
		DestroyBuffer( m_pacAllocator );
	}

	// == Functions.
	/**
	 * Creates the image object.
	 *
	 * \param _pbciCreateInfo A pointer to an instance of the VkBufferCreateInfo structure containing parameters affecting creation of the buffer.
	 * \param _pacAllocator Controls host memory allocation.
	 * \return Returns true if the VkDeviceMemory object was created.
	 */
	LSBOOL LSE_CALL CVulkan::CBuffer::CreateBuffer( const VkBufferCreateInfo * _pbciCreateInfo, const VkAllocationCallbacks * _pacAllocator ) {
		DestroyBuffer( m_pacAllocator );
		if ( ::vkCreateBuffer( CVulkan::GetVulkanDevice(), _pbciCreateInfo, _pacAllocator, &m_bBuffer ) == VK_SUCCESS ) {
			m_pacAllocator = _pacAllocator;
			return true;
		}
		return false;
	}

	/**
	 * Destroys the buffer object.
	 *
	 * \param _pacAllocator Controls host memory allocation.
	 */
	void LSE_CALL CVulkan::CBuffer::DestroyBuffer( const VkAllocationCallbacks * _pacAllocator ) {
		if ( m_bBuffer != VK_NULL_HANDLE ) {
			::vkDestroyBuffer( CVulkan::GetVulkanDevice(), m_bBuffer, _pacAllocator );
			m_bBuffer = VK_NULL_HANDLE;
		}
		m_pacAllocator = NULL;
	}

	// == Members.
	/** Vulkan instance. */
	VkInstance CVulkan::m_iInstance = VK_NULL_HANDLE;

	/** Vulkan device. */
	VkDevice CVulkan::m_dDevice = VK_NULL_HANDLE;

	/** Main surface. */
	VkSurfaceKHR CVulkan::m_sSurface = VK_NULL_HANDLE;

	/** Swap chain. */
	VkSwapchainKHR CVulkan::m_sSwapChain = VK_NULL_HANDLE;

	/** Swap-chain images. */
	CVectorPoD<CVulkan::LSG_VULKAN_SWAP_IMAGE> CVulkan::m_vSwapImages;

	/** Depth/stencil buffer. */
	CVulkan::LSG_VULKAN_DEPTH_BUFFER CVulkan::m_vdbDepthStencil;

	/** Command-buffer pool. */
	VkCommandPool CVulkan::m_cpCommandPool = VK_NULL_HANDLE;

	/** Device information. */
	VkPhysicalDevice CVulkan::m_pdDevices[8] = { VK_NULL_HANDLE };

	/** Number of devices. */
	uint32_t CVulkan::m_ui32NumDevices = 0;

	/** Device properties. */
	VkPhysicalDeviceProperties CVulkan::m_pdpDeviceProperties[LSE_ELEMENTS( m_pdDevices )] = {};
	
	/** Device memory properties (only the actual device being used. */
	VkPhysicalDeviceMemoryProperties CVulkan::m_pdmpMemProperties = {};

	/** Which device will we be using? */
	uint32_t CVulkan::m_ui32DeviceIndex = 0;

	/** Family queue. */
	uint32_t CVulkan::m_ui32QueueFamilyIndex = 0;

	/** Queue. */
	VkQueue CVulkan::m_qQueue = VK_NULL_HANDLE;

	/** Main command buffer. */
	CVulkanCommandBuffer CVulkan::m_vcbCmdBuffer;

	/** Default sampler state. */
	LSG_SAMPLER_STATE * CVulkan::m_pssDefaultSamplerState = NULL;

	/** Default rasterizer state. */
	LSG_RASTERIZER_STATE * CVulkan::m_prsDefaultRasterizerState = NULL;

	/** Default blend state. */
	LSG_BLEND_STATE * CVulkan::m_pbsDefaultBlendState = NULL;

	/** Default depth/stencil state. */
	LSG_DEPTH_STENCIL_STATE * CVulkan::m_pdssDefaultDepthStencilState = NULL;

	/** The last render state. */
	CVulkan::LSG_VULKAN_RENDER_STATE CVulkan::m_rsLastRenderState;

	/** The current render state. */
	CVulkan::LSG_VULKAN_RENDER_STATE CVulkan::m_rsCurRenderState;

	/** Critical section for memory allocation. */
	CCriticalSection CVulkan::m_csAllocateMemoryCrit;

	/** Total allocated memory. */
	uint64_t CVulkan::m_ui64AllocatedMemory = 0ULL;

	/** Map of VkDeviceMemory objects to their sizes. */
	CLinearMap<VkDeviceMemory, uint64_t> CVulkan::m_lmDeviceMemorySizes;

	/** The shader manager. */
	CShaderManager * CVulkan::m_psmShaderMan = NULL;

#define LSG_CREATESTATESDEF_CLASS CVulkan
#include "../Gfx/LSGCreateStatesMembersDef.h"

	// == Functions.
	/**
	 * Initialize Vulkan.
	 *
	 * \param _lpdiInit Data required to initialize the device.
	 * \return Returns false if there are any problem during initialization.
	 */
	LSBOOL LSE_CALL CVulkan::InitVulkan( LSG_VULKAN_INIT * _pvInit ) {
		ResetInternal();

		// Create the instance.
		VkApplicationInfo aiAppInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,			// sType
			NULL,										// pNext
			_pvInit->pcWindowName,						// pApplicationName
			_pvInit->ui32AppVersion,					// applicationVersion
			"L. Spiro Engine",							// pEngineName
			1,											// engineVersion
			0											// apiVersion
		};

		const char * pcExt[] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef LSE_WINDOWS
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
			VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif	// #ifdef LSE_WINDOWS
		};

		VkInstanceCreateInfo iciInstInfo = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,		// sType
			NULL,										// pNext
			0,											// flags
			&aiAppInfo,									// pApplicationInfo
			0,											// enabledLayerCount
			NULL,										// ppEnabledLayerNames
			LSE_ELEMENTS( pcExt ),						// enabledExtensionCount
			pcExt										// ppEnabledExtensionNames
		};

		if ( PrintVulkanError( "CVulkan::InitVulkan(): ", ::vkCreateInstance( &iciInstInfo, NULL, &m_iInstance ) ) ) {
			ResetInternal();
			return false;
		}

		// Create a surface.
#ifdef LSE_WINDOWS
		VkWin32SurfaceCreateInfoKHR viInitMe = {};
		viInitMe.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		viInitMe.pNext = NULL;
		viInitMe.hinstance = _pvInit->hInst;
		viInitMe.hwnd = _pvInit->hWnd;
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkCreateWin32SurfaceKHR( m_iInstance, &viInitMe, NULL, &m_sSurface ) ) ) {
			ResetInternal();
			return false;
		}
#else
		VkXcbSurfaceCreateInfoKHR xsciInitMe = {};
		xsciInitMe.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		xsciInitMe.pNext = NULL;
		xsciInitMe.connection = _pvInit->hInst;
		xsciInitMe.window = _pvInit->hWnd;
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkCreateXcbSurfaceKHR( m_iInstance, &xsciInitMe, NULL, &m_sSurface ) ) ) {
			ResetInternal();
			return false;
		}
#endif	// #ifdef LSE_WINDOWS


		// Enumerate devices.
		m_ui32NumDevices = LSE_ELEMENTS( m_pdDevices );
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ", ::vkEnumeratePhysicalDevices( m_iInstance,
			&m_ui32NumDevices,
			m_pdDevices ) ) ) {
			ResetInternal();
			return false;
		}
		assert( m_ui32NumDevices );
		if ( !m_ui32NumDevices ) {
			ResetInternal();
			return false;
		}

		for ( uint32_t I = m_ui32NumDevices; I--; ) {
			::vkGetPhysicalDeviceProperties( m_pdDevices[I], &m_pdpDeviceProperties[I] );
		}
		// Pick the best device.
		m_ui32DeviceIndex = GetBestGpuAndQueue( m_sSurface, m_ui32QueueFamilyIndex );
		::vkGetPhysicalDeviceMemoryProperties( GetVulkanPhysicalDevice(), &m_pdmpMemProperties );
			

		// Create a device.
		const float fQueuePriorities[] = {
			0.0f
		};
		VkDeviceQueueCreateInfo dqciQueueInfo = {
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,	// sType
			NULL,										// pNext
			0,											// flags
			m_ui32DeviceIndex,							// queueFamilyIndex
			LSE_ELEMENTS( fQueuePriorities ),			// queueCount
			fQueuePriorities,							// pQueuePriorities
		};
		VkDeviceCreateInfo dciDeviceInfo = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,		// sType
			NULL,										// pNext
			0,											// flags
			1,											// queueCreateInfoCount
			&dqciQueueInfo,								// pQueueCreateInfos
			0,											// enabledLayerCount
			NULL,										// ppEnabledLayerNames
			0,											// enabledExtensionCount
			NULL,										// ppEnabledExtensionNames
			NULL,										// pEnabledFeatures
		};

		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkCreateDevice( GetVulkanPhysicalDevice(), &dciDeviceInfo, NULL, &m_dDevice ) ) ) {
			ResetInternal();
			return false;
		}

		// Create the swap-chain.
		// What format should it be?
		VkFormat fSwapFormat = GetBackSurfaceFormat( GetVulkanPhysicalDevice(), m_sSurface );
		if ( fSwapFormat == VK_FORMAT_UNDEFINED ) { return false; }

		// Surface capabilities.
		VkSurfaceCapabilitiesKHR scSurfaceCapabilities;
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkGetPhysicalDeviceSurfaceCapabilitiesKHR( GetVulkanPhysicalDevice(), m_sSurface,
			&scSurfaceCapabilities ) ) ) {
			ResetInternal();
			return false;
		}
		// Use the capabilities to get the extents.
		VkExtent2D e2SwapExtent;
		// Width and height are either both -1, or both not -1.
		if ( scSurfaceCapabilities.currentExtent.width == uint32_t( -1 ) ) {
			// If the surface size is undefined, the size is set to the size of the window.
#ifdef LSE_WINDOWS
			RECT rClient;
			::GetClientRect( _pvInit->hWnd, &rClient );
			e2SwapExtent.width = rClient.right - rClient.left;
			e2SwapExtent.height = rClient.bottom - rClient.top;
#endif	// #ifdef LSE_WINDOWS
		} else {
			// If the surface size is defined, the swap-chain size must match.
			e2SwapExtent = scSurfaceCapabilities.currentExtent;
		}
		m_rsCurRenderState.ui32BackBufferWidth = e2SwapExtent.width;
		m_rsCurRenderState.ui32BackBufferHeight = e2SwapExtent.height;

		// Present mode.
		VkPresentModeKHR pmPresentMode = GetPresentMode( GetVulkanPhysicalDevice(), m_sSurface );

		// Number of back-buffer images.
		uint32_t ui32SwapImages = scSurfaceCapabilities.minImageCount + 1;
		if ( (scSurfaceCapabilities.maxImageCount > 0) &&
			(ui32SwapImages > scSurfaceCapabilities.maxImageCount) ) {
			ui32SwapImages = scSurfaceCapabilities.maxImageCount;
		}

		// Surface transform.
		VkSurfaceTransformFlagBitsKHR stfbTrans;
		if ( scSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
			stfbTrans = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else {
			stfbTrans = scSurfaceCapabilities.currentTransform;
		}

		VkSwapchainCreateInfoKHR sciSwapChain = {};
		sciSwapChain.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		sciSwapChain.pNext = NULL;
		sciSwapChain.surface = m_sSurface;
		sciSwapChain.minImageCount = ui32SwapImages;
		sciSwapChain.imageFormat = fSwapFormat;
		sciSwapChain.imageExtent.width = e2SwapExtent.width;
		sciSwapChain.imageExtent.height = e2SwapExtent.height;
		sciSwapChain.preTransform = stfbTrans;
		sciSwapChain.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		sciSwapChain.imageArrayLayers = 1;
		sciSwapChain.presentMode = pmPresentMode;
		sciSwapChain.oldSwapchain = NULL;
		sciSwapChain.clipped = true;
		sciSwapChain.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		sciSwapChain.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		sciSwapChain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		sciSwapChain.queueFamilyIndexCount = 0;
		sciSwapChain.pQueueFamilyIndices = NULL;
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkCreateSwapchainKHR( GetVulkanDevice(), &sciSwapChain, NULL, &m_sSwapChain ) ) ) {
			ResetInternal();
			return false;
		}

		// Count the number of images.
		uint32_t ui32SwapTotal = 0;
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkGetSwapchainImagesKHR( GetVulkanDevice(), m_sSwapChain, &ui32SwapTotal, NULL ) ) ) {
			ResetInternal();
			return false;
		}
		CVectorPoD<VkImage> vSwapImages;
		if ( !m_vSwapImages.Resize( ui32SwapTotal ) || !vSwapImages.Resize( ui32SwapTotal ) ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to allocate swap images.\r\n" );
			ResetInternal();
			return false;
		}
		if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
			::vkGetSwapchainImagesKHR( GetVulkanDevice(), m_sSwapChain, &ui32SwapTotal, &vSwapImages[0] ) ) ) {
			ResetInternal();
			return false;
		}

		// Create the command pool.
		if ( !CreateCommandPool( m_cpCommandPool ) ) {
			ResetInternal();
			return false;
		}

		// Get the queue.
		::vkGetDeviceQueue( GetVulkanDevice(), m_ui32QueueFamilyIndex, 0,
			&m_qQueue );

		if ( !m_vcbCmdBuffer.AllocateCommandBuffer() ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to allocate command buffer.\r\n" );
			ResetInternal();
			return false;
		}
		m_vcbCmdBuffer.BeginCommandBuffer();

		// Create the views to the swap images.
		for ( uint32_t I = 0; I < ui32SwapTotal; I++ ) {
			VkImageViewCreateInfo ivciInfo = {};
			ivciInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ivciInfo.pNext = NULL;
			ivciInfo.format = fSwapFormat;
			ivciInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			ivciInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			ivciInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			ivciInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			ivciInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ivciInfo.subresourceRange.baseMipLevel = 0;
			ivciInfo.subresourceRange.levelCount = 1;
			ivciInfo.subresourceRange.baseArrayLayer = 0;
			ivciInfo.subresourceRange.layerCount = 1;
			ivciInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivciInfo.flags = 0;

			m_vSwapImages[I].iImage = vSwapImages[I];

			m_vcbCmdBuffer.PipelineBarrierImage( m_vSwapImages[I].iImage,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );

			ivciInfo.image = m_vSwapImages[I].iImage;

			if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
				::vkCreateImageView( GetVulkanDevice(), &ivciInfo, NULL,
				&m_vSwapImages[I].ivView ) ) ) {
				ResetInternal();
				return false;
			}
		}
		vSwapImages.Reset();

		// Create the depth buffer.
		static const VkFormat fFormats[] = {
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D16_UNORM
		};
		VkFormat fDepthFormat = VK_FORMAT_UNDEFINED;
		for ( uint32_t I = 0; I < LSE_ELEMENTS( fFormats ); ++I ) {
			if ( TestDepthFormat( fFormats[I] ) ) {
				fDepthFormat = fFormats[I];
				break;
			}
		}
		if ( fDepthFormat == VK_FORMAT_UNDEFINED ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to find a suitable depth/stencil format.\r\n" );
			ResetInternal();
			return false;
		}
		if ( !CreateDepthBuffer( sciSwapChain.imageExtent.width, sciSwapChain.imageExtent.height, fDepthFormat ) ) {
			ResetInternal();
			return false;
		}

		m_vcbCmdBuffer.EndCommandBuffer();
		m_vcbCmdBuffer.QueueSubmit();


		// Create the default states.
		// Sampler.
		LSG_SAMPLER_DESC sdOurSamplerDesc;
		if ( FAILED( CreateSamplerState( DefaultSamplerDesc( sdOurSamplerDesc ), &m_pssDefaultSamplerState ) ) ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to create default sampler state.\r\n" );
			ResetInternal();
			return false;
		}

		// Rasterizer.
		LSG_RASTERIZER_DESC rdOurRasterizerDesc;
		if ( FAILED( CreateRasterizerState( DefaultRasterizerDesc( rdOurRasterizerDesc ), &m_prsDefaultRasterizerState ) ) ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to create default rasterizer state.\r\n" );
			ResetInternal();
			return false;
		}

		// Blend.
		LSG_BLEND_DESC bdOurBlendDesc;
		if ( FAILED( CreateBlendState( DefaultBlendDesc( bdOurBlendDesc ), &m_pbsDefaultBlendState ) ) ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to create default blend state.\r\n" );
			ResetInternal();
			return false;
		}

		// DepthStencil.
		LSG_DEPTH_STENCIL_DESC dsdOurDepthStencilDesc;
		if ( FAILED( CreateDepthStencilState( DefaultDepthStencilDesc( dsdOurDepthStencilDesc ), &m_pdssDefaultDepthStencilState ) ) ) {
			CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to create default depth/stencil state.\r\n" );
			ResetInternal();
			return false;
		}

		if ( !m_psmShaderMan ) {
			m_psmShaderMan = LSENEW CShaderManager();
			if ( !m_psmShaderMan ) {
				CStd::DebugPrintA( "CVulkan::InitVulkan(): Failed to create shader manager.\r\n" );
				ResetInternal();
				return false;
			}
		}

		return true;
	}

	/**
	 * Secondary initialization of Vulkan, after a device has been created.
	 */
	void LSE_CALL CVulkan::PostInitVulkan() {
	}

	/**
	 * Destroy Vulkan one time at engine shutdown.
	 */
	void LSE_CALL CVulkan::DestroyVulkan() {
		LSEDELETE m_psmShaderMan;
		m_psmShaderMan = NULL;


		ResetInternal();

		if ( m_mSamplerStateMap.Length() ) {
			CStd::DebugPrintA( "CVulkan::DestroyVulkan(): Unreleased sampler states detected.\r\n" );
		}
		if ( m_mRasterizerStateMap.Length() ) {
			CStd::DebugPrintA( "CVulkan::DestroyVulkan(): Unreleased rasterizer states detected.\r\n" );
		}
		if ( m_mBlendStateMap.Length() ) {
			CStd::DebugPrintA( "CVulkan::DestroyVulkan(): Unreleased blend states detected.\r\n" );
		}
		if ( m_mDepthStencilStateMap.Length() ) {
			CStd::DebugPrintA( "CVulkan::DestroyVulkan(): Unreleased depth/stencil states detected.\r\n" );
		}
	}

	/**
	 * Gets the string form of a Vulkan error code.
	 *
	 * \param _rRes The error code whose string to get.
	 * \return Returns the string form of the given error code.
	 */
	const char * LSE_CALL CVulkan::VulkanErrorToString( VkResult _rRes ) {
		switch ( _rRes ) {
			case VK_SUCCESS : { return "VK_SUCCESS"; }
			case VK_NOT_READY : { return "VK_NOT_READY"; }
			case VK_TIMEOUT : { return "VK_TIMEOUT"; }
			case VK_EVENT_SET : { return "VK_EVENT_SET"; }
			case VK_EVENT_RESET : { return "VK_EVENT_RESET"; }
			case VK_INCOMPLETE : { return "VK_INCOMPLETE"; }
			case VK_ERROR_OUT_OF_HOST_MEMORY : { return "VK_ERROR_OUT_OF_HOST_MEMORY"; }
			case VK_ERROR_OUT_OF_DEVICE_MEMORY : { return "VK_ERROR_OUT_OF_DEVICE_MEMORY"; }
			case VK_ERROR_INITIALIZATION_FAILED : { return "VK_ERROR_INITIALIZATION_FAILED"; }
			case VK_ERROR_DEVICE_LOST : { return "VK_ERROR_DEVICE_LOST"; }
			case VK_ERROR_MEMORY_MAP_FAILED : { return "VK_ERROR_MEMORY_MAP_FAILED"; }
			case VK_ERROR_LAYER_NOT_PRESENT : { return "VK_ERROR_LAYER_NOT_PRESENT"; }
			case VK_ERROR_EXTENSION_NOT_PRESENT : { return "VK_ERROR_EXTENSION_NOT_PRESENT"; }
			case VK_ERROR_FEATURE_NOT_PRESENT : { return "VK_ERROR_FEATURE_NOT_PRESENT"; }
			case VK_ERROR_INCOMPATIBLE_DRIVER : { return "VK_ERROR_INCOMPATIBLE_DRIVER"; }
			case VK_ERROR_TOO_MANY_OBJECTS : { return "VK_ERROR_TOO_MANY_OBJECTS"; }
			case VK_ERROR_FORMAT_NOT_SUPPORTED : { return "VK_ERROR_FORMAT_NOT_SUPPORTED"; }
			case VK_ERROR_SURFACE_LOST_KHR : { return "VK_ERROR_SURFACE_LOST_KHR"; }
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR : { return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; }
			case VK_SUBOPTIMAL_KHR : { return "VK_SUBOPTIMAL_KHR"; }
			case VK_ERROR_OUT_OF_DATE_KHR : { return "VK_ERROR_OUT_OF_DATE_KHR"; }
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR : { return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; }
			case VK_ERROR_VALIDATION_FAILED_EXT : { return "VK_ERROR_VALIDATION_FAILED_EXT"; }
			default : { return "Unrecognized"; }
		}
	}

	/**
	 * Prints a Vulkan error, if any.  If there is no error, false is returned.
	 *
	 * \param _pcPrefix Error prefix.
	 * \param _rRes The error code.
	 * \return Returns true if an error was printed.
	 */
	LSBOOL LSE_CALL CVulkan::PrintVulkanError( const char * _pcPrefix, VkResult _rRes ) {
		if ( _rRes == VK_SUCCESS ) { return false; }
		char szBuffer[256];
		CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%s%s\r\n",
			_pcPrefix, VulkanErrorToString( _rRes ) );
		CStd::DebugPrintA( szBuffer );
		return true;
	}

	/**
	 * Gets the memory type based on the given properties.
	 *
	 * \param _ui32TypeBits Type bits returned by functions that get memory requirements of images, buffers, etc.
	 * \param _fRequirementsMask Mask of requirements.
	 * \param _ui32TypeIndex The return value.
	 * \return Returns true if the requirements are met for the given memory properties.
	 */
	LSBOOL LSE_CALL CVulkan::MemoryTypeFromProperties( uint32_t _ui32TypeBits,
		VkFlags _fRequirementsMask, uint32_t &_ui32TypeIndex ) {
		// Search memory types to find first index with those properties.
		for ( uint32_t I = 0; I < 32; I++ ) {
			if ( (_ui32TypeBits & 1) == 1 ) {
				// Type is available; does it match user properties?
				if ( (m_pdmpMemProperties.memoryTypes[I].propertyFlags & _fRequirementsMask) == _fRequirementsMask ) {
					_ui32TypeIndex = I;
					return true;
				}
			}
			_ui32TypeBits >>= 1;
		}
		// No memory types matched.
		return false;
	}

	/**
	* Sets and applies all default render states.
	*/
	void LSE_CALL CVulkan::ApplyDefaultStates() {
		SetAllDefaultRenderStates();

		// States that are always set and non-configurable.
		/*GetDirectX9Device()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
		GetDirectX9Device()->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
		GetDirectX9Device()->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );*/

		// Other states.
		ApplyRenderStates( true );

		// Vertex buffers.
		//GetDirectX9Device()->SetVertexDeclaration( static_cast<IDirect3DVertexDeclaration9 *>(const_cast<LSG_INPUT_LAYOUT *>(m_rsCurRenderState.pilInputLayout)) );
		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			m_rsLastRenderState.pvbVertexBuffers[I].pvbVertBuffer = NULL;
		}
	}

	/**
	 * Applies all current render states.
	 *
	 * \param _bForce If true, all states are applied, even if they are redundant.
	 */
	void LSE_CALL CVulkan::ApplyRenderStates( LSBOOL _bForce ) {
	}

	/**
	 * Sets the current shader.  A shader must be set in order to render.
	 *
	 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
	 *	without a shader is not permitted.
	 */
	void LSE_FCALL CVulkan::SetShader( CShader * _psShader ) {
		m_psmShaderMan->SetShader( _psShader );
	}

	/**
	 * Gets the current shader.  If no shader is set, NULL is returned.
	 *
	 * \return Returns the current shader or NULL.
	 */
	CShader * LSE_FCALL CVulkan::GetShader() {
		return m_psmShaderMan->GetShader();
	}

#define LSG_DESCFUNCDEF_CLASS CVulkan
#include "../Gfx/LSGDescFuncDef.h"

#define LSG_CREATESTATESDEF_CLASS CVulkan
#include "../Gfx/LSGCreateStatesDef.h"

	/**
	 * Clear the active buffer of the given type(s).
	 *
	 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
	 */
	void LSE_FCALL CVulkan::Clear( uint32_t _ui32Mask ) {
		VkClearValue clear_values[2];
		clear_values[0].color.float32[0] = m_rsCurRenderState.fClearColor[0];
		clear_values[0].color.float32[1] = m_rsCurRenderState.fClearColor[1];
		clear_values[0].color.float32[2] = m_rsCurRenderState.fClearColor[2];
		clear_values[0].color.float32[3] = m_rsCurRenderState.fClearColor[3];
		clear_values[1].depthStencil.depth = m_rsCurRenderState.fClearDepth;
		clear_values[1].depthStencil.stencil = m_rsCurRenderState.ui8ClearStencil;
	}

	/**
	 * Release all resources and reset our internal state.
	 */
	void LSE_CALL CVulkan::ResetInternal() {
		ReleaseState( m_pssDefaultSamplerState );
		ReleaseState( m_prsDefaultRasterizerState );
		ReleaseState( m_pbsDefaultBlendState );
		ReleaseState( m_pdssDefaultDepthStencilState );

		m_vcbCmdBuffer.FreeCommandBuffer();

		for ( uint32_t I = m_vSwapImages.Length(); I--; ) {
			if ( m_vSwapImages[I].ivView != VK_NULL_HANDLE ) {
				::vkDestroyImageView( GetVulkanDevice(), m_vSwapImages[I].ivView, NULL );
			}
		}
		m_vSwapImages.Reset();
		DestroyDepthBuffer();
		
		if ( m_cpCommandPool != VK_NULL_HANDLE ) {
			::vkDestroyCommandPool( GetVulkanDevice(), m_cpCommandPool, NULL );
			m_cpCommandPool = VK_NULL_HANDLE;
		}
		if ( m_sSwapChain != VK_NULL_HANDLE ) {
			::vkDestroySwapchainKHR( GetVulkanDevice(), m_sSwapChain, NULL );
			m_sSwapChain = VK_NULL_HANDLE;
		}
		if ( m_sSurface != VK_NULL_HANDLE ) {
			::vkDestroySurfaceKHR( m_iInstance, m_sSurface, NULL );
			m_sSurface = VK_NULL_HANDLE;
		}
		if ( m_dDevice != VK_NULL_HANDLE ) {
			::vkDestroyDevice( m_dDevice, NULL );
			m_dDevice = VK_NULL_HANDLE;
		}
		if ( m_iInstance != VK_NULL_HANDLE ) {
			::vkDestroyInstance( m_iInstance, NULL );
			m_iInstance = VK_NULL_HANDLE;
		}
	}

	/**
	 * Gets the best physical device index.
	 *
	 * \param _sSurface The presentable surface.
	 * \param _ui32QueueIndex The returned index of the queue to use.
	 * \return Returns the best physical device index.
	 */
	uint32_t LSE_CALL CVulkan::GetBestGpuAndQueue( VkSurfaceKHR _sSurface, uint32_t &_ui32QueueIndex ) {
		_ui32QueueIndex = 0;
		uint32_t ui32TotalQueues;
		// Try for discrete GPU's first.
		for ( uint32_t I = 0; I < m_ui32NumDevices; ++I ) {
			if ( m_pdpDeviceProperties[I].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
				VkQueueFamilyProperties * pqfpProps = AllocateQueues( m_pdDevices[I], ui32TotalQueues );
				for ( uint32_t J = 0; J < ui32TotalQueues; ++J ) {
					if ( pqfpProps[J].queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
						VkBool32 bPresentable;
						::vkGetPhysicalDeviceSurfaceSupportKHR( m_pdDevices[I], J, _sSurface, &bPresentable );
						if ( bPresentable == VK_TRUE ) {
							LSEDELETE [] pqfpProps;
							_ui32QueueIndex = J;
							return I;
						}
					}
				}
				LSEDELETE [] pqfpProps;
			}
		}

		// Try for integrated GPU's first.
		for ( uint32_t I = 0; I < m_ui32NumDevices; ++I ) {
			if ( m_pdpDeviceProperties[I].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ) {
				VkQueueFamilyProperties * pqfpProps = AllocateQueues( m_pdDevices[I], ui32TotalQueues );
				for ( uint32_t J = 0; J < ui32TotalQueues; ++J ) {
					if ( pqfpProps[J].queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
						VkBool32 bPresentable;
						::vkGetPhysicalDeviceSurfaceSupportKHR( m_pdDevices[I], J, _sSurface, &bPresentable );
						if ( bPresentable == VK_TRUE ) {
							LSEDELETE [] pqfpProps;
							_ui32QueueIndex = J;
							return I;
						}
					}
				}
				LSEDELETE [] pqfpProps;
			}
		}
		return 0;
	}

	/**
	 * Allocates an array of device queues.  Must be freed with LSEDELETE [].
	 *
	 * \param _pdDevice The device.
	 * \param _ui32Total Holds the number of queues to which the return value points.
	 * \return Returns an array of VkQueueFamilyProperties structures which must be freed with LSEDELETE [] when no longer in use.
	 */
	VkQueueFamilyProperties * LSE_CALL CVulkan::AllocateQueues( VkPhysicalDevice _pdDevice, uint32_t &_ui32Total ) {
		::vkGetPhysicalDeviceQueueFamilyProperties( _pdDevice, &_ui32Total, NULL );
		VkQueueFamilyProperties * pqfpRet = LSENEW VkQueueFamilyProperties[_ui32Total];
		if ( pqfpRet ) {
			::vkGetPhysicalDeviceQueueFamilyProperties( _pdDevice, &_ui32Total, pqfpRet );
		}
		return pqfpRet;
	}

	/**
	 * Gets the format of the back surface.
	 *
	 * \param _pdDevice The device.
	 * \param _sSurface The window/back surface.
	 * \return Finds a valid format for the back buffer.
	 */
	VkFormat LSE_CALL CVulkan::GetBackSurfaceFormat( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface ) {
		uint32_t ui32Count = 0;
		if ( PrintVulkanError( "CVulkan::GetBackSurfaceFormat(): ",
			::vkGetPhysicalDeviceSurfaceFormatsKHR( _pdDevice, _sSurface,
			&ui32Count, NULL ) ) ) {
			return VK_FORMAT_UNDEFINED;
		}
		if ( !ui32Count ) {
			CStd::DebugPrintA( "CVulkan::GetBackSurfaceFormat(): No surface formats found on device.\r\n" );
			return VK_FORMAT_UNDEFINED;
		}
		CVectorPoD<VkSurfaceFormatKHR> vFormats;
		if ( !vFormats.Resize( ui32Count ) ) {
			CStd::DebugPrintA( "CVulkan::GetBackSurfaceFormat(): Memory failure trying to find surface formats.\r\n" );
			return VK_FORMAT_UNDEFINED;
		}

		// Now get the actual list.
		if ( PrintVulkanError( "CVulkan::GetBackSurfaceFormat(): ",
			::vkGetPhysicalDeviceSurfaceFormatsKHR( _pdDevice, _sSurface,
			&ui32Count, &vFormats[0] ) ) ) {
			return VK_FORMAT_UNDEFINED;
		}

		VkFormat fRet;
		if ( ui32Count == 1 && vFormats[0].format == VK_FORMAT_UNDEFINED ) {
			fRet = VK_FORMAT_B8G8R8A8_UNORM;
		}
		else {
			fRet = vFormats[0].format;
		}
		return fRet;
	}

	/**
	 * Gets the present mode.
	 *
	 * \param _pdDevice The device.
	 * \param _sSurface The window/back surface.
	 * \return Returns the present mode.
	 */
	VkPresentModeKHR LSE_CALL CVulkan::GetPresentMode( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface ) {
		uint32_t ui32Count = 0;
		if ( PrintVulkanError( "CVulkan::GetPresentMode(): ",
			::vkGetPhysicalDeviceSurfacePresentModesKHR( _pdDevice, _sSurface,
			&ui32Count, NULL ) ) ) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
		if ( !ui32Count ) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		CVectorPoD<VkPresentModeKHR> vPresentMode;
		if ( !vPresentMode.Resize( ui32Count ) ) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
		if ( PrintVulkanError( "CVulkan::GetPresentMode(): ",
			::vkGetPhysicalDeviceSurfacePresentModesKHR( _pdDevice, _sSurface,
			&ui32Count, &vPresentMode[0] ) ) ) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkPresentModeKHR pmRet = VK_PRESENT_MODE_FIFO_KHR;
		for ( uint32_t I = 0; I < ui32Count; ++I ) {
			if ( vPresentMode[I] == VK_PRESENT_MODE_MAILBOX_KHR ) {
				pmRet = vPresentMode[I];
				break;
			}
			if ( vPresentMode[I] == VK_PRESENT_MODE_IMMEDIATE_KHR ) {
				pmRet = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		return pmRet;
	}

	/**
	 * Creates the main depth buffer.  Command buffer must be open.
	 *
	 * \param _ui32Width Width of the texture to create.
	 * \param _ui32Height Height of the texture to create.
	 * \param _fFormat Format of the texture to create.
	 * \return Returns true if the creation of the depth buffer succeeded.
	 */
	LSBOOL LSE_CALL CVulkan::CreateDepthBuffer( uint32_t _ui32Width, uint32_t _ui32Height, VkFormat _fFormat ) {
		VkImageCreateInfo iciImgCreateInfo = {};
		VkFormatProperties fpProps;
		::vkGetPhysicalDeviceFormatProperties( GetVulkanPhysicalDevice(), _fFormat, &fpProps );
		if ( fpProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) {
			iciImgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
		}
		else if ( fpProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			iciImgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		}
		else {
			// Not a suitable format.
			CStd::DebugPrintA( "CVulkan::CreateDepthBuffer(): Not a suitable depth/stencil format.\r\n" );
			return false;
		}

		DestroyDepthBuffer();

		iciImgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		iciImgCreateInfo.pNext = NULL;
		iciImgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		iciImgCreateInfo.format = _fFormat;
		iciImgCreateInfo.extent.width = _ui32Width;
		iciImgCreateInfo.extent.height = _ui32Height;
		iciImgCreateInfo.extent.depth = 1;
		iciImgCreateInfo.mipLevels = 1;
		iciImgCreateInfo.arrayLayers = 1;
		iciImgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		iciImgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciImgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciImgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		iciImgCreateInfo.queueFamilyIndexCount = 0;
		iciImgCreateInfo.pQueueFamilyIndices = NULL;
		iciImgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iciImgCreateInfo.flags = 0;

		VkMemoryAllocateInfo maiAllocInfo = {};
		maiAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		maiAllocInfo.pNext = NULL;
		maiAllocInfo.allocationSize = 0;
		maiAllocInfo.memoryTypeIndex = 0;

		VkImageViewCreateInfo ivciViewCreateInfo = {};
		ivciViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ivciViewCreateInfo.pNext = NULL;
		ivciViewCreateInfo.image = VK_NULL_HANDLE;
		ivciViewCreateInfo.format = _fFormat;
		ivciViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		ivciViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		ivciViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		ivciViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		ivciViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		ivciViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ivciViewCreateInfo.subresourceRange.levelCount = 1;
		ivciViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ivciViewCreateInfo.subresourceRange.layerCount = 1;
		ivciViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciViewCreateInfo.flags = 0;

		// Create the image.
		if ( PrintVulkanError( "CVulkan::CreateDepthBuffer(): ",
			::vkCreateImage( GetVulkanDevice(), &iciImgCreateInfo, NULL, &m_vdbDepthStencil.iImage ) ) ) {
			return false;
		}

		VkMemoryRequirements mrReqs;
		::vkGetImageMemoryRequirements( GetVulkanDevice(), m_vdbDepthStencil.iImage, &mrReqs );
		maiAllocInfo.allocationSize = mrReqs.size;
		if ( !MemoryTypeFromProperties( mrReqs.memoryTypeBits,
			0,
			maiAllocInfo.memoryTypeIndex ) ) {
			// Memory type not found on this physical device.
			CStd::DebugPrintA( "CVulkan::CreateDepthBuffer(): Memory type not found on the physical device.\r\n" );
			return false;
		}

		// Allocate the memory.
		if ( PrintVulkanError( "CVulkan::CreateDepthBuffer(): ",
			AllocateMemory( &maiAllocInfo, NULL, m_vdbDepthStencil.dmMemory ) ) ) {
			return false;
		}

		// Bind the image.
		if ( PrintVulkanError( "CVulkan::CreateDepthBuffer(): ",
			::vkBindImageMemory( GetVulkanDevice(), m_vdbDepthStencil.iImage, m_vdbDepthStencil.dmMemory, 0 ) ) ) {
			return false;
		}
		m_vcbCmdBuffer.PipelineBarrierImage( m_vdbDepthStencil.iImage, VK_IMAGE_ASPECT_DEPTH_BIT,
			 VK_IMAGE_LAYOUT_UNDEFINED,
			 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
		ivciViewCreateInfo.image = m_vdbDepthStencil.iImage;
		if ( PrintVulkanError( "CVulkan::CreateDepthBuffer(): ",
			::vkCreateImageView( GetVulkanDevice(), &ivciViewCreateInfo, NULL, &m_vdbDepthStencil.ivView ) ) ) {
			return false;
		}
		return true;
	}

	/**
	 * Destroys the system depth buffer.
	 */
	void LSE_CALL CVulkan::DestroyDepthBuffer() {
		if ( m_vdbDepthStencil.ivView != VK_NULL_HANDLE ) {
			::vkDestroyImageView( GetVulkanDevice(), m_vdbDepthStencil.ivView, NULL );
			m_vdbDepthStencil.ivView = VK_NULL_HANDLE;
		}
		if ( m_vdbDepthStencil.iImage != VK_NULL_HANDLE ) {
			::vkDestroyImage( GetVulkanDevice(), m_vdbDepthStencil.iImage, NULL );
			m_vdbDepthStencil.iImage = VK_NULL_HANDLE;
		}
		FreeMemory( m_vdbDepthStencil.dmMemory, NULL );
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN

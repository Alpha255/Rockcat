#ifndef __INCLUDE_DEFINITIONS_HLSLI__
#define __INCLUDE_DEFINITIONS_HLSLI__

/***************************************************************************************************
	Vulkan binding decorations.

	Define _SPIRV_ when targeting SPIR-V (dxc -spirv) so the decorations below are emitted and the
	register() assignment becomes the source of truth for the binding (Slot, Set). Leave _SPIRV_
	undefined when targeting D3D, in which case the decorations collapse to nothing.

	Definitions.h deliberately lives on its own: it carries the engine's shared VSInput/VSOutput
	layout and collides with shaders that declare their own (e.g. ImGui.vert), so pull this header
	in instead whenever only the binding macros are needed.
***************************************************************************************************/

#if _SPIRV_
	#define VK_PUSH_CONSTANT      [[vk::push_constant]]
	#define VK_BINDING(Slot, Set) [[vk::binding(Slot, Set)]]
	#define VK_LOCATION(Index)    [[vk::location(Index)]]
	#define VK_OFFSET(Offset)     [[vk::offset(Offset)]]
#else  // _SPIRV_
	#define VK_PUSH_CONSTANT
	#define VK_BINDING(Slot, Set)
	#define VK_LOCATION(Index)
	#define VK_OFFSET(Offset)
#endif  // _SPIRV_

#endif  // __INCLUDE_DEFINITIONS_HLSLI__

#ifndef _INCLUDE_DEFINITIONS_
#define _INCLUDE_DEFINITIONS_

#if SPIRV
    #define VK_PUSH_CONSTANT      [[vk::push_constant]]
    #define VK_BINDING(Slot, Set) [[vk::binding(Slot, Set)]]
    #define VK_LOCATION(Index)    [[vk::location(Index)]]
    #define VK_OFFSET(Offset)     [[vk::offset(Offset)]]
#else
    #define VK_PUSH_CONSTANT
    #define VK_BINDING(Slot, Set)
    #define VK_LOCATION(Index)
    #define VK_OFFSET(Offset)
#endif

#define MATH_PI 3.141592654f
#define MATH_PI_2 6.283185307f
#define MATH_PI_DIV2 1.570796327f
#define MATH_PI_DIV4 0.785398163f
#define MATH_PI_INV 0.318309886f
#define MATH_PI_2_INV 0.159154943f

#pragma pack_matrix(row_major)

#endif
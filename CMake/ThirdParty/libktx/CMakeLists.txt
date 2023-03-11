file(GLOB_RECURSE FileList 
     ${RockCatRootPath}/ThirdParty/KTX-Software/include/*.h 
     ${RockCatRootPath}/ThirdParty/KTX-Software/lib/*.h
     ${RockCatRootPath}/ThirdParty/KTX-Software/lib/*.inl
     ${RockCatRootPath}/ThirdParty/KTX-Software/lib/*.c
     ${RockCatRootPath}/ThirdParty/KTX-Software/other_include/KHR/*.h
     ${RockCatRootPath}/ThirdParty/KTX-Software/utils/*.h)

include_directories(
    ${RockCatRootPath}/ThirdParty/KTX-Software/include
    ${RockCatRootPath}/ThirdParty/KTX-Software/other_include
    ${RockCatRootPath}/ThirdParty/KTX-Software/utils
    ${RockCatRootPath}/ThirdParty/KTX-Software/lib/basisu/zstd)

list(FILTER FileList EXCLUDE REGEX 
    "lib/basisu/zstd/zstddeclib.c|dfdutils/vulkan/vulkan_core.h|dfdutils/testbidirectionalmapping.c|dfdutils/createdfdtest.c|vkloader.c|dfdutils/endswap.c|lib/writer1.c|lib/writer2.c|lib/vk_funcs.c")

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/ThirdParty/KTX-Software
    SourceFileList ${FileList})

add_definitions(-DKHRONOS_STATIC -DLIBKTX)

add_library(libktx STATIC ${FileList})

set_target_properties(libktx PROPERTIES FOLDER "ThirdParty")
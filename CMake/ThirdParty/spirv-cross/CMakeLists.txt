set(CoreSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/GLSL.std.450.h
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_common.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross_containers.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross_error_handling.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_parser.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_parser.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cfg.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cfg.cpp
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv.h
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cpp.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cpp.hpp)

set(IncludeSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/include/spirv_cross/barrier.hpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/include/spirv_cross/external_interface.h
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/cmake/gitversion.in.h
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/include/spirv_cross/image.hpp
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/include/spirv_cross/internal_interface.hpp
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/include/spirv_cross/sampler.hpp
        ${RockCatRootPath}/ThirdParty/SPIRV-Cross/include/spirv_cross/thread_group.hpp)

set(GLSLSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_glsl.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_glsl.hpp)

set(MSLSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_msl.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_msl.hpp)

set(HLSLSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_hlsl.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_hlsl.hpp)

set(ReflectSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_reflect.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_reflect.hpp)

set(UtilSource
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross_util.cpp
		${RockCatRootPath}/ThirdParty/SPIRV-Cross/spirv_cross_util.hpp)

source_group(core FILES ${CoreSource})
source_group(include FILES ${IncludeSource})
source_group(glsl FILES ${GLSLSource})
source_group(hlsl FILES ${HLSLSource})
source_group(msl FILES ${MSLSource})
source_group(reflect FILES ${ReflectSource})
source_group(util FILES ${UtilSource})

include_directories(${RockCatRootPath}/ThirdParty/SPIRV-Cross)

add_library(spirv-cross STATIC 
    ${CoreSource}
    ${IncludeSource}
    ${GLSLSource}
    ${HLSLSource}
    ${ReflectSource}
    ${UtilSource})

set_target_properties(spirv-cross PROPERTIES FOLDER "ThirdParty")
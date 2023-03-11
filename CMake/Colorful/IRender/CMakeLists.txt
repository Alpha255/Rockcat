file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Colorful/IRenderer/**)

set(D3DCommanFile 
    ${RockCatRootPath}/Source/Colorful/D3D/DXGI_Interface.h
    ${RockCatRootPath}/Source/Colorful/D3D/DXGI_Interface.cpp)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Colorful/IRenderer 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(
    ${RockCatRootPath}/ThirdParty/Vulkan-Headers/include
    ${RockCatRootPath}/ThirdParty/KTX-Software/include)

source_group(D3D FILES ${D3DCommanFile})

add_library(IRenderer STATIC 
    ${FileList}
    ${D3DCommanFile})

set_target_properties(IRenderer PROPERTIES FOLDER "Colorful")
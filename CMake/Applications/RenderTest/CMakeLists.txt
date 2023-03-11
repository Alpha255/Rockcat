file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Applications/RenderTest/**)

set(Win32ResourceFiles
    ${RockCatRootPath}/Source/Applications/Win32Resource.h
    ${RockCatRootPath}/Assets/Icon/Resource.rc)
source_group(Resource FILES ${Win32ResourceFiles})

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Applications/RenderTest 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

if(WIN32)
    add_executable(RenderTest WIN32 
        ${FileList}
        ${Win32ResourceFiles})
    target_link_libraries(RenderTest
        Core
        Runtime
        IRenderer
        VulkanRenderer
        D3D12Renderer
        assimp
        ImGUI
        libktx
        spirv-cross
        ${WINLIBS}
        dxcompiler
        d3dcompiler
        d3d12
        dxgi)
endif()

set_target_properties(RenderTest PROPERTIES FOLDER "Applications")
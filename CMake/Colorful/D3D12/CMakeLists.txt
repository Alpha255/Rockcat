file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Colorful/D3D/D3D12/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Colorful/D3D/D3D12 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

add_library(D3D12Renderer STATIC ${FileList})

set_target_properties(D3D12Renderer PROPERTIES FOLDER "Colorful")
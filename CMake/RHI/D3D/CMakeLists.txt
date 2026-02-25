file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/RHI/D3D/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/RHI/D3D
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(
    ${RockCatRootPath}/Submodules
    ${RockCatRootPath}/Submodules/spdlog/include
    ${RockCatRootPath}/Submodules/cereal/include)

add_library(D3DRHI STATIC ${FileList})

set_target_properties(D3DRHI PROPERTIES 
    FOLDER "RHI")

target_link_libraries(D3DRHI PRIVATE Core Runtime)

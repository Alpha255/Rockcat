file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/RHI/Software/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/RHI/Software
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(
    ${RockCatRootPath}/Submodules
    ${RockCatRootPath}/Submodules/spdlog/include
    ${RockCatRootPath}/Submodules/cereal/include)

add_library(SoftwareRHI STATIC ${FileList})

set_target_properties(SoftwareRHI PROPERTIES 
    FOLDER "RHI")

target_link_libraries(SoftwareRHI PRIVATE Core Runtime)

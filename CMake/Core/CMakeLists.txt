file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Runtime/Core/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Runtime/Core 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(
    ${RockCatRootPath}/Submodules/magic_enum/include
    ${RockCatRootPath}/Submodules/spdlog/include)

add_library(Core STATIC ${FileList})

set_target_properties(Core PROPERTIES 
    FOLDER "Gear")

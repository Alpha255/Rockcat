file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Core/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Core 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

add_library(Core STATIC ${FileList})

set_target_properties(Core PROPERTIES FOLDER "Gear")
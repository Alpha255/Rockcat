file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Runtime/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Runtime 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

add_definitions(-DSTB_IMAGE_IMPLEMENTATION -DKHRONOS_STATIC -DLIBKTX)

include_directories(
    ${RockCatRootPath}/ThirdParty/glslang
    ${RockCatRootPath}/ThirdParty/KTX-Software/include
    ${RockCatRootPath}/ThirdParty/assimp/build/include
    ${RockCatRootPath}/ThirdParty/assimp/include)

add_library(Runtime STATIC ${FileList})

set_target_properties(Runtime PROPERTIES FOLDER "Gear")
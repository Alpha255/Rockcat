file(GLOB_RECURSE FileList 
     ${RockCatRootPath}/ThirdParty/ImGUI/*.h ${RockCatRootPath}/ThirdParty/ImGUI/*.cpp)

list(FILTER FileList EXCLUDE REGEX "examples|backends|binary_to_compressed_c|freetype")

include_directories(${RockCatRootPath}/ThirdParty/ImGUI)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/ThirdParty/ImGUI 
    SourceFileList ${FileList})

add_library(ImGUI STATIC ${FileList})

set_target_properties(ImGUI PROPERTIES FOLDER "ThirdParty")
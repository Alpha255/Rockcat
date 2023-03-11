project(Assimp VERSION 5.2.5)

set(ASSIMP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ASSIMP_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ASSIMP_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(ASSIMP_VERSION ${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH})
set(ASSIMP_SOVERSION 5)

# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git rev-parse --short=8 HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)

configure_file("${RockCatRootPath}/ThirdParty/assimp/assimp.pc.in" "${RockCatRootPath}/ThirdParty/assimp/build/assimp.pc" @ONLY)
configure_file(
    ${RockCatRootPath}/ThirdParty/assimp/revision.h.in
    ${RockCatRootPath}/ThirdParty/assimp/build/revision.h)
configure_file(
    ${RockCatRootPath}/ThirdParty/assimp/include/assimp/config.h.in
    ${RockCatRootPath}/ThirdParty/assimp/build/include/assimp/config.h)
configure_file(
    ${RockCatRootPath}/ThirdParty/assimp/contrib/zlib/zlib.pc.cmakein
    ${RockCatRootPath}/ThirdParty/assimp/build/contrib/zlib/zlib.pc @ONLY)
configure_file(
        ${RockCatRootPath}/ThirdParty/assimp/contrib/zlib/zconf.h.cmakein
        ${RockCatRootPath}/ThirdParty/assimp/build/contrib/zlib/zconf.h @ONLY)

file(GLOB_RECURSE FileList 
     ${RockCatRootPath}/ThirdParty/assimp/*.h 
     ${RockCatRootPath}/ThirdParty/assimp/*.cpp
     ${RockCatRootPath}/ThirdParty/assimp/*.hpp
     ${RockCatRootPath}/ThirdParty/assimp/*.c
     ${RockCatRootPath}/ThirdParty/assimp/*.cc)

include_directories(
    ${RockCatRootPath}/ThirdParty/assimp/build/include
    ${RockCatRootPath}/ThirdParty/assimp/build
    ${RockCatRootPath}/ThirdParty/assimp/include
    ${RockCatRootPath}/ThirdParty/assimp/code
    ${RockCatRootPath}/ThirdParty/assimp
    ${RockCatRootPath}/ThirdParty/assimp/contrib/zlib
    ${RockCatRootPath}/ThirdParty/assimp/build/contrib/zlib
    ${RockCatRootPath}/ThirdParty/assimp/contrib/rapidjson/include
    ${RockCatRootPath}/ThirdParty/assimp/contrib
    ${RockCatRootPath}/ThirdParty/assimp/contrib/pugixml/src
    ${RockCatRootPath}/ThirdParty/assimp/contrib/unzip
    ${RockCatRootPath}/ThirdParty/assimp/contrib/irrXML
    ${RockCatRootPath}/ThirdParty/assimp/contrib/openddlparser/include)

list(FILTER FileList EXCLUDE REGEX 
    "build/CMakeFiles|contrib/gtest|test/|IFCReaderGen_4|tools/|include/port/AndroidJNI|port/|code/AMF|samples/|contrib/zip/test|contrib/draco|contrib/zlib/contrib")

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/ThirdParty/assimp
    SourceFileList ${FileList})

add_definitions(
    -DASSIMP_BUILD_DLL_EXPORT 
    -DASSIMP_BUILD_NO_M3D_IMPORTER
    -DASSIMP_BUILD_NO_M3D_EXPORTER
    -DWIN32_LEAN_AND_MEAN
    -DASSIMP_BUILD_NO_C4D_IMPORTER
    -DMINIZ_USE_UNALIGNED_LOADS_AND_STORES=0
    -DASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1
    -DRAPIDJSON_HAS_STDSTRING=1
    -DRAPIDJSON_NOMEMBERITERATORCLASS
    -D_SCL_SECURE_NO_WARNINGS
    -D_CRT_SECURE_NO_WARNINGS
    -DOPENDDLPARSER_BUILD
    -Dassimp_EXPORTS)

if(MSVC)
    #add_compile_options(/GR)
    add_compile_options(/wd4244 /wd4189 /wd4131 /wd4127 /wd4996)
endif()

add_library(assimp SHARED ${FileList})

set_target_properties(assimp PROPERTIES FOLDER "ThirdParty")
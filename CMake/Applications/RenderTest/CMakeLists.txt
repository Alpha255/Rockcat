file(GLOB_RECURSE FileList 
    ${RockCatRootPath}/Source/Applications/RenderTest/*.cpp
    ${RockCatRootPath}/Source/Applications/RenderTest/*.h)

set(Win32ResourceFiles
    ${RockCatRootPath}/Source/Runtime/Application/Win32/Resource.rc)

source_group(Resource FILES ${Win32ResourceFiles})

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Applications/RenderTest 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(
    ${Vulkan_INCLUDE_DIRS}
    ${RockCatRootPath}/Submodules/cereal/include
    ${RockCatRootPath}/Submodules/spdlog/include
    ${RockCatRootPath}/Submodules/magic_enum/include
    ${RockCatRootPath}/Submodules/assimp/include
    ${RockCatRootPath}/Submodules/assimp/build/include
    ${RockCatRootPath}/Submodules/imgui)

if(WIN32)
    add_executable(RenderTest WIN32 
        ${FileList}
        ${Win32ResourceFiles})
        
    target_link_libraries(RenderTest PRIVATE
        Core
        Runtime
        Vulkan::Vulkan
        dxcompiler
        d3dcompiler
        d3d12
        dxgi)
        
    if(EXISTS ${RockCatRootPath}/Submodules/assimp/build/libassimp.lib)
        target_link_libraries(RenderTest PRIVATE assimp)
    endif()
endif()

set_target_properties(RenderTest PROPERTIES 
    FOLDER "Applications"
    RUNTIME_OUTPUT_DIRECTORY "${RockCatRootPath}/Bin/${CMAKE_BUILD_TYPE}")

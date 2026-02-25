file(GLOB_RECURSE FileList 
    ${RockCatRootPath}/Source/Runtime/Application/**
    ${RockCatRootPath}/Source/Runtime/Asset/**
    ${RockCatRootPath}/Source/Runtime/Async/**
    ${RockCatRootPath}/Source/Runtime/Editor/**
    ${RockCatRootPath}/Source/Runtime/Profile/**
    ${RockCatRootPath}/Source/Runtime/Rendering/**
    ${RockCatRootPath}/Source/Runtime/Scene/**
    ${RockCatRootPath}/Source/Runtime/Services/**
    ${RockCatRootPath}/Source/Runtime/System/**
    ${RockCatRootPath}/Source/Runtime/View/**
    ${RockCatRootPath}/Source/Runtime/World/**
    ${RockCatRootPath}/Source/Runtime/TaskFlow/**
    ${RockCatRootPath}/Source/Runtime/Paths.cpp
    ${RockCatRootPath}/Source/Runtime/Paths.h)

list(FILTER FileList EXCLUDE REGEX "test|Test")

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Runtime 
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

add_definitions(-DSTB_IMAGE_IMPLEMENTATION -DKHRONOS_STATIC -DLIBKTX)

include_directories(
    ${Vulkan_INCLUDE_DIRS}
    ${RockCatRootPath}/Submodules
    ${RockCatRootPath}/Submodules/cereal/include
    ${RockCatRootPath}/Submodules/spdlog/include
    ${RockCatRootPath}/Submodules/magic_enum/include
    ${RockCatRootPath}/Submodules/taskflow
    ${RockCatRootPath}/Submodules/taskflow/taskflow
    ${RockCatRootPath}/Submodules/taskflow/taskflow/utility
    ${RockCatRootPath}/Submodules/filewatch
    ${RockCatRootPath}/Submodules/imgui
    ${RockCatRootPath}/Submodules/stb
    ${RockCatRootPath}/Submodules/assimp/include
    ${RockCatRootPath}/Submodules/assimp/build/include)

add_library(Runtime STATIC ${FileList})

set_target_properties(Runtime PROPERTIES 
    FOLDER "Gear")

target_link_libraries(Runtime PRIVATE Core Vulkan::Vulkan)

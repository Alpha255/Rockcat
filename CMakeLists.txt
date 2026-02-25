cmake_minimum_required(VERSION 3.20)

project(Rockcat VERSION 1.0.0)

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_compile_options(/MP)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-mavx -msse4.1)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(AutoSetSourceFileFilters)
    cmake_parse_arguments(ARG "" "BasePath" "SourceFileList" ${ARGN})
    foreach(FilePath IN ITEMS ${ARG_SourceFileList})
        set(FilePathRelative "${FilePath}")
        if (IS_ABSOLUTE "${FilePath}")
            file(RELATIVE_PATH FilePathRelative "${ARG_BasePath}" "${FilePath}")
        endif()
        get_filename_component(SourcePath "${FilePathRelative}" PATH)
        string(REPLACE "/" "\\" SourcePathMsvc "${SourcePath}")
        source_group("${SourcePathMsvc}" FILES "${FilePath}")
    endforeach()
endfunction()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(RockCatRootPath ${CMAKE_CURRENT_LIST_DIR})

find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Vulkan SDK found: ${Vulkan_VERSION}")
endif()

function(AddPrivateIncludeDirectories)
    include_directories(${RockCatRootPath})
    include_directories(${RockCatRootPath}/Source)
    include_directories(${RockCatRootPath}/Source/Runtime)
    include_directories(${RockCatRootPath}/Submodules/cereal/include)
endfunction()

function(AddPrivateDefinitions)
    if(WIN32)
        add_definitions(-DVK_USE_PLATFORM_WIN32_KHR=1 -DPLATFORM_WIN32=1)
    endif()
endfunction()

add_subdirectory(CMake/Core)
add_subdirectory(CMake/Runtime)
add_subdirectory(CMake/RHI)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Applications)
    add_subdirectory(CMake/Applications)
endif()

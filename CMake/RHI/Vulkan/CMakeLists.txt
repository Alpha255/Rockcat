file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/RHI/Vulkan/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/RHI/Vulkan
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(
    ${Vulkan_INCLUDE_DIRS}
    ${RockCatRootPath}/Submodules
    ${RockCatRootPath}/Submodules/spdlog/include
    ${RockCatRootPath}/Submodules/cereal/include)

add_library(VulkanRHI STATIC ${FileList})

set_target_properties(VulkanRHI PROPERTIES 
    FOLDER "RHI")

target_link_libraries(VulkanRHI PRIVATE Core Runtime)

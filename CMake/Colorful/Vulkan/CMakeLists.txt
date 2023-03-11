file(GLOB_RECURSE FileList ${RockCatRootPath}/Source/Colorful/Vulkan/**)

AutoSetSourceFileFilters(
    BasePath ${RockCatRootPath}/Source/Colorful/Vulkan
    SourceFileList ${FileList})

AddPrivateIncludeDirectories()
AddPrivateDefinitions()

include_directories(${RockCatRootPath}/ThirdParty/Vulkan-Headers/include)

add_library(VulkanRenderer STATIC ${FileList})

set_target_properties(VulkanRenderer PROPERTIES FOLDER "Colorful")
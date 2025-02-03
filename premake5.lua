workspace "Rockcat"
	location "./"
	configurations { "Debug", "DebugEditor", "Release", "ReleaseEditor" }
	objdir "$(SolutionDir)Out/Intermediate"
	targetdir "$(SolutionDir)Out/Intermediate/$(Platform)/$(Configuration)/$(ProjectName)"
	characterset "Unicode"
	platforms "x64"
	targetname "$(ProjectName)"
	warnings "Extra"
	dpiawareness "High"
	systemversion "latest"
	symbolspath "$(IntDir)$(TargetName).pdb"
	debugdir "$(SolutionDir)Out"
	--flags { "MultiProcessorCompile", "NoIncrementalLink" }
	cppdialect "C++20"
	flags { "MultiProcessorCompile", }
	filter { "configurations:Debug" }
		symbols "On"
		optimize "Debug"
		defines { "_DEBUG", "_UNICODE", "UNICODE", "CONFIGURATION=\"_Debug\"", "IMGUI_USE_WCHAR32", "WITH_EDITOR=0" }
	filter { "configurations:DebugEditor" }
		symbols "On"
		optimize "Debug"
		defines { "_DEBUG", "_UNICODE", "UNICODE", "CONFIGURATION=\"_Debug\"", "IMGUI_USE_WCHAR32", "WITH_EDITOR=1" }
	filter { "configurations:Release" }
		optimize "Speed"
		defines { "NDEBUG", "_UNICODE", "UNICODE", "CONFIGURATION=\"_Release\"", "IMGUI_USE_WCHAR32", "WITH_EDITOR=0" }
	filter { "configurations:ReleaseEditor" }
		optimize "Speed"
		defines { "NDEBUG", "_UNICODE", "UNICODE", "CONFIGURATION=\"_Release\"", "IMGUI_USE_WCHAR32", "WITH_EDITOR=1" }
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
	filter {}
	disablewarnings { "4996", "5054" }
	buildoptions { "/Zc:char8_t-" }

	group "Gear"
		project "Runtime"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files {
				"./Source/Runtime/**",
				"./Assets/Shaders/**"
			}
			removefiles {
				"./Assets/**.json"
			}
			vpaths {
				["Source/*"] = { "./Source/Runtime/**" },
			}
			defines { "STB_IMAGE_IMPLEMENTATION" }
			includedirs {
				"$(SolutionDir)",
				"$(SolutionDir)Source",
				"$(SolutionDir)Source/Runtime",
				"$(SolutionDir)Assets",
				"$(SolutionDir)Submodules/cereal/include",
				"$(SolutionDir)Submodules/spdlog/include",
				"$(SolutionDir)Submodules/assimp/build/include",
				"$(SolutionDir)Submodules/assimp/include",
				"$(VK_SDK_PATH)/Include",
			}
			libdirs {
				"$(VK_SDK_PATH)/Lib"
			}
			filter { "configurations:Debug or DebugEditor" }
				links {
					"spirv-cross-core"
				}
			filter { "configurations:Release or ReleaseEditor" }
				links {
					"spirv-cross-core"
				}

		group "RHI"
			project "VulkanRHI"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files {
					"./Source/RHI/Vulkan/**",
					"./Assets/Configs/VkLayerAndExtensionConfigs.json"
				}
				removefiles {
					"./Source/RHI/Vulkan/Documents/**"
				}
				includedirs {
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)Source/Runtime",
					"$(SolutionDir)Submodules/cereal/include",
					"$(SolutionDir)Submodules/spdlog/include",
					"$(VK_SDK_PATH)/Include",
				}
				vpaths {
					["Configs"] = {
						"./Assets/Configs/VkLayerAndExtensionConfigs.json"
					},
					[""] = {
						"./Source/RHI/Vulkan/**",
					}
				}
			project "D3D12RHI"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files { 
					"./Source/RHI/D3D/D3D12/**",
					"./Source/RHI/D3D/DXGIInterface.h",
					"./Source/RHI/D3D/DXGIInterface.cpp",
				}
				includedirs { 
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)Source/Runtime",
					"$(SolutionDir)Submodules/cereal/include",
					"$(SolutionDir)Submodules/spdlog/include",
					"$(VK_SDK_PATH)/Include",
				}
				links {
					"d3d12",
					"dxgi"
				}
				project "D3D11RHI"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files { 
					"./Source/RHI/D3D/D3D11/**",
					"./Source/RHI/D3D/DXGIInterface.h",
					"./Source/RHI/D3D/DXGIInterface.cpp",
				}
				includedirs { 
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)Source/Runtime",
					"$(SolutionDir)Submodules/cereal/include",
					"$(SolutionDir)Submodules/spdlog/include",
					"$(VK_SDK_PATH)/Include",
				}
				links {
					"d3d11",
					"dxgi"
				}
	group "Submodules"
		project "imgui"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files { 
				"./Submodules/imgui/**.h", 
				"./Submodules/imgui/**.cpp" 
			}
			removefiles { 
				"./Submodules/imgui/examples/**", 
				"./Submodules/imgui/misc/fonts/**",
				"./Submodules/imgui/misc/freetype/**",
				"./Submodules/imgui/backends/**",
				"./Submodules/imgui/plugins/ImGuiAl/**.cpp"
			}
			includedirs { 
				"$(SolutionDir)Submodules/imgui"
			}

		project "assimp"
  			kind "SharedLib"
			location "./Out/Intermediate/VCProjects"
			targetdir "$(SolutionDir)Out"
			targetname "$(ProjectName)_$(Configuration)"
			buildoptions { "/bigobj" }
			disablewarnings { "4131", "4127", "4244" }
			--implibname "$(SolutionDir)Out/Intermediate/$(Configuration)/$(ProjectName)"
			files {
				"./Submodules/assimp/**.h",
				"./Submodules/assimp/**.cpp",
				"./Submodules/assimp/**.hpp",
				"./Submodules/assimp/**.c",
				"./Submodules/assimp/**.cc",
			}
			removefiles { 
				"./Submodules/assimp/contrib/zlib/contrib/inflate86/**",
				"./Submodules/assimp/code/AssetLib/IFC/IFCReaderGen_4.h",
				"./Submodules/assimp/code/AssetLib/IFC/IFCReaderGen_4.cpp",
				"./Submodules/assimp/contrib/zlib/contrib/**",
				"./Submodules/assimp/test/**",
				"./Submodules/assimp/tools/**",
				"./Submodules/assimp/contrib/gtest/**",
				"./Submodules/assimp/build/CMakeFiles/**",
				"./Submodules/assimp/include/port/AndroidJNI/**",
				"./Submodules/assimp/port/**",
				"./Submodules/assimp/code/AMF/**",
				"./Submodules/assimp/samples/**",
				"./Submodules/assimp/contrib/zip/test/**",
				"./Submodules/assimp/contrib/draco/**",
				"./Submodules/assimp/contrib/googletest/**"
			}
			includedirs { 
				"$(SolutionDir)Submodules/assimp/build/include",
				"$(SolutionDir)Submodules/assimp/build",
				"$(SolutionDir)Submodules/assimp/include",
				"$(SolutionDir)Submodules/assimp/code",
				"$(SolutionDir)Submodules/assimp",
				"$(SolutionDir)Submodules/assimp/contrib/zlib",
				"$(SolutionDir)Submodules/assimp/build/contrib/zlib",
				"$(SolutionDir)Submodules/assimp/contrib/rapidjson/include",
				"$(SolutionDir)Submodules/assimp/contrib/",
				"$(SolutionDir)Submodules/assimp/contrib/pugixml/src",
				"$(SolutionDir)Submodules/assimp/contrib/unzip",
				"$(SolutionDir)Submodules/assimp/contrib/irrXML",
				"$(SolutionDir)Submodules/assimp/contrib/openddlparser/include",
				"$(SolutionDir)Submodules/assimp/contrib/utf8cpp/source"
			}
			filter { "configurations:Debug or DebugEditor" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"_DEBUG",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
					"ASSIMP_BUILD_NO_USD_IMPORTER",
					"ASSIMP_BUILD_NO_IFC_IMPORTER",
					"MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
					"ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
					"RAPIDJSON_HAS_STDSTRING=1",
					"RAPIDJSON_NOMEMBERITERATORCLASS",
					"ASSIMP_BUILD_DLL_EXPORT",
					"_SCL_SECURE_NO_WARNINGS",
					"_CRT_SECURE_NO_WARNINGS",
					"OPENDDLPARSER_BUILD",
					"assimp_EXPORTS",
				}
			filter { "configurations:Release or ReleaseEditor" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"NDEBUG",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
					"ASSIMP_BUILD_NO_USD_IMPORTER",
					"ASSIMP_BUILD_NO_IFC_IMPORTER",
					"MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
					"ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
					"RAPIDJSON_HAS_STDSTRING=1",
					"RAPIDJSON_NOMEMBERITERATORCLASS",
					"ASSIMP_BUILD_DLL_EXPORT",
					"_SCL_SECURE_NO_WARNINGS",
					"_CRT_SECURE_NO_WARNINGS",
					"OPENDDLPARSER_BUILD",
					"assimp_EXPORTS", 
				}
	group "Applications"
		project "RenderTest"
		kind "WindowedApp"
		language "C++"
		location "./Out/Intermediate/VCProjects"
		targetname "$(ProjectName)_$(Configuration)"
		files {
			"./Source/Applications/RenderTest/**",
			"./Source/Runtime/Engine/Application/Resource.rc"
		}
		includedirs { 
			"$(SolutionDir)",
			"$(SolutionDir)Source",
			"$(SolutionDir)Source/Runtime",
			"$(SolutionDir)Submodules/cereal/include",
			"$(SolutionDir)Submodules/spdlog/include",
			"$(VK_SDK_PATH)/Include",
		}
		libdirs {
			"$(VK_SDK_PATH)/Lib"
		}
		targetdir "$(SolutionDir)Out"
		links { 
			"Runtime",  
			"VulkanRHI",
			--"D3D12RHI",
			"vulkan-1",
			"dxcompiler",
			"d3dcompiler",
			"assimp",
			"imgui"
		}
		vpaths {
			["Resource"] = {
				"./Source/Runtime/Engine/Application/Resource.rc"
			},
			[""] = {
				"./Source/Applications/RenderTest/**",
			}
		}

		project "ImGuiEditor"
		kind "WindowedApp"
		language "C++"
		location "./Out/Intermediate/VCProjects"
		targetname "$(ProjectName)_$(Configuration)"
		files {
			"./Source/Applications/ImGuiEditor/**",
			"./Source/Runtime/Engine/Application/Resource.rc",
			"./Submodules/imgui/backends/imgui_impl_vulkan.h",
			"./Submodules/imgui/backends/imgui_impl_vulkan.cpp",
			"./Submodules/imgui/backends/imgui_impl_Win32.h",
			"./Submodules/imgui/backends/imgui_impl_win32.cpp",
			"./Submodules/imgui/backends/imgui_impl_dx12.h",
			"./Submodules/imgui/backends/imgui_impl_dx12.cpp"
		}
		includedirs {
			"$(SolutionDir)",
			"$(SolutionDir)Source",
			"$(SolutionDir)Source/Runtime",
			"$(SolutionDir)Submodules/cereal/include",
			"$(SolutionDir)Submodules/spdlog/include",
			"$(SolutionDir)Submodules/imgui",
			"$(VK_SDK_PATH)/Include",
		}
		libdirs {
			"$(VK_SDK_PATH)/Lib"
		}
		targetdir "$(SolutionDir)Out"
		links {
			"Runtime",
			"assimp",
			"imgui",
			"vulkan-1",
			"dxcompiler",
			"d3dcompiler",
			"VulkanRHI",
			"d3d12",
			"dxgi"
		}
		vpaths {
			["Resource"] = {
				"./Source/Runtime/Engine/Application/Resource.rc"
			},
			[""] = {
				"./Source/Applications/ImGuiEditor/**",
			},
			["ImGuiImpl"] = {
				"./Submodules/imgui/backends/imgui_impl_vulkan.h",
				"./Submodules/imgui/backends/imgui_impl_vulkan.cpp",
				"./Submodules/imgui/backends/imgui_impl_Win32.h",
				"./Submodules/imgui/backends/imgui_impl_win32.cpp",
				"./Submodules/imgui/backends/imgui_impl_dx12.h",
				"./Submodules/imgui/backends/imgui_impl_dx12.cpp"
			}
		}


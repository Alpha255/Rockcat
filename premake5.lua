workspace "Rockcat"
	location "./"
	configurations { "Debug", "Release" }
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
	cppdialect "C++17"
	flags { "MultiProcessorCompile", }
	filter { "configurations:Debug" }
		symbols "On"
		optimize "Debug"
		defines { "_DEBUG", "_UNICODE", "UNICODE", "CONFIGURATION=\"_Debug\"" }
	filter { "configurations:Release" }
		optimize "Speed"
		defines { "NDEBUG", "_UNICODE", "UNICODE", "CONFIGURATION=\"\"" }
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
	filter {}

	group "Gear"
		project "Runtime"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files "./Source/Runtime/**"
			defines { "STB_IMAGE_IMPLEMENTATION" }
			includedirs {
				"$(SolutionDir)",
				"$(SolutionDir)Source",
				"$(SolutionDir)Submodules/cereal/include",
				"$(SolutionDir)Submodules/spdlog/include",
				"$(SolutionDir)Submodules/assimp/build/include",
				"$(SolutionDir)Submodules/assimp/include",
				"$(VK_SDK_PATH)/Include",
			}
			libdirs {
				"$(VK_SDK_PATH)/Lib"
			}
			filter { "configurations:Debug" }
				links {
					"spirv-cross-cored"
				}
			filter { "configurations:Release" }
				links {
					"spirv-cross-core"
				}

		group "RHI"
			project "VulkanRHI"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files "./Source/RHI/Vulkan/**"
				includedirs {
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)Submodules/cereal/include",
					"$(SolutionDir)Submodules/spdlog/include",
					"$(VK_SDK_PATH)/Include",
				}
			project "D3D12RHI"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files { 
					"./Source/RHI/D3D/D3D12/**",
					"./Source/RHI/D3D/DXGI_Interface.h",
					"./Source/RHI/D3D/DXGI_Interface.cpp",
				}
				includedirs { 
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)Submodules/cereal/include",
					"$(SolutionDir)Submodules/spdlog/include",
				}
				links {
					"d3d12",
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
				"./Submodules/imgui/backends/**"
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
			--disablewarnings { "4819", "4189", "4131", "4996", "4127", "4244" }
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
				"./Submodules/assimp/contrib/draco/**"
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
				"$(SolutionDir)Submodules/assimp/contrib/openddlparser/include"
			}
			filter { "configurations:Debug" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"_DEBUG",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
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
			filter { "configurations:Release" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"NDEBUG",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
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
			"$(SolutionDir)/Source",
			"$(SolutionDir)Submodules/cereal/include",
			"$(SolutionDir)Submodules/spdlog/include"
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


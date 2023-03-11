function appResourceFiles()
	files { 
		"./Assets/Icon/Resource.rc",
		"./Applications/Resource.h"
	}
end

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
		project "Core"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files "./Source/Core/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)Source",
				"$(SolutionDir)ThirdParty/cereal/include",
				"$(SolutionDir)ThirdParty/spdlog/include"
			}
		project "Runtime"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files "./Source/Runtime/**"
			defines { "STB_IMAGE_IMPLEMENTATION", "KHRONOS_STATIC", "LIBKTX" }
			includedirs {
				"$(SolutionDir)",
				"$(SolutionDir)Source",
				"$(SolutionDir)ThirdParty/cereal/include",
				"$(SolutionDir)ThirdParty/spdlog/include",
				"$(SolutionDir)ThirdParty/glslang",
				"$(SolutionDir)ThirdParty/KTX-Software/include",
				"$(SolutionDir)ThirdParty/assimp/build/include",
				"$(SolutionDir)ThirdParty/assimp/include",
			}
			libdirs {
				"$(SolutionDir)ThirdParty/dxc/lib/x64"
			}
			links {
				"spirv-cross",
				"libktx",
				"dxcompiler",
				"d3dcompiler",
				"assimp",
				"ImGui"
			}
			postbuildcommands {
				"{COPY} $(SolutionDir)ThirdParty/dxc/bin/x64/*.dll $(SolutionDir)Out"
			}
		group "Colorful"
			project "IRenderer"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files "./Source/Colorful/IRenderer/**"
				includedirs {
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)ThirdParty",
					"$(SolutionDir)ThirdParty/cereal/include",
					"$(SolutionDir)ThirdParty/spdlog/include",
					"$(SolutionDir)ThirdParty/KTX-Software/include",
					"$(SolutionDir)ThirdParty/Vulkan-Headers/include",
				}
			project "VulkanRenderer"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files "./Source/Colorful/Vulkan/**"
				includedirs {
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)ThirdParty/cereal/include",
					"$(SolutionDir)ThirdParty/spdlog/include",
					"$(SolutionDir)ThirdParty/Vulkan-Headers/include",
				}
			project "D3D12Renderer"
				kind "StaticLib"
				language "C++"
				location "./Out/Intermediate/VCProjects"
				files { 
					"./Source/Colorful/D3D/D3D12/**",
					"./Source/Colorful/D3D/DXGI_Interface.h",
					"./Source/Colorful/D3D/DXGI_Interface.cpp",
				}
				includedirs { 
					"$(SolutionDir)",
					"$(SolutionDir)Source",
					"$(SolutionDir)ThirdParty/cereal/include",
					"$(SolutionDir)ThirdParty/spdlog/include",
				}
				links {
					"d3d12",
					"dxgi"
				}
--[[
	group "Colorful"
		project "IRenderer"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files "./Colorful/Gfx/**"
			removefiles "./Colorful/Gfx/ImGui/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)ThirdParty",
				"$(SolutionDir)ThirdParty/cereal/include",
				"$(SolutionDir)ThirdParty/spdlog/include",
				"$(SolutionDir)ThirdParty/glslang",
				"$(SolutionDir)ThirdParty/KTX-Software/include",
				"$(SolutionDir)ThirdParty/assimp/build/include",
				"$(SolutionDir)ThirdParty/assimp/include",
				"$(SolutionDir)ThirdParty/Vulkan-Headers/include",
			}
			defines { "STB_IMAGE_IMPLEMENTATION", "KHRONOS_STATIC", "LIBKTX" }
			libdirs {
				"$(SolutionDir)ThirdParty/dxc/lib/x64"
			}
			links {
				"spirv-cross",
				"libktx",
				"dxcompiler",
				"d3dcompiler",
				"assimp"
			}
			postbuildcommands {
				"{COPY} $(SolutionDir)ThirdParty/dxc/bin/x64/*.dll $(SolutionDir)Out"
			}

		project "VulkanRenderer"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			targetname "$(ProjectName)_$(Configuration)"
			targetdir "$(SolutionDir)Out"
			files "./Colorful/Vulkan/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)ThirdParty/Vulkan-Headers/include",
				"$(SolutionDir)ThirdParty/cereal/include",
				"$(SolutionDir)ThirdParty/spdlog/include",
			}
			-- defines { "DYNAMIC_LIB" }
			--implibname "$(SolutionDir)Out/Intermediate/$(Configuration)/$(ProjectName)"
			links { 
				"Gear", 
				"IRenderer",
				"spirv-cross",
				"libktx",
				"dxcompiler",
				"d3dcompiler",
				"assimp",
			}
--]]
--[[
		project "D3D11Renderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files { 
				"./Colorful/D3D/D3D11/**",
				"./Colorful/D3D/DXGI_Interface.h",
				"./Colorful/D3D/DXGI_Interface.cpp",
			}
			includedirs { "$(SolutionDir)" }
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out/Intermediate/$(Configuration)/$(ProjectName)"
			links { 
				"Gear",
				"d3d11",
				"dxgi",
				"IRenderer"
			}

		project "SoftwareRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Software/**"
			includedirs { "$(SolutionDir)" }
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out/Intermediate/$(Configuration)/$(ProjectName)"
			links { 
				"Gear",
				"IRenderer"
			}
		
		project "ImGuiRenderer"
			kind "StaticLib"
			language "C++"
			location "./Projects/"
			files "./Colorful/Gfx/ImGui/**"
			includedirs { "$(SolutionDir)" }
]]
	group "ThirdParty"
		project "libktx"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files {
				"./ThirdParty/KTX-Software/other_include/KHR/**",
				"./ThirdParty/KTX-Software/include/**",
				"./ThirdParty/KTX-Software/lib/**.h",
				"./ThirdParty/KTX-Software/lib/**.c",
				"./ThirdParty/KTX-Software/lib/**.inl",
				"./ThirdParty/KTX-Software/lib/basisu/zstd/**",
			}
			removefiles {
				"./ThirdParty/KTX-Software/lib/basisu/zstd/zstddeclib.c",
				"./ThirdParty/KTX-Software/lib/dfdutils/vulkan/vulkan_core.h",
				"./ThirdParty/KTX-Software/lib/dfdutils/endswap.c",
				"./ThirdParty/KTX-Software/lib/dfdutils/testbidirectionalmapping.c",
				"./ThirdParty/KTX-Software/lib/dfdutils/createdfdtest.c",
				"./ThirdParty/KTX-Software/lib/gl_funclist.inl",
				"./ThirdParty/KTX-Software/lib/gl_funcs.c",
				"./ThirdParty/KTX-Software/lib/gl_funcs.h",
				"./ThirdParty/KTX-Software/lib/vk_funclist.inl",
				"./ThirdParty/KTX-Software/lib/vk_funcs.h",
				"./ThirdParty/KTX-Software/lib/vk_funcs.c",
				"./ThirdParty/KTX-Software/lib/vk_format_check.c",
				"./ThirdParty/KTX-Software/lib/vk_format_enum.h",
				"./ThirdParty/KTX-Software/lib/vk_format_str.c",
				"./ThirdParty/KTX-Software/lib/vkloader.c",
				"./ThirdParty/KTX-Software/lib/writer1.c",
				"./ThirdParty/KTX-Software/lib/writer2.c",
			}
			includedirs {
				"$(SolutionDir)",
				"$(SolutionDir)ThirdParty/KTX-Software/include",
				"$(SolutionDir)ThirdParty/KTX-Software/other_include",
				"$(SolutionDir)ThirdParty/KTX-Software/utils",
				"$(SolutionDir)ThirdParty/KTX-Software/lib/basisu/zstd",
			}
			filter {
				defines {
					"KHRONOS_STATIC",
					"LIBKTX"
				}
			}
			--disablewarnings { "4389", "4706", "4100", "4127", "4189", "4701", "4702", "4101", "4456" }

		project "ImGui"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files { 
				"./ThirdParty/ImGUI/**.h", 
				"./ThirdParty/ImGUI/**.cpp" 
			}
			removefiles { 
				"./ThirdParty/ImGUI/examples/**", 
				"./ThirdParty/ImGUI/misc/fonts/**",
				"./ThirdParty/ImGUI/misc/freetype/**",
				"./ThirdParty/ImGUI/backends/**"
			}
			includedirs { 
				"$(SolutionDir)ThirdParty/ImGUI"
			}

		project "spirv-cross"
			kind "StaticLib"
			language "C++"
			location "./Out/Intermediate/VCProjects"
			files {
				"./ThirdParty/SPIRV-Cross/**.h",
				"./ThirdParty/SPIRV-Cross/**.hpp",
				"./ThirdParty/SPIRV-Cross/**.cpp",
			}
			removefiles {
				"./ThirdParty/SPIRV-Cross/samples/**",
				"./ThirdParty/SPIRV-Cross/main.cpp",
				"./ThirdParty/SPIRV-Cross/tests-other/**",
				"./ThirdParty/SPIRV-Cross/spirv_cross_c.h",
				"./ThirdParty/SPIRV-Cross/spirv_cross_c.cpp",
			}
			includedirs { "$(SolutionDir)ThirdParty/SPIRV-Cross" }
			vpaths {
				["core"] = {
					"./ThirdParty/SPIRV-Cross/GLSL.std.450.h",
					"./ThirdParty/SPIRV-Cross/spirv.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cfg.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_common.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_containers.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_error_handling.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cfg.cpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross.cpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.cpp",
					"./ThirdParty/SPIRV-Cross/spirv_parser.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_parser.cpp",
				},
				["hlsl"] = {
					"./ThirdParty/SPIRV-Cross/spirv_hlsl.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_hlsl.cpp",
				},
				["glsl"] = {
					"./ThirdParty/SPIRV-Cross/spirv_glsl.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_glsl.cpp",
				},
				["reflect"] = {
					"./ThirdParty/SPIRV-Cross/spirv_reflect.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_reflect.cpp",
				},
				["util"] = {
					"./ThirdParty/SPIRV-Cross/spirv_cross_util.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_util.cpp"
				},
				["msl"] = {
					"./ThirdParty/SPIRV-Cross/spirv_msl.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_msl.cpp"
				},
				["cpp"] = {
					"./ThirdParty/SPIRV-Cross/spirv_cpp.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cpp.cpp",
				},
				["include"] = {
					"./ThirdParty/SPIRV-Cross/include/**",
					"./ThirdParty/SPIRV-Cross/cmake/**",
					"./ThirdParty/SPIRV-Cross/spirv.h"
				}
			}
			--disablewarnings { "4065", "4702", "4706", '4996' }

		project "assimp"
  			kind "SharedLib"
			location "./Out/Intermediate/VCProjects"
			targetdir "$(SolutionDir)Out"
			targetname "$(ProjectName)_$(Configuration)"
			buildoptions { "/bigobj" }
			--disablewarnings { "4819", "4189", "4131", "4996", "4127", "4244" }
			--implibname "$(SolutionDir)Out/Intermediate/$(Configuration)/$(ProjectName)"
			files {
				"./ThirdParty/assimp/**.h",
				"./ThirdParty/assimp/**.cpp",
				"./ThirdParty/assimp/**.hpp",
				"./ThirdParty/assimp/**.c",
				"./ThirdParty/assimp/**.cc",
			}
			removefiles { 
				"./ThirdParty/assimp/contrib/zlib/contrib/inflate86/**",
				"./ThirdParty/assimp/code/AssetLib/IFC/IFCReaderGen_4.h",
				"./ThirdParty/assimp/code/AssetLib/IFC/IFCReaderGen_4.cpp",
				"./ThirdParty/assimp/contrib/zlib/contrib/**",
				"./ThirdParty/assimp/test/**",
				"./ThirdParty/assimp/tools/**",
				"./ThirdParty/assimp/contrib/gtest/**",
				"./ThirdParty/assimp/build/CMakeFiles/**",
				"./ThirdParty/assimp/include/port/AndroidJNI/**",
				"./ThirdParty/assimp/port/**",
				"./ThirdParty/assimp/code/AMF/**",
				"./ThirdParty/assimp/samples/**",
				"./ThirdParty/assimp/contrib/zip/test/**",
				"./ThirdParty/assimp/contrib/draco/**"
			}
			includedirs { 
				"$(SolutionDir)ThirdParty/assimp/build/include",
				"$(SolutionDir)ThirdParty/assimp/build",
				"$(SolutionDir)ThirdParty/assimp/include",
				"$(SolutionDir)ThirdParty/assimp/code",
				"$(SolutionDir)ThirdParty/assimp",
				"$(SolutionDir)ThirdParty/assimp/contrib/zlib",
				"$(SolutionDir)ThirdParty/assimp/build/contrib/zlib",
				"$(SolutionDir)ThirdParty/assimp/contrib/rapidjson/include",
				"$(SolutionDir)ThirdParty/assimp/contrib/",
				"$(SolutionDir)ThirdParty/assimp/contrib/pugixml/src",
				"$(SolutionDir)ThirdParty/assimp/contrib/unzip",
				"$(SolutionDir)ThirdParty/assimp/contrib/irrXML",
				"$(SolutionDir)ThirdParty/assimp/contrib/openddlparser/include"
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
			"./Source/Applications/Win32Resource.h",
			"./Assets/Icon/Resource.rc",
		}
		includedirs { 
			"$(SolutionDir)",
			"$(SolutionDir)/Source",
			"$(SolutionDir)ThirdParty/cereal/include",
			"$(SolutionDir)ThirdParty/spdlog/include"
		}
		libdirs {
			"$(VK_SDK_PATH)/Lib"
		}
		targetdir "$(SolutionDir)Out"
		links { 
			"Core", 
			"Runtime", 
			"IRenderer", 
			"VulkanRenderer",
			"D3D12Renderer",
			"vulkan-1" 
		}
		vpaths {
			["Resource"] = { 
				"./Assets/Icon/Resource.rc",
				"./Source/Applications/Win32Resource.h",
			},
			[""] = { 
				"./Source/Applications/RenderTest/**",
			}
		}


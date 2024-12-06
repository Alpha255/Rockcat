#if 0
#include "Asset/ShaderImporter.h"
#include "Asset/ShaderCompiler.h"
#include "Colorful/IRenderer/IDevice.h"

static std::shared_ptr<ShaderCompileConfigs> CompileConfigs;

RHI::EShaderStage ParseStage(const char* Path)
{
	std::string Ext = File::Extension(Path, true);

	if (Ext == ".vert")
	{
		return RHI::EShaderStage::Vertex;
	}
	else if (Ext == ".hull")
	{
		return RHI::EShaderStage::Hull;
	}
	else if (Ext == ".domain")
	{
		return RHI::EShaderStage::Domain;
	}
	else if (Ext == ".geo")
	{
		return RHI::EShaderStage::Geometry;
	}
	else if (Ext == ".frag")
	{
		return RHI::EShaderStage::Fragment;
	}
	else if (Ext == ".compute")
	{
		return RHI::EShaderStage::Compute;
	}

	return RHI::EShaderStage::ShaderStageCount;
}

ShaderImporter::ShaderImporter(RHI::IDevice* RHIDevice)
	: IImporter(RHIDevice)
{
	CompileConfigs = ShaderCompileConfigs::Load("ShaderCompileConfigs.json");
}

void ShaderImporter::Reimport(std::shared_ptr<IAsset> Asset)
{
	auto Shader = std::static_pointer_cast<ShaderAsset>(Asset);
	assert(Shader);

	Shader->Object = RHI::ShaderBinaryCache::Load(Shader->Path.c_str());

	assert(File::Exists(Shader->Path.c_str()));

	auto LastWriteTime = File::LastWriteTime(Shader->Path.c_str());

	if (LastWriteTime == Shader->Object->LastWriteTime)
	{
		LOG_INFO("ShaderImporter: \"{}\", create shader module from cache.", Shader->Path.c_str());

		for (auto& Variant : Shader->Object->Variants)
		{
			Variant.second.Shader = m_Device->CreateShader(*Variant.second.Desc);
		}
	}
	else
	{
		Shader->Object->LastWriteTime = LastWriteTime;

		auto CompileShaderTask = [&](const char* SourceName, const void* Source, size_t Size, const std::vector<RHI::ShaderMacro>& Macros) {
			auto Permutation = RHI::ShaderMacro::ToPermutation(Macros);

			if (Permutation.length() > 0u)
			{
				LOG_INFO("ShaderImporter: Compile shader from file \"{}\" with permutation \"{}\".", Shader->Path.c_str(), Permutation.c_str());
			}
			else
			{
				LOG_INFO("ShaderImporter: Compile shader from file \"{}\".", Shader->Path.c_str());
			}

			auto Compiler = IShaderCompiler::Create(RHI::IRenderer::Get().Type());
			assert(Compiler);

			auto Stage = ParseStage(Shader->Path.c_str());
			assert(Stage < RHI::EShaderStage::ShaderStageCount);
			
			auto Desc = Compiler->Compile(
				SourceName,
				Source,
				Size,
				"main",
				Stage,
				Macros);

			/// Default move constructor destruct shader ptr...
			Shader->Object->Variants[Permutation] = RHI::ShaderCompiled
			{
				m_Device->CreateShader(*Desc), 
				Desc 
			};
		};

		File ShaderFile(Shader->Path);
		auto Code = ShaderFile.Read();

		auto Find = CompileConfigs->ShadersMacros.find(Shader->Path);
		if (Find == CompileConfigs->ShadersMacros.end())
		{
			CompileShaderTask(Shader->Path.c_str(), Code.get(), ShaderFile.Size(), std::vector<RHI::ShaderMacro>{});
		}
		else
		{
			for (auto& Macros : Find->second)
			{
				CompileShaderTask(Shader->Path.c_str(), Code.get(), ShaderFile.Size(), Macros);
			}
		}

		Shader->Object->Save(true);

		/// #TODO Queue for parallel compile tasks
		LOG_INFO("ShaderImporter: Compile shader \"{}\" done.", Shader->Path.c_str());
	}
}
#endif
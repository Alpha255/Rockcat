#include "Colorful/IRenderer/IShader.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Runtime/Asset/Asset.h"

NAMESPACE_START(RHI)

std::string ShaderMacro::ToPermutation(const std::vector<ShaderMacro>& Macros)
{
	std::string Permutation;
	for (auto& Macro : Macros)
	{
		Permutation += (Macro.Name + "=" + Macro.Definition);
	}

	return Permutation;
}

std::string ShaderCache::GetCachedPath(const char8_t* Path)
{
	return String::Format(
		"%s%s.%s.json",
		ASSET_PATH_SHADER_CACHE,
		Path,
		IRenderer::Get().Settings()->RendererName());
}

IShaderPtr ShaderCache::Get(const std::vector<ShaderMacro>& Macros)
{
	auto Permutation = ShaderMacro::ToPermutation(Macros);
	auto Ret = Variants.find(Permutation);
	if (Ret != Variants.end())
	{
		return Ret->second.Shader;
	}

	assert(false);
	return nullptr;
}

NAMESPACE_END(RHI)
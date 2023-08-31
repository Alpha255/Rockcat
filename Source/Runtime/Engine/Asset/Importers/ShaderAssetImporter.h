#pragma once

#include "Runtime/Engine/Asset/ShaderAsset.h"

class ShaderAssetImporter : public IAssetImporter
{
public:
	ShaderAssetImporter()
		: IAssetImporter({
			AssetType("Vertex Shader", ".vert", AssetType::EContentsType::Text), 
			AssetType("Fragment Shader", ".frag", AssetType::EContentsType::Text),
			AssetType("Geometry Shader", ".geo", AssetType::EContentsType::Text),
			AssetType("Compute Shader", ".comp", AssetType::EContentsType::Text),
			AssetType("High-level shader language", ".hlsl", AssetType::EContentsType::Text)})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<ShaderAsset>(AssetPath); }

	bool8_t Reimport(Asset& InAsset) override final
	{
		return false;
	}
private:
	class GlobalShaderCompileConfigurations : public SerializableAsset<GlobalShaderCompileConfigurations>
	{
	public:
		using BaseClass::BaseClass;

		const std::vector<std::string>& GetDefines(const std::filesystem::path& ShaderPath) const 
		{
			static std::vector<std::string> NullDefines;
			const auto It = m_ShaderDefines.find(ShaderPath.generic_string());
			return It == m_ShaderDefines.cend() ? NullDefines : (*It).second;
		}

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(m_ShaderDefines)
			);
		}
	private:
		std::map<std::string, std::vector<std::string>> m_ShaderDefines;
	};
};

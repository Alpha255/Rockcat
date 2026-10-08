#pragma once

class AssetLoader
{
public:
	AssetLoader(std::vector<std::string_view>&& SupportedFormats)
		: m_SupportedFormats(std::move(SupportedFormats))
	{
	}

	inline bool IsSupportedFormat(std::string_view Extension) const
	{
		return std::find_if(m_SupportedFormats.begin(), m_SupportedFormats.end(), [Extension](const std::string_view& Ext) {
				return _stricmp(Ext.data(), Extension.data()) == 0;
			}) != m_SupportedFormats.end();
	}

	virtual bool Load(Asset& Target) = 0;
protected:
	friend class AssetDatabase;

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) = 0;
private:
	std::vector<std::string_view> m_SupportedFormats;
};
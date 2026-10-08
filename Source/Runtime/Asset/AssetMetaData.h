#pragma once

#include "Core/Name.h"

struct AssetMetaData
{
    std::filesystem::path Path;
    std::string_view Name;
    Name TypeName;

    class Asset* GetAsset();
    class Asset* LoadAsset();
    void LoadAssetAsync();
private:
    std::shared_ptr<class Asset> AssetHandle;
};

struct IAssetType
{
    Name TypeName;
    std::string Descriptions;
    std::vector<string> Extensions;

    IAssetType(Name InTypeName, std::string_view InDescriptions, std::vector<string> InExtensions)
        : TypeName(std::move(InTypeName))
        , Descriptions(InDescriptions)
        , Extensions(std::move(InExtensions))
    {
    }

    virtual class std::shared_ptr<Asset> CreateAsset() = 0;
    virtual class std::shared_ptr<AssetImporter> CreateAssetImporter() = 0;

    bool IsValidType(std::filesystem::path Path) const
    {
        auto Extension = string(Path.extension().string()).lowercase();
        return std::find(Extensions.begin(), Extensions.end(), Extension) != Extensions.end();
    }
};

template<class TAsset, class TAssetImporter> 
struct AssetType : public IAssetType
{
    using IAssetType::IAssetType;

    virtual std::shared_ptr<Asset> CreateAsset() override
    {
        return std::make_shared<TAsset>();
    }

    virtual std::shared_ptr<AssetImporter> CreateAssetImporter() override
    {
        return std::make_shared<TAssetImporter>();
    }
};

template<class TAsset, class TAssetImporter>
struct AssetTypeRegister
{
    AssetTypeRegister(Name TypeName, std::string_view Descriptions, std::vector<string> Extensions, class AssetDatabase& Database)
    {
        Database.RegisterAssetType(std::make_shared<AssetType<TAsset, TAssetImporter>>(TypeName, Descriptions, Extensions));
    }
};

#define REGISTER_ASSET_TYPE(AssetType, AssetImporter, TypeName, Descriptions, Extensions) \
    static AssetTypeRegister<AssetType, AssetImporter> _##AssetType##_Register(TypeName, Descriptions, Extensions, AssetDatabase::Get());
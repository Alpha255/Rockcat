#pragma once

#include "Runtime/Asset/IImporter.h"
#include <ThirdParty/taskflow/taskflow/taskflow.hpp>

class AssetDatabase : public LazySingleton<AssetDatabase>
{
public:
	template<class TAsset>
	void GetOrReimport(const char8_t* AssetPath, typename TAsset::CompletionCallback Callback, bool8_t Async = false, bool8_t Force = false)
	{
		assert(AssetPath);

		const IAsset::ECategory Category = TAsset::Category();
		std::string DstPath = IAsset::Path(Category, AssetPath);

		std::unique_lock Locker(m_Assets[Category].first);

		auto& Asset = m_Assets[Category].second[DstPath];
		if (!Asset || Asset->LoadingStatus() == IAsset::ELoadingStatus::None || Force)
		{
			if (!Asset)
			{
				Asset = std::make_shared<TAsset>(DstPath);
			}

			auto Temp = std::static_pointer_cast<TAsset>(Asset);
			{
				std::unique_lock CallbacksLocker(Temp->CompletionCallbacks.first);
				Temp->CompletionCallbacks.second.push_back(Callback);
			}

			auto ReimportFunc = [this, Category, Temp, Asset]() {
				Temp->SetLoadingStatus(IAsset::ELoadingStatus::Loading);

				m_Importers[Category]->Reimport(Asset);

				Temp->SetLoadingStatus(IAsset::ELoadingStatus::Loaded);

				std::unique_lock Locker(Temp->CompletionCallbacks.first);
				for (auto& Callback : Temp->CompletionCallbacks.second)
				{
					Callback(Temp);
				}
				Temp->CompletionCallbacks.second.clear();
			};

			if (m_AsyncImporting && Async)
			{
				m_AsyncExecutor->silent_async(ReimportFunc);
			}
			else
			{
				ReimportFunc();
			}
		}
		else
		{
			auto Temp = std::static_pointer_cast<TAsset>(Asset);

			if (Temp->LoadingStatus() == IAsset::ELoadingStatus::Loading)
			{
				std::unique_lock CallbacksLocker(Temp->CompletionCallbacks.first);
				Temp->CompletionCallbacks.second.push_back(Callback);
			}
			else if (Temp->LoadingStatus() == IAsset::ELoadingStatus::Loaded)
			{
				Callback(Temp);
			}
		}
	}

	~AssetDatabase();
protected:
	ALLOW_ACCESS_LAZY(AssetDatabase);

	using AssetMap = ThreadSafeContainer<std::unordered_map<std::string, std::shared_ptr<IAsset>>>;
	using AssetMapArray = std::array<AssetMap, IAsset::ECategory::Unknown>;

	AssetDatabase();
private:
	std::shared_ptr<tf::Executor> m_AsyncExecutor;
	bool8_t m_AsyncImporting = false;

	std::array<std::shared_ptr<IImporter>, IAsset::ECategory::Unknown> m_Importers;
	AssetMapArray m_Assets;
};

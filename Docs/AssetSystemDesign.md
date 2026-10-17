# RockCat 资产系统设计

综合 Asset / AssetDatabase / Scene / Importer / GPU 资源归属五个议题的结论。

---

## 0. 三条设计原则

1. **资产是数据，运行时对象是它构建出来的东西。** 不再有"一半磁盘一半运行时"的类型。
2. **类型是"名字"，不是"枚举"。** 加一种资产类型不修改任何中心文件，且类型信息可持久化。
3. **GPU 资源的归属由"键"决定，不由"它是不是资产"决定。**

### 借鉴 UE / 刻意偏离 UE

| 议题 | UE 的做法 | 本项目 | 理由 |
|---|---|---|---|
| 类型身份 | `FTopLevelAssetPath`（路径名字符串） | **照抄**：`AssetType`（稳定字符串） | 可持久化、跨平台、无中心枚举 |
| 元数据索引 | `IAssetRegistry`（不含已加载对象） | **照抄**：`AssetRegistry` | 编辑器列 10 万资产时一个都不加载 |
| 依赖图 | 从**包头**读出，双向 | **照抄**：`ProbeHeader` + 双向图 | 建索引不需要真加载 |
| GPU 创建 | `ENQUEUE_RENDER_COMMAND` 投递 | **照抄**：`RenderResource::BeginInit` | RHI 只能在渲染线程碰 |
| 线程一致性 | 主线程 + 渲染线程**双指针镜像** | **照抄** | 避免 `shared_ptr` 跨线程竞争 |
| Editor 类型动作 | `IAssetTypeActions` 按类注册 | **照抄**：`AssetTypeActions` 按类型名注册 | 图标/缩略图不进 Core |
| 缩略图 | 独立子系统 + DDC | **照抄**：`ThumbnailCache`（磁盘缓存） | 派生数据不常驻 |
| **谁持有资产** | **GC 可达性图** | **偏离**：显式引用计数 + 句柄 | 本项目没有 GC |
| **CPU/GPU 生命周期** | **绑死**（都由 GC 决定） | **偏离**：独立 | 这是本次重构的目标本身 |
| **按资产索引的 GPU 缓存** | **不存在**（资产对象自持） | **偏离**：弱引用注册表 | 生命周期解耦的必然结果 |

---

## 1. 分层

```
Layer 0  Core      AssetType / AssetId / AssetHandle / Name            无依赖
Layer 1  Asset     AssetData / AssetRegistry / Asset / AssetDatabase   只依赖 Core
                   AssetImporter / AssetTypeRegistry
Layer 2  Runtime   SceneAsset / World                                  依赖 Asset
Layer 3  Render    RenderResource / AssetRenderResources               只依赖 Core
                   DescriptorCache<K> / FrameAllocator
Layer 4  Editor    AssetTypeActions / ThumbnailCache / AssetBrowser    依赖以上
```

**关键约束**：`Render` 层**不依赖 `Asset` 层**，只用 `AssetId` 作为键。
这样渲染侧可以独立测试，也避免 `Rendering → Asset → cereal/rapidjson` 的依赖回环。

---

## 2. 类清单

### Layer 0 — Core

| 类 | 职责 |
|---|---|
| `AssetType` | 稳定字符串类型标识。`constexpr AssetType{"Texture"}`。**不是 enum** |
| `AssetId` | 虚拟路径（项目相对）。**弱引用**，≈ `TSoftObjectPtr` |
| `AssetHandle` | 强引用句柄，持有 `shared_ptr<Asset>`。≈ `UPROPERTY` 硬引用 |

### Layer 1 — Asset

| 类 | 职责 |
|---|---|
| `AssetData` | 轻量元数据：`AssetId` / `AssetType` / 大小 / mtime / `Tags`。**不含已加载对象** |
| `AssetRegistry` | 类型索引 + 双向依赖图 + 索引缓存持久化 + 增量刷新 |
| `AssetImporter` | 格式知识。`ProbeHeader`（只读头）+ `Import`（真加载） |
| `Asset` | 纯数据基类。`virtual bool Load()` **只产 CPU 数据** |
| `AssetTypeRegistry` | 类型名 → 工厂 / 导入器 / 扩展名 |
| `AssetDatabase` | 加载缓存 + 引用计数 + 依赖编排 + **主线程投递队列** |

### Layer 2 — Runtime

| 类 | 职责 |
|---|---|
| `SceneAsset : Asset` | 场景的磁盘描述（引用的 glTF 列表、相机、组件参数） |
| `World` | 运行时容器。常驻、可重建、`Tick`、`Build(SceneAsset)` |

### Layer 3 — Render

| 类 | 键 | 职责 |
|---|---|---|
| `RenderResource` | — | 薄基类：投递 + `InitRHI`/`ReleaseRHI` + 双指针镜像 |
| `AssetRenderResources` | `AssetId` | 资产型资源。值 = **一个资源组**（一个 mesh 的全部 VB/IB） |
| `DescriptorCache<K>` | 描述符 hash | 模板。PSO / Sampler / RenderTarget 各一个实例 |
| `FrameAllocator` | 无 | 环形/线性分配，每帧重置。动态 UB、临时上传缓冲 |

### Layer 4 — Editor

| 类 | 职责 |
|---|---|
| `IAssetTypeActions` | 按类型名注册：图标、双击打开哪个编辑器、右键菜单 |
| `ThumbnailCache` | 按需生成 + 磁盘缓存 + LRU；按类型注册 renderer |
| `AssetBrowser` | UI |

---

## 3. 关键定义

### 3.1 类型标识（不是 enum）

```cpp
class AssetType
{
public:
	constexpr AssetType(const char* InName, const char* InDisplay = nullptr)
		: m_Name(InName), m_Display(InDisplay ? InDisplay : InName) {}

	constexpr std::string_view GetName() const { return m_Name; }
	constexpr std::string_view GetDisplayName() const { return m_Display; }

	friend constexpr bool operator==(AssetType A, AssetType B) { return A.m_Name == B.m_Name; }
private:
	std::string_view m_Name;
	std::string_view m_Display;
};
```

类型常量住在自己的头里：

```cpp
class TextureAsset : public Asset
{
public:
	static constexpr AssetType StaticType{ "Texture", "Texture" };
	AssetType GetType() const override final { return StaticType; }
};
```

### 3.2 弱引用 / 强引用

```cpp
class AssetId
{
public:
	AssetId() = default;
	explicit AssetId(std::filesystem::path InPath) : m_Path(std::move(InPath)) {}
	const std::filesystem::path& GetPath() const { return m_Path; }
private:
	std::filesystem::path m_Path;      // 项目相对路径
};

class AssetHandle
{
public:
	const Asset& operator*() const;
	const Asset* operator->() const;
	bool IsValid() const { return m_Asset != nullptr; }
private:
	std::shared_ptr<Asset> m_Asset;    // 强引用
};
```

### 3.3 元数据（不含已加载对象）

```cpp
struct AssetTag { Name Key; Name Value; };

struct AssetData
{
	AssetId                        Id;
	Name                           Name;
	AssetType                      Type;
	uint64_t                       FileSize = 0;
	std::time_t                    LastWriteTime = 0;
	std::vector<AssetTag>          Tags;
};
```

### 3.4 Importer：两阶段

```cpp
class AssetImporter
{
public:
	virtual ~AssetImporter() = default;

	// ① 只读头部：拿类型 + 依赖，绝不加载内容     ← 建索引/扫描用
	virtual bool ProbeHeader(const std::filesystem::path& Path,
	                         AssetType& OutType,
	                         std::vector<AssetId>& OutDependencies) = 0;

	// ② 真加载：产出资产                          ← 只在 Request 时用
	virtual std::shared_ptr<Asset> Import(const std::filesystem::path& Path) = 0;
};
```

**`Load` 是 1:1（可以做虚函数），`Import` 是 1:N（必须独立）**：

```cpp
class Asset
{
public:
	virtual ~Asset() = default;

	bool Load();                       // 由 AssetDatabase 驱动；状态机由 DB 写
	AssetType GetType() const;

protected:
	virtual bool OnLoad() = 0;         // 派生类实现：只产 CPU 数据
	friend class AssetDatabase;
};
```

1:N 的例子（glTF → SceneAsset + N 个 TextureAsset）走 `GltfImporter`，
它通过 `AssetDatabase::Request` 请求贴图，**天然去重**。

### 3.5 Registry：索引 + 双向依赖图

```cpp
using AssetIndex = uint32_t;

class AssetRegistry
{
public:
	const AssetData* Find(AssetId) const;
	std::span<const AssetIndex> FindByType(AssetType) const;
	std::span<const AssetIndex> FindByDirectory(const std::filesystem::path&) const;
	std::span<const AssetIndex> Search(std::string_view Query) const;

	std::span<const AssetIndex> GetDependencies(AssetIndex) const;
	std::span<const AssetIndex> GetReferencers(AssetIndex) const;

	void ScanAll(const std::filesystem::path& Root);
	void Refresh(std::span<const AssetId> Changed);
	void LoadCache(const std::filesystem::path&);
	void SaveCache(const std::filesystem::path&) const;
};
```

启动时只比 `mtime`，没变就复用缓存记录，不解析文件。

### 3.6 Database：状态唯一写入者 + 主线程投递

```cpp
class AssetDatabase
{
public:
	// 任意线程调用；回调排到主线程队列
	void Request(AssetId, AssetLoadedCallback = {});
	std::shared_ptr<Asset> Find(AssetId) const;

	void AddRef(AssetId);
	void Release(AssetId);

	// 主循环每帧调一次（帧首、tick 之前）
	void DrainMainThreadCallbacks();

private:
	struct FEntry
	{
		std::shared_ptr<Asset>                 Asset;
		std::shared_ptr<TFTask>                Task;
		std::vector<AssetLoadedCallback>       Pending;   // 所有等待者
		uint32_t                               RefCount = 0;
	};
	std::unordered_map<AssetId, FEntry>        m_Entries;
	std::vector<std::function<void()>>         m_MainThreadQueue;
	std::mutex                                 m_Lock;
};
```

**两个必须点**：
- `Pending` 是**回调列表**（当前实现里同路径的第二次请求回调永远不触发）
- `Request` 的 lambda 捕获 `AssetId` / `shared_ptr`，**不捕获 `&Request`**（当前有悬垂）

### 3.7 RenderResource：唯一该统一的薄层

```cpp
class RenderResource
{
public:
	virtual ~RenderResource();

	void BeginInit();                      // 主线程：投递 InitRHI
	void BeginRelease();                   // 主线程：投递 ReleaseRHI
	bool IsInitialized() const;            // 原子读

protected:
	virtual void InitRHI(class FRHICommandList&) = 0;
	virtual void ReleaseRHI() = 0;

private:
	std::atomic<bool> m_Initialized{ false };
};
```

**双指针镜像**（照抄 UE，`Texture.cpp:317-326`）：

```cpp
void UTexture::ReleaseResource()                    // 主线程
{
	check(!IsInActualRenderingThread() && !IsInRHIThread());

	FTextureResource* ToDelete = PrivateResource;
	PrivateResource = nullptr;

	ENQUEUE_RENDER_COMMAND(DeleteResource)([this, ToDelete](FRHICommandListImmediate& RHICmdList)
	{
		PrivateResourceRenderThread = nullptr;      // ← 镜像在渲染线程清
		ToDelete->ReleaseResource();
		delete ToDelete;                            // ← 释放+析构都在渲染线程
	});
}
```

**Mesh 的去重粒度是资源组，不是单个 buffer**（`StaticMesh.cpp:1529-1546`）：

```cpp
class FStaticMeshResources : public RenderResource
{
	FVertexBuffer PositionVertexBuffer;
	FVertexBuffer ColorVertexBuffer;
	FIndexBuffer  IndexBuffer;
	FIndexBuffer  ReversedIndexBuffer;
protected:
	void InitRHI(FRHICommandList&) override;   // 一次性创建全部
	void ReleaseRHI() override;                // 一次性释放全部
};
```

### 3.8 三个渲染侧容器

```cpp
// 键 = 资产
class AssetRenderResources
{
public:
	std::shared_ptr<RenderResource> Acquire(AssetId, std::shared_ptr<const Asset> Source);
private:
	std::unordered_map<AssetId, std::weak_ptr<RenderResource>> m_Resources;   // 弱引用，只去重
};

// 键 = 描述符（模板，按资源族实例化）
template<class TDesc, class TResource>
class DescriptorCache
{
public:
	std::shared_ptr<TResource> GetOrCreate(const TDesc& Desc);
	void Trim(uint32_t MaxCount);              // LRU
private:
	std::unordered_map<size_t, std::weak_ptr<TResource>> m_Cache;
};

// 无键：每帧重置
class FrameAllocator
{
public:
	std::span<std::byte> Allocate(size_t Size, size_t Align);
	void Reset();                              // 帧末
};
```

### 3.9 Runtime

```cpp
class SceneAsset : public Asset
{
public:
	static constexpr AssetType StaticType{ "Scene", "Scene" };

	std::vector<AssetId> SceneFiles;           // 引用的 glTF
	std::vector<CameraDesc> Cameras;
	std::vector<ComponentDesc> Components;

	std::vector<AssetId> GetDependencies() const override { return SceneFiles; }
};

class World final : public NoneCopyable, public ITickable, public SceneGraph
{
public:
	void Tick(float ElapsedSeconds) override final;
	void Build(const SceneAsset&);             // 清空 + 重建，只在主线程
	void Clear();
private:
	std::shared_ptr<WorldSettings> m_Settings;
};
```

---

## 4. 协同流程

### 4.1 启动

```
BaseApplication::Initialize()
  TFTask::Initialize()
  AssetRegistry::Get().LoadCache("Out/AssetRegistry.bin")
  AssetRegistry::Get().Refresh(全部已知路径)     ← 只比 mtime，变了才 ProbeHeader
  AssetDatabase::Get().Initialize()
  AssetTypeRegistry::Get().RegisterBuiltins()   ← 显式注册内建类型工厂
  m_World = std::make_shared<World>()           ← 空 World，常驻
```

### 4.2 打开场景

```
OpenScene(ScenePath)
  └─ AssetDatabase::Request(AssetId{ScenePath}, OnSceneReady)
        │
        │  工作线程
        ├─ 查 Registry 拿 AssetData → 得到 AssetType
        ├─ AssetTypeRegistry 找到 SceneAsset 工厂
        ├─ 递归 Request 依赖（SceneFiles 里的 glTF，再递归到贴图）
        │     （在 Asset::OnLoad 期间发起的 Request 自动记为它的依赖）
        ├─ Asset::Load()  → 纯 CPU 数据
        └─ 全部依赖就绪 → 把回调推进 m_MainThreadQueue
        │
        │  主线程（下一帧 DrainMainThreadCallbacks）
        └─ OnSceneReady(Asset&)
              m_World->Build(static_cast<const SceneAsset&>(Asset))
                ├─ 建 entity / component
                └─ 对每个需要 GPU 的资源：
                     AssetRenderResources::Acquire(Id, Source)
                       └─ 未命中 → 创建 RenderResource
                            └─ BeginInit() → ENQUEUE_RENDER_COMMAND
                                  [渲染线程] InitRHI()
```

### 4.3 每帧

```
BaseApplication::Tick(DeltaSeconds)
  AssetDatabase::Get().DrainMainThreadCallbacks()   ← Build 在这里发生
  m_World->Tick(DeltaSeconds)
  Renderer::Render(*m_World)
      ├─ 绘制前检查 RenderResource::IsInitialized()  ← 未就绪则跳过/画占位
      ├─ DescriptorCache<PSODesc>::GetOrCreate(...)   ← PSO 惰性创建
      ├─ DescriptorCache<RTDesc>::GetOrCreate(...)    ← 中间 RT
      └─ FrameAllocator::Allocate(...)                ← 动态 UB
  FrameAllocator::Reset()                            ← 帧末
```

### 4.4 切场景

```
OpenScene(另一个)
  ├─ AssetDatabase::Release(旧 SceneAsset 及依赖)   ← 引用计数
  │     └─ 归零 → 释放 CPU 数据
  ├─ AssetRenderResources 的 weak_ptr 自动过期      ← GPU 资源随消费者释放
  │     └─ ~RenderResource → BeginRelease() → [渲染线程] ReleaseRHI()
  └─ m_World->Build(新 SceneAsset)
```

**CPU 与 GPU 的释放可以不同步** —— 这是解耦的价值。

### 4.5 退出

```
BaseApplication::Finalize()
  m_World->Clear()
  m_World.reset()
  AssetDatabase::Get().Finalize()
  AssetRegistry::Get().SaveCache(...)
  TFTask::Finalize()
```

---

## 5. 线程模型

| 类 / 操作 | 主线程 | 工作线程 | 渲染线程 |
|---|---|---|---|
| `AssetRegistry::Scan / Refresh` | 发起 | ✅ 执行 | — |
| `AssetRegistry::Find / GetDependencies` | ✅ | ✅（只读） | — |
| `AssetImporter::ProbeHeader` | — | ✅ | — |
| `AssetImporter::Import` / `Asset::OnLoad` | — | ✅ | — |
| `AssetDatabase::Request` | ✅ | ✅ | — |
| `AssetDatabase::DrainMainThreadCallbacks` | ✅ 唯一 | — | — |
| `World::Build / Tick` | ✅ 唯一 | — | — |
| `RenderResource::BeginInit / BeginRelease` | ✅ | — | — |
| `RenderResource::InitRHI / ReleaseRHI` | — | — | ✅ 唯一 |
| `ThumbnailCache::Request` | ✅ | 生成任务投递 | 渲染 ✅ |

**跨线程传递只允许两样东西**：`shared_ptr<const Asset>`（创建后不可变）和 `AssetId`（值类型）。

---

## 6. 两个独立的状态机

```
Asset:          None → Loading → Ready / Failed
RenderResource: Uninitialized → Initializing → Initialized → Released
```

**Asset Ready ≠ GPU Ready。** UE 的印证（`StreamableRenderAsset.h:74`）：

> "Whether a stream in / out request is pending. **This also includes the resource initialization!**"

渲染器绘制前必须查 `RenderResource::IsInitialized()`。

---

## 7. 落地顺序

| 批次 | 内容 | 依赖 | 收益 |
|---|---|---|---|
| **1** | `RenderResource`（投递 + 双指针镜像） | 无 | 所有 GPU 资源的基础，可独立做 |
| **2** | `AssetType` + `AssetTypeRegistry` | 无 | 去掉枚举枷锁 |
| **3** | `AssetData` + `AssetRegistry`（内存索引 + 依赖图 + `ProbeHeader`） | 2 | 编辑器能列资产、按类型过滤 |
| **4** | 索引缓存持久化 + mtime 增量刷新 | 3 | 大规模启动变快 |
| **5** | `AssetDatabase` 回调改走主线程队列 + 引用计数 | 2,3 | 修掉悬垂与回调丢失 |
| **6** | `SceneAsset` + `World` 分离 | 5 | `Scene` 三职责拆开 |
| **7** | `AssetRenderResources` + `Asset::Load` 只产 CPU 数据 | 1,5 | CPU/GPU 解耦 |
| **8** | `DescriptorCache<K>` + `FrameAllocator` | 1 | PSO / UB 归位 |
| **9** | `ThumbnailCache` + `AssetTypeActions` | 3 | 编辑器预览 |

**1、2、3 可以立刻并行开始**，都不依赖现有代码的改造。

---

## 8. 当前代码里已知的具体缺陷

按本次讨论发现的，落地时一并修掉：

| 位置 | 问题 |
|---|---|
| `AssetDatabase` 加载任务 lambda | 捕获 `&Request` → 悬垂引用 |
| `AssetDatabase` 同路径第二次请求 | 直接 `return`，**它的 `OnLoaded` 永远不触发** |
| `AssimpSceneLoader.cpp:492` | `new Texture(Path)` 绕过数据库 → 无去重、无引用计数 |
| `Scene::~Scene()` | 析构里 `Save(true)` 写磁盘 |
| `Scene::OnPostLoad` | 手写 N 个子资产的 join 轮询，应由依赖图承担 |
| `AssetLoader` | 有虚函数无虚析构；`vector<string_view>` 存视图 |
| `Asset::Unload` / `CancelLoad` | 空壳，资产一旦加载永不释放 |
| `TextureLoader.cpp:1` | `#include "Asset/AssetLoaders/TextureLoader.h"` 路径不存在 |
| `AssimpSceneLoader.cpp:1` | 同上 |
| `Scene.cpp:3` / `BaseApplication.cpp:6` | `#include "Services/AssetDatabase.h"` 路径不存在 |
| `Scene.cpp:39` | `assert(IsMainThread() \|\| IsGameThread())`，默认配置下退化为 `IsMainThread()` |
| `Serializable<T>` | 用 `typeid(T).name()` 当 cereal key → 跨编译器不可持久化 |
| `Task.cpp` `t_ThreadTag` | 初值 `WorkerThread` → `Initialize()` 之前主线程被误判 |
| `Task.cpp` `IsWorkerThread()` | 对任意自建线程返回 true（实为"非主线程"） |
| `Task.cpp` `WaitForSeconds` | `Seconds * 1000u` 溢出（`Seconds > 1.8e16`） |

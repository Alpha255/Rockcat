#include "Applications/RenderTest/RenderTest.h"
#include "Application/ApplicationManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderGraph/RenderGraph.h"

#include "Asset/GlobalShaders.h"
#include "Services/ShaderLibrary.h"

static std::shared_ptr<GenericVS> VS;

class Serialize
{
public:
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(A)
		);
	}

	void Set(uint32_t Value) 
	{
		A = Value;
	}
private:
	uint32_t A = 32;
};

class SerializeA : public Serialize
{
public:
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Serialize)
		);
	}
};

class SerializeB : public Serialize
{
public:
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Serialize)
		);
	}
};

class SerializeTest : public Serializable<SerializeTest>
{
public:
	using BaseClass::BaseClass;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(A),
			CEREAL_NVP(B)
		);
	}

public:
	std::shared_ptr<Serialize> A = std::make_shared<SerializeA>();
	std::shared_ptr<Serialize> B = std::make_shared<SerializeB>();
};

void RenderTest::Initialize()
{
	auto Test = SerializeTest::Load("Test.json");
	Test->A->Set(123);
	Test->B->Set(456);
	Test->Save(true);

	auto test = StringUtils::vFormat("Test %d", 123);

	VS = std::make_shared<GenericVS>();
	ShaderLibrary::Get().Compile(*VS, ERHIDeviceType::Vulkan);
	ShaderLibrary::Get().Compile(*VS, ERHIDeviceType::D3D11);
	ShaderLibrary::Get().Compile(*VS, ERHIDeviceType::D3D12);

	return;
	m_Scene = Scene::Load(Paths::ScenePath() / "RenderTest.json");
	m_Scene->AddView<PlanarSceneView>();
	//m_View = std::make_shared<PlanarView>();
	//m_View->SetCamera(&m_Scene->GetMainCamera());
	//m_View->SetViewport(RHIViewport(m_Window->GetWidth(), m_Window->GetHeight()));
	//m_View->SetScissorRect(RHIScissorRect(m_Window->GetWidth(), m_Window->GetHeight()));

	//m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);
}

void RenderTest::Render()
{
	//m_RenderGraph->Execute(*m_Scene);
}

REGISTER_APPLICATION(RenderTest);

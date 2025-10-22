#include "Applications/RenderTest/RenderTest.h"
#include "Application/ApplicationManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderGraph/RenderGraph.h"

#include "Asset/GlobalShaders.h"
#include "Services/ShaderLibrary.h"

static std::shared_ptr<GenericVS> VS;

void RenderTest::Initialize()
{
	auto test = StringUtils::vFormat("Test %d", 123);

	VS = std::make_shared<GenericVS>();
	ShaderLibrary::Get().Compile(*VS, ERHIDeviceType::Vulkan);
	ShaderLibrary::Get().Compile(*VS, ERHIDeviceType::D3D11);
	ShaderLibrary::Get().Compile(*VS, ERHIDeviceType::D3D12);

	return;
	m_Scene = Scene::Load(Paths::ScenePath() / "RenderTest.scene");
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

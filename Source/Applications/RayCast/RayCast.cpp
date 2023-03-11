
#include "RayCast.h"
#include "Camera.h"
#include "D3DEngine.h"
#include "ImGui.h"
#include "System.h"
#include "ResourceFile.h"
#include <fstream>

void AppRayCast::LoadMesh()
{
	ResourceFile mesh("car.txt");
	std::ifstream meshFile(mesh.GetFilePath(), std::ios::in);
	assert(meshFile.good());

	uint32_t vertexCount = 0U, indexCount = 0U;
	std::string ignore;
	meshFile >> ignore >> vertexCount;
	meshFile >> ignore >> indexCount;
	meshFile >> ignore >> ignore >> ignore >> ignore;

	Math::XMVECTOR vMin = 
	{
		FLT_MAX, FLT_MAX, FLT_MAX
	};

	Math::XMVECTOR vMax = 
	{
		-FLT_MAX, -FLT_MAX, -FLT_MAX
	};

	m_Verties.resize(vertexCount);
	for (uint32_t i = 0U; i < vertexCount; ++i)
	{
		meshFile >> m_Verties[i].Pos.x >> m_Verties[i].Pos.y >> m_Verties[i].Pos.z;
		meshFile >> m_Verties[i].Normal.x >> m_Verties[i].Normal.y >> m_Verties[i].Normal.z;

		Math::XMVECTOR vPos = Math::XMLoadFloat3(&m_Verties[i].Pos);

		vMin = Math::XMVectorMin(vMin, vPos);
		vMax = Math::XMVectorMax(vMax, vPos);
	}

	Math::XMStoreFloat3(&m_MeshBox.Center, Math::XMVectorAdd(vMax, vMin));
	m_MeshBox.Center *= 0.5f;
	Math::XMStoreFloat3(&m_MeshBox.Extents, Math::XMVectorSubtract(vMax, vMin));
	m_MeshBox.Extents *= 0.5f;

	meshFile >> ignore;
	meshFile >> ignore;
	meshFile >> ignore;

	m_Indices.resize(indexCount * 3U);
	for (uint32_t i = 0U; i < indexCount; ++i)
	{
		meshFile >> m_Indices[i * 3 + 0] >> m_Indices[i * 3 + 1] >> m_Indices[i * 3 + 2];
	}

	meshFile.close();

	m_VertexBuffer.CreateAsVertexBuffer(sizeof(Vertex) * vertexCount, D3DBuffer::eGpuReadOnly, &m_Verties[0]);
	m_IndexBuffer.CreateAsIndexBuffer(sizeof(uint32_t) * indexCount * 3, D3DBuffer::eGpuReadOnly, &m_Indices[0]);
}

void AppRayCast::Initialize()
{
	LoadMesh();
	m_VertexShader.Create("RayCast.hlsl", "VSMain");
	m_PixelShader.Create("RayCast.hlsl", "PSMain");

	m_ConstantBuffer.CreateAsConstantBuffer(sizeof(ConstantsBuffer), D3DBuffer::eGpuReadCpuWrite);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_VertexLayout.Create(m_VertexShader.GetBlob(), layout, _countof(layout));
}

void AppRayCast::RenderScene()
{
	static ConstantsBuffer CBuffer;

    D3DEngine::Instance().ResetDefaultRenderSurfaces();
    D3DEngine::Instance().SetViewport(D3DViewport(0.0f, 0.0f, (float)m_Width, (float)m_Height));

	D3DEngine::Instance().SetVertexShader(m_VertexShader);
	D3DEngine::Instance().SetPixelShader(m_PixelShader);

	D3DEngine::Instance().SetVertexBuffer(m_VertexBuffer, sizeof(Vertex), 0U, 0U);
	D3DEngine::Instance().SetIndexBuffer(m_IndexBuffer, eR32_UInt, 0U);
	D3DEngine::Instance().SetInputLayout(m_VertexLayout);

	D3DEngine::Instance().SetRasterizerState(D3DStaticState::Wireframe);

	CBuffer.WVP = Matrix::Transpose(m_Camera->GetWVPMatrix());
	CBuffer.Color = Color::White;
	m_ConstantBuffer.Update(&CBuffer, sizeof(ConstantsBuffer));
	D3DEngine::Instance().SetConstantBuffer(m_ConstantBuffer, 0U, D3DShader::eVertexShader);
	D3DEngine::Instance().SetConstantBuffer(m_ConstantBuffer, 0U, D3DShader::ePixelShader);

	D3DEngine::Instance().DrawIndexed((uint32_t)m_Indices.size(), 0U, 0, eTriangleList);

	if (m_PickedTriangleIndex != UINT_MAX)
	{
		CBuffer.Color = Color::Red;
		m_ConstantBuffer.Update(&CBuffer, sizeof(ConstantsBuffer));

		D3DEngine::Instance().SetRasterizerState(D3DStaticState::Solid);

		/// Change depth test from < to <= so that if we draw the same triangle twice, it will still pass
		/// the depth test.  This is because we redraw the picked triangle with a different material
		/// to highlight it.

		D3DEngine::Instance().DrawIndexed(3U, 3U * m_PickedTriangleIndex, 0, eTriangleList);
	}
}

void AppRayCast::Pick(int32_t x, int32_t y)
{
	/// Compute picking ray in view space.
	Matrix proj = m_Camera->GetProjMatrix();
	
	float vx = (2.0f * x / m_Width - 1.0f) / proj.m[0][0];
	float vy = (-2.0f * y / m_Height + 1.0f) / proj.m[1][1];

	/// Ray definition in view space.
	Math::XMVECTOR vRayOrigin = 
	{
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Math::XMVECTOR vRayDir = 
	{
		vx, vy, 1.0f, 0.0f 
	};

	/// Tranform ray to local space of Mesh.
	Matrix view = m_Camera->GetViewMatrix();
	view.Inverse();

	Matrix world = m_Camera->GetWorldMatrix();
	world.Inverse();

	Matrix toLocal = view * world;

	vRayOrigin = Math::XMVector3TransformCoord(vRayOrigin, *(Math::XMMATRIX *)&toLocal);
	vRayDir = Math::XMVector3TransformNormal(vRayDir, *(Math::XMMATRIX *)&toLocal);

	/// Make the ray direction unit length for the intersection tests.
	vRayDir = Math::XMVector3Normalize(vRayDir);

	/// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	/// so do the ray/triangle tests.
	/// If we did not hit the bounding box, then it is impossible that we hit 
	/// the Mesh, so do not waste effort doing ray/triangle tests.

	/// Assume we have not picked anything yet, so init to -1.
	m_PickedTriangleIndex = UINT_MAX;
	float tmin0 = 0.0f;
	if (!IsIntersectRayAxisAlignedBox(vRayOrigin, vRayDir, tmin0))
	{
		return;
	}

	/// Find the nearest ray/triangle intersection.
	tmin0 = FLT_MAX;
	for (uint32_t i = 0U; i < m_Indices.size() / 3U; ++i)
	{
		/// We have to iterate over all the triangles in order to find the nearest intersection.
		Vec3 v0 = m_Verties[m_Indices[i * 3 + 0]].Pos;
		Vec3 v1 = m_Verties[m_Indices[i * 3 + 1]].Pos;
		Vec3 v2 = m_Verties[m_Indices[i * 3 + 2]].Pos;

		float tmin1 = FLT_MAX;
		if (IsIntersectRayTriangle(vRayOrigin, vRayDir, v0, v1, v2, tmin1))
		{
			if (tmin1 < tmin0)
			{
				/// This is the new nearest picked triangle.
				tmin0 = tmin1;
				m_PickedTriangleIndex = i;
			}
		}
	}
}

void AppRayCast::MouseButtonDown(::WPARAM wParam, int32_t x, int32_t y)
{
	Base::MouseButtonDown(wParam, x, y);

	if (wParam & MK_RBUTTON)
	{
		Pick(x, y);
	}
}

bool AppRayCast::IsIntersectRayAxisAlignedBox(const Math::XMVECTOR &vOrigin, const Math::XMVECTOR &vDir, float &min)
{
	Math::XMVECTOR vEpsilon
	{
		1e-20f, 1e-20f, 1e-20f, 1e-20f
	};
	Math::XMVECTOR vFloatMin =
	{
		-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX
	};
	Math::XMVECTOR vFloatMax =
	{
		FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX
	};

	Math::XMVECTOR vCenter = Math::XMLoadFloat3(&m_MeshBox.Center);
	Math::XMVECTOR vExtents = Math::XMLoadFloat3(&m_MeshBox.Extents);

	/// Adjust ray origin to be relative to center of the box.
	Math::XMVECTOR vAxisOrigin = Math::XMVectorSubtract(vCenter, vOrigin);

	/// Compute the dot product againt each axis of the box.
	/// Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
	/// if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
	Math::XMVECTOR vParallel = Math::XMVectorLessOrEqual(Math::XMVectorAbs(vDir), vEpsilon);

	/// Test against all three axii simultaneously.
	Math::XMVECTOR inverseAxisDotDir = Math::XMVectorReciprocal(vDir);
	Math::XMVECTOR t1 = Math::XMVectorMultiply(Math::XMVectorSubtract(vAxisOrigin, vExtents), inverseAxisDotDir);
	Math::XMVECTOR t2 = Math::XMVectorMultiply(Math::XMVectorAdd(vAxisOrigin, vExtents), inverseAxisDotDir);

	/// Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
	/// use the results from any directions parallel to the slab.
	Math::XMVECTOR t_min = Math::XMVectorSelect(Math::XMVectorMin(t1, t2), vFloatMin, vParallel);
	Math::XMVECTOR t_max = Math::XMVectorSelect(Math::XMVectorMax(t1, t2), vFloatMax, vParallel);

	t_min = Math::XMVectorMax(t_min, Math::XMVectorSplatY(t_min));
	t_min = Math::XMVectorMax(t_min, Math::XMVectorSplatZ(t_min));
	t_max = Math::XMVectorMin(t_max, Math::XMVectorSplatY(t_max));
	t_max = Math::XMVectorMin(t_max, Math::XMVectorSplatZ(t_max));

	/// if ( t_min > t_max ) return FALSE;
	Math::XMVECTOR vNoIntersection = Math::XMVectorGreater(Math::XMVectorSplatX(t_min), Math::XMVectorSplatX(t_max));

	/// if ( t_max < 0.0f ) return FALSE;
	vNoIntersection = Math::XMVectorOrInt(vNoIntersection, Math::XMVectorLess(Math::XMVectorSplatX(t_max), Math::XMVectorZero()));

	/// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
	Math::XMVECTOR vParallelOverlap = Math::XMVectorInBounds(vAxisOrigin, vExtents);
	vNoIntersection = Math::XMVectorOrInt(vNoIntersection, Math::XMVectorAndCInt(vParallel, vParallelOverlap));

	if (!Math::Internal::XMVector3AnyTrue(vNoIntersection))
	{
		/// Store the x-component to *pDist
		Math::XMStoreFloat(&min, t_min);
		return true;
	}

	return false;
}

bool AppRayCast::IsIntersectRayTriangle(const Math::XMVECTOR &vOrigin, const Math::XMVECTOR &vDir, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, float &min)
{
	Math::XMVECTOR Epsilon =
	{
		1e-20f, 1e-20f, 1e-20f, 1e-20f
	};

	Math::XMVECTOR negEpsilon = 
	{
		-1e-20f, -1e-20f, -1e-20f, -1e-20f
	};

	Math::XMVECTOR Zero = Math::XMVectorZero();
	Math::XMVECTOR V0 = Math::XMLoadFloat3(&v0);
	Math::XMVECTOR V1 = Math::XMLoadFloat3(&v1);
	Math::XMVECTOR V2 = Math::XMLoadFloat3(&v2);

	Math::XMVECTOR e1 = Math::XMVectorSubtract(V1, V0);
	Math::XMVECTOR e2 = Math::XMVectorSubtract(V2, V0);

	/// p = Direction ^ e2;
	Math::XMVECTOR p = Math::XMVector3Cross(vDir, e2);

	/// det = e1 * p;
	Math::XMVECTOR det = Math::XMVector3Dot(e1, p);

	Math::XMVECTOR u, v, t;

	if (Math::XMVector3GreaterOrEqual(det, Epsilon))
	{
		/// Determinate is positive (front side of the triangle).
		Math::XMVECTOR s = Math::XMVectorSubtract(vOrigin, V0);

		/// u = s * p;
		u = Math::XMVector3Dot(s, p);

		Math::XMVECTOR NoIntersection = Math::XMVectorLess(u, Zero);
		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorGreater(u, det));

		/// q = s ^ e1;
		Math::XMVECTOR q = Math::XMVector3Cross(s, e1);

		/// v = Direction * q;
		v = Math::XMVector3Dot(vDir, q);

		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorLess(v, Zero));
		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorGreater(Math::XMVectorAdd(u, v), det));

		/// t = e2 * q;
		t = Math::XMVector3Dot(e2, q);

		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorLess(t, Zero));

		if (Math::XMVector4EqualInt(NoIntersection, Math::XMVectorTrueInt()))
		{
			return false;
		}
	}
	else if (Math::XMVector3LessOrEqual(det, negEpsilon))
	{
		/// Determinate is negative (back side of the triangle).
		Math::XMVECTOR s = Math::XMVectorSubtract(vOrigin, V0);

		/// u = s * p;
		u = Math::XMVector3Dot(s, p);

		Math::XMVECTOR NoIntersection = Math::XMVectorGreater(u, Zero);
		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorLess(u, det));

		/// q = s ^ e1;
		Math::XMVECTOR q = Math::XMVector3Cross(s, e1);

		/// v = Direction * q;
		v = Math::XMVector3Dot(vDir, q);

		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorGreater(v, Zero));
		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorLess(Math::XMVectorAdd(u, v), det));

		/// t = e2 * q;
		t = Math::XMVector3Dot(e2, q);

		NoIntersection = Math::XMVectorOrInt(NoIntersection, Math::XMVectorGreater(t, Zero));

		if (Math::XMVector4EqualInt(NoIntersection, Math::XMVectorTrueInt()))
		{
			return false;
		}
	}
	else
	{
		/// Parallel ray.
		return false;
	}

	Math::XMVECTOR inv_det = Math::XMVectorReciprocal(det);

	t = Math::XMVectorMultiply(t, inv_det);

	/// u * inv_det and v * inv_det are the barycentric cooridinates of the intersection.

	/// Store the x-component to *pDist
	Math::XMStoreFloat(&min, t);

	return true;
}

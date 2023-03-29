#include "pch.h"
#include "CWaveRenderer.h"

#include "CDevice.h"
#include "CTimeMgr.h"

#include "CTransform.h"

CWaveRenderer::CWaveRenderer()
	: CRenderComponent(COMPONENT_TYPE::WAVERENDERER)
{
}

CWaveRenderer::~CWaveRenderer()
{
}

void CWaveRenderer::CreateMesh()
{
	m_Wave.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);

	UINT BYTEWIDTH = m_Wave.VertexCount();

	m_iZFaceCount = m_Wave.RowCount();
	m_iXFaceCount = m_Wave.ColumnCount();

	CMesh* pMesh = GetMesh().Get();

	// 메쉬 만들기
	// 기존에 참조하던 메쉬는 삭제
	if (nullptr != pMesh)
	{
		DeleteRes(pMesh, RES_TYPE::MESH);
	}

	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	vecIdx.resize(3 * m_Wave.TriangleCount()); // 3 indices per face
	// 인덱스
	UINT m = m_Wave.RowCount();
	UINT n = m_Wave.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			vecIdx[k] = i * n + j;
			vecIdx[k + 1] = i * n + j + 1;
			vecIdx[k + 2] = (i + 1) * n + j;

			vecIdx[k + 3] = (i + 1) * n + j;
			vecIdx[k + 4] = i * n + j + 1;
			vecIdx[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	pMesh = new CMesh;
	pMesh->Create(nullptr, BYTEWIDTH, vecIdx.data(), (UINT)vecIdx.size(), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	pMesh->SetKey(L"WaveMesh");

	tEvent evn = {};

	// 추가
	AddRes(pMesh, RES_TYPE::MESH);
	SetMesh(pMesh);

	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ColorMtrl"), 0);
}

void CWaveRenderer::tick() 
{
	float fTotalTime = g_global.fAccTime;

	static float t_base = 0.0f;
	if ((fTotalTime - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(1.0f, 2.0f); // define the range
		float r = distr(gen);

		m_Wave.Disturb(i, j, r);
	}

	float fDeltaTime = CTimeMgr::GetInst()->GetDeltaTime();
	m_Wave.Update(fDeltaTime);

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;

	ComPtr<ID3D11Buffer> buffer = GetMesh()->GetBuffer();

	CONTEXT->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	Vtx* v = reinterpret_cast<Vtx*>(mappedData.pData);

	for (UINT i = 0; i < m_Wave.VertexCount(); ++i)
	{
		v[i].vPos = m_Wave[i];
		v[i].vColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	CONTEXT->Unmap(buffer.Get(), 0);
}

void CWaveRenderer::finaltick()
{
	// MainCam 위치를 알아낸다.
	// SkyBox 의 위치값을 갱신

	// MainCam 의 Far 값을 가져온다
	// Transform Scale 로 적용
}

void CWaveRenderer::render()
{
	Transform()->UpdateData();

	GetCurMaterial(0)->UpdateData();

	GetMesh()->render_particle(0);

	CMaterial::Clear();
}


void CWaveRenderer::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);
}

void CWaveRenderer::LoadFromFile(FILE* _File)
{

}

void CWaveRenderer::Initialize()
{
	CreateMesh();
}
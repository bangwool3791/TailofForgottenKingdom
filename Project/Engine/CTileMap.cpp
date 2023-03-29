#include "pch.h"
#include "CTileMap.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CResMgr.h"
#include "CJpsMgr.h"

#include "CTransform.h"
#include "CCamera.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "Base.h"
#include "CDevice.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_iAlive{}
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"TileMesh"));
	
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UiTileMtrl"), 0);
	m_TileBuffer = new CStructuredBuffer;

	m_AtlasTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\Mask\\TileArea.png");
}

CTileMap::~CTileMap()
{
	if (nullptr != m_TileBuffer)
		delete m_TileBuffer;
}

void CTileMap::SetTileCount(UINT _iWidth, UINT _iHeight)
{
	m_vTileCount = Vec2((float)_iWidth, (float)_iHeight);

	m_vecTile.clear();
	m_vecTile.resize(_iWidth * _iHeight);

	if (m_TileBuffer->GetElementCount() < _iWidth * _iHeight)
	{
		m_TileBuffer->Create(sizeof(tTile), _iWidth * _iHeight, SB_TYPE::SRV_ONLY, nullptr, true);
	}

	//// Ÿ�� ������ ���� ����
	//float fWidth = (float)m_AtlasTex->GetWidth();
	//float fHeight = (float)m_AtlasTex->GetHeight();

	//Vec2 vLeftTop = Vec2(64.f, 64.f);
	//Vec2 vSlice = Vec2(64.f, 64.f);

	//vLeftTop /= Vec2(fWidth, fHeight);
	//vSlice /= Vec2(fWidth, fHeight);

	//for (size_t i = 0; i < m_vecTile.size(); ++i)
	//{
	//	m_vecTile[i].vLeftTop = vLeftTop;
	//	m_vecTile[i].vSlice = vSlice;
	//}

	//m_vecTile[0].vLeftTop = Vec2(0.f, 0.f);

	//m_TileBuffer->SetData(m_vecTile.data(), (UINT)(m_vTileCount.x * m_vTileCount.y));
}

void CTileMap::begin()
{
	tTile tTile{};
	for (int i = 0; i < TILEX; ++i)
	{
		for (int j = 0; j < TILEZ; ++j)
		{
			float	fX = (TILECX * j) + ((i % 2) * (TILECX * 0.5f));
			float	fZ = (TILECZ * 0.5f) * i;

			tTile.iInfo = 0;
			tTile.vPos = Vec3{ fX, 0.f, fZ };
			tTile.iIndex = i * TILEZ + j;

			m_vecTile.push_back(tTile);
		}
	}

	m_TileBuffer->Create(sizeof(tTile), TILEX * TILEZ, SB_TYPE::SRV_ONLY, m_vecTile.data(), true);

	m_TileBuffer->SetData(m_vecTile.data(), (UINT)m_vecTile.size());

	CJpsMgr::GetInst()->Initialize(GetMesh());
}

void CTileMap::finaltick()
{
}

void CTileMap::render()
{
	Transform()->UpdateData();

	m_TileBuffer->UpdateData(56, PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);

	float fWidth = (float)m_AtlasTex->GetWidth();
	float fHeight = (float)m_AtlasTex->GetHeight();

	Vec2 vLeftTop = Vec2(64.f, 64.f);
	Vec2 vSlice = Vec2(64.f, 64.f);

	GetCurMaterial(0)->SetScalarParam(VEC2_0, &vLeftTop);
	GetCurMaterial(0)->SetScalarParam(VEC2_1, &vSlice);
	GetCurMaterial(0)->SetScalarParam(VEC2_2, &m_vTileCount);

	GetCurMaterial(0)->SetTexParam(TEX_0, m_AtlasTex);

	GetCurMaterial(0)->UpdateData();

	GetMesh()->render(0);

	CMaterial::Clear();
}

tTile CTileMap::GetInfo(int _iIndex)
{
	if (0 <= _iIndex && _iIndex < m_vecTile.size())
	{
		return m_vecTile[_iIndex];
	}
	return tTile{};
}

tTile CTileMap::GetInfo(Vec3 _vPos)
{
	//cout << "GetInfo" << endl;
	UINT index = 0;
	//int i = _vPos.z / (TILECZ * 0.5f);

	for (int i = 0; i < TILEZ; ++i)
	{
		int left = i * TILEX;
		int right = left + TILEX - 1;

		index = Find(_vPos, left, right);

		if (-1 != index)
			return m_vecTile[index];
	}
	return m_vecTile[0];
}

void CTileMap::SetInfo(Vec3 _vPos, UINT _iInfo)
{
	//cout << "SetInfo" << endl;
	UINT index = 0;

	//int i = _vPos.z / (TILECZ * 0.5f);

	for (int i = 0; i < TILEZ; ++i)
	{
		int left = i * TILEX;
		int right = left + TILEX - 1;

		index = Find(_vPos, left, right);

		if (-1 != index)
		{
			m_vecTile[index].iInfo = _iInfo;
			m_TileBuffer->SetData(m_vecTile.data(), (UINT)m_vecTile.size());
			return;
		}
	}
}

void CTileMap::SetInfo(int _iIndex, UINT _iInfo)
{
	if (0 <= _iIndex && _iIndex < m_vecTile.size())
	{
		m_vecTile[_iIndex].iInfo = _iInfo;
		m_TileBuffer->SetData(m_vecTile.data(), (UINT)m_vecTile.size());
	}
}

int CTileMap::Find(Vec3 vPos, int left, int right)
{
	if (left > right)
		return -1;

	UINT mid = (left + right) / 2;

	//cout << mid << endl;

	if (Picking(vPos, mid))
		return mid;

	if (vPos.x < m_vecTile[mid].vPos.x)
		return Find(vPos, left, mid -1);
	else
		return Find(vPos, mid + 1, right);
}


bool CTileMap::Picking(Vec3 vPos, UINT& iIndex)
{
	Vec3	vPoint[4] = {

		Vec3(m_vecTile[iIndex].vPos.x,					 m_vecTile[iIndex].vPos.y, m_vecTile[iIndex].vPos.z + (TILECZ * 0.5f)),	// 12
		Vec3(m_vecTile[iIndex].vPos.x + (TILECX * 0.5f), m_vecTile[iIndex].vPos.y, m_vecTile[iIndex].vPos.z),					// 3
		Vec3(m_vecTile[iIndex].vPos.x,					 m_vecTile[iIndex].vPos.y, m_vecTile[iIndex].vPos.z - (TILECZ * 0.5f)),	// 6
		Vec3(m_vecTile[iIndex].vPos.x - (TILECX * 0.5f), m_vecTile[iIndex].vPos.y, m_vecTile[iIndex].vPos.z),					// 9

	};

	Vec3		vDir[4] = {

		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]

	};

	Vec3		vNormal[4] = {

		Vec3(-vDir[0].z, vDir[0].y, vDir[0].x),
		Vec3(-vDir[1].z, vDir[1].y, vDir[1].x),
		Vec3(-vDir[2].z, vDir[2].y, vDir[2].x),
		Vec3(-vDir[3].z, vDir[3].y, vDir[3].x)
	};

	for (int i = 0; i < 4; ++i)
		vNormal[i].Normalize();

	Vec3	vMouseDir[4] = {

		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]
	};

	for (int i = 0; i < 4; ++i)
	{
		if (0.f < vNormal[i].Dot(vMouseDir[i]))
			return false;
	}

	return true;
}

void CTileMap::SaveToFile(FILE* _File)
{
	CRenderComponent::SaveToFile(_File);

	size_t size = m_vecTile.size();

	fwrite(&size, sizeof(size_t), 1, _File);

	for (size_t i{}; i < size; ++i)
	{
		fwrite(&m_vecTile[i], sizeof(tTile), 1, _File);
	}
}

void CTileMap::LoadFromFile(FILE* _File)
{
	CRenderComponent::LoadFromFile(_File);

	size_t size = 0;

	fread(&size, sizeof(size_t), 1, _File);

	m_vecTile.resize(size);

	for (size_t i{}; i < size; ++i)
	{
		fread(&m_vecTile[i], sizeof(tTile), 1, _File);
	}
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();
}

void CTileMap::On()
{
	m_iAlive = 1;
}

void CTileMap::Off()
{
	m_iAlive = 0;
}
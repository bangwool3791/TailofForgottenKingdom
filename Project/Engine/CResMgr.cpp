#include "pch.h"
#include "CResMgr.h"
#include "CSound.h"

CResMgr::CResMgr()
	:m_vecLayoutInfo{}
	, m_iLayoutOffset{0}
{
	for (UINT i{ 0 }; i < (UINT)RES_TYPE::END; ++i)
	{
		m_arrRes[i] = {};
	}
}

CResMgr::~CResMgr()
{
	for (UINT i{ 0 }; i < (UINT)RES_TYPE::END; ++i)
	{
		Safe_Del_Map(m_arrRes[i]);
	}
	//CSound::g_pFMOD->release();
}

bool CResMgr::DeleteRes(RES_TYPE _Type, const wstring& _strKey)
{
	map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)_Type].find(_strKey);

	if (m_arrRes[(UINT)_Type].end() != iter)
	{
		m_arrRes[(UINT)_Type].erase(iter);
		m_bChanged = true;
		return true;
	}

	return false;
}

void CResMgr::tick()
{
	m_bChanged = false;
}


Vec3 CResMgr::GetCircleVector(size_t i, size_t tessellation) 
{
    const float angle = float(i) * XM_2PI / float(tessellation);
    float       sin;
    float       cos;

    XMScalarSinCos(&sin, &cos, angle);
    return { cos, 0, sin };
}


void CResMgr::CreateConeMesh(float radius, float height, size_t tessellation) noexcept
{
    if (tessellation < 3)
    {
        throw std::invalid_argument("tessellation parameter must be at least 3");
    }

    vector<Vtx>  vertices;
    vector<UINT> indices;

    vertices.reserve(tessellation + 1); // 1 + x
    indices.reserve(tessellation * 6 - 6);  // 3x + 3(x-2)

    const Vec3   topPosition = Vec3::Zero;
    vertices.push_back(Vtx{ topPosition, {}, {}, {}, {}, {} });

    for (size_t i = 0; i < tessellation; i++)
    {
        const Vec3 circleVec = GetCircleVector(i, tessellation);
        const Vec3 position = circleVec * radius + Vec3(0, -height, 0);
        vertices.push_back(Vtx{ position, {}, {}, {}, {}, {} });

        indices.push_back(0);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
    }

    indices[indices.size() - 2] = 1;

    for (size_t i = 0; i < tessellation - 2; i++)
    {
        indices.push_back(1);
        indices.push_back(1 + i + 1);
        indices.push_back(1 + i + 2);
    }


    // 추가
    CMesh* pMesh = new CMesh(true);
    pMesh->Create(vertices.data(), vertices.size(), indices.data(), indices.size());
    AddRes<CMesh>(L"ConeMesh", pMesh);
}

/*
* 일반함수는 헤더가 아닌 cpp 정의.
* LNK 2005 에러 발생
*/
void CResMgr::AddRes(const wstring& _strKey, RES_TYPE _type, CRes* _pRes)
{
    Ptr<CRes> pRes = FindRes(_strKey, _type);

    if (nullptr != pRes)
    {
        int a = 0;
    }
    assert(!pRes.Get());

    _pRes->SetKey(_strKey);
    m_arrRes[(UINT)_type].insert(make_pair(_strKey, _pRes));

    m_bChanged = true;
}

Ptr<CRes> CResMgr::FindRes(const wstring& _strKey, RES_TYPE _type)
{
    map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)_type].find(_strKey);

    if (iter == m_arrRes[(UINT)_type].end())
    {
        return nullptr;
    }

    return iter->second;
}


Ptr<CMeshData> CResMgr::LoadFBX(const wstring& _strPath)
{
    wstring strFileName = path(_strPath).stem();

    wstring strName = L"meshdata\\";
    strName += strFileName + L".mdat";

    Ptr<CMeshData> pMeshData = FindRes<CMeshData>(strName);

    if (nullptr != pMeshData)
        return pMeshData;

    pMeshData = CMeshData::LoadFromFBX(_strPath);
    pMeshData->SetKey(strName);
    pMeshData->SetRelativePath(strName);
    pMeshData->m_bEngineRes = true;
    m_arrRes[(UINT)RES_TYPE::MESHDATA].insert(make_pair(strName, pMeshData.Get()));

    // meshdata 를 실제파일로 저장
    pMeshData->Save(strName);

    return pMeshData;
}
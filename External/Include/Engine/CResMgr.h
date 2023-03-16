#pragma once

#include "CPathMgr.h"

#include "CMesh.h"
#include "CTexture.h"
#include "CSound.h"
#include "CGraphicsShader.h"
#include "CMaterial.h"
#include "CComputeShader.h"
#include "CPrefab.h"

class CResMgr
	:public CSingleton<CResMgr>
{
private:
	map<wstring, Ptr<CRes>>				m_arrRes[(UINT)RES_TYPE::END];
	vector<D3D11_INPUT_ELEMENT_DESC>	m_vecLayoutInfo;
	UINT								m_iLayoutOffset;
	bool								m_bChanged;
public:
	template<typename T>
	inline void AddRes(const wstring& _strKey, T* _pRes);
	void AddRes(const wstring& _strKey, RES_TYPE _type, CRes* _pRes);

	bool DeleteRes(RES_TYPE _Type, const wstring& _strKey);
	void SaveRes(RES_TYPE _Type, const wstring& _strKey);

	template<typename T>
	inline Ptr<T> Load(const wstring& _strRelativePath);
	template<typename T>
	Ptr<T> Load(const wstring& _strKey, const wstring& _strRelativePath);

	template<typename T>
	Ptr<T> FindRes(const wstring& _strKey);
	Ptr<CRes> FindRes(const wstring& _strKey, RES_TYPE _type);

	template<typename T>
	wstring GetNewResName();
public:
	void init();
	void initSound();
	void tick();
	bool IsChanged() { return m_bChanged; }

	const vector<D3D11_INPUT_ELEMENT_DESC>& GetInputLayoutInfo() { return m_vecLayoutInfo; }

	Ptr<CTexture> CreateTexture(const wstring& _strKey
		, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat, UINT _iBindFlag);
	Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D);
	Ptr<CTexture> CreateCubeTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat, UINT _iBindFlag);
	const map<wstring, Ptr<CRes>>& GetResource(RES_TYPE _eResType)
	{
		return m_arrRes[(UINT)_eResType];
	}

	map<wstring, Ptr<CRes>>& GetResourceRef(RES_TYPE _eResType)
	{
		return m_arrRes[(UINT)_eResType];
	}

private:
	void CreateDefaultMesh();
	void CreateDefaultSound();
	void CreateDefaultTexture();
	void CreateDefaultGraphicsShader();
	void CreateDefaultMaterial();
	void CreateDefaultComputeShader();
	void AddInputLayout(DXGI_FORMAT _eFormat, const char* _strSemanticName);
	Vec3 GetCircleVector(size_t i, size_t tessellation);
	void CreateConeMesh(float radius, float height, size_t tessellation) noexcept;
private:
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertcies, vector<UINT>& Indices);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertcies, vector<UINT>& Indices);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,UINT sliceCount, UINT stackCount, vector<Vtx>& Vertcies, vector<UINT>& Indices);
private:
	CResMgr();
	virtual ~CResMgr();
	friend class CSingleton<CResMgr>;


};

template<typename T>
RES_TYPE GetType()
{
	if (typeid(T).hash_code() == typeid(CMesh).hash_code())
		return RES_TYPE::MESH;

	if (typeid(T).hash_code() == typeid(CGraphicsShader).hash_code())
		return RES_TYPE::GRAPHICS_SHADER;

	if (typeid(T).hash_code() == typeid(CMaterial).hash_code())
		return RES_TYPE::MATERIAL;

	if (typeid(T).hash_code() == typeid(CComputeShader).hash_code())
		return RES_TYPE::COMPUTE_SHADER;

	if (typeid(T).hash_code() == typeid(CTexture).hash_code())
		return RES_TYPE::TEXTURE;

	if (typeid(T).hash_code() == typeid(CPrefab).hash_code())
		return RES_TYPE::PREFAB;

	if (typeid(T).hash_code() == typeid(CSound).hash_code())
		return RES_TYPE::SOUND;
	else
		return RES_TYPE::END;
}

template<typename T>
inline void CResMgr::AddRes(const wstring& _strKey, T* _pRes)
{
	RES_TYPE eResType = ::GetType<T>();

	Ptr<T> pRes = FindRes<T>(_strKey);

	_pRes->SetKey(_strKey);

	m_bChanged = true;

	if (nullptr != pRes)
	{
		auto iter = m_arrRes[(UINT)eResType].find(_strKey);
		iter->second = nullptr;
		m_arrRes[(UINT)eResType].erase(_strKey);
	}

	m_arrRes[(UINT)eResType].insert(make_pair(_strKey, _pRes));
}

template<typename T>
inline Ptr<T> CResMgr::FindRes(const wstring& _strKey)
{
	auto iter = m_arrRes[(UINT)::GetType<T>()].find(_strKey);

	if (iter == m_arrRes[(UINT)::GetType<T>()].end())
		return nullptr;

	return (T*)iter->second.Get();
}

template<typename T>
inline Ptr<T> CResMgr::Load(const wstring& _strRelativePath)
{
	//1. 타입을 읽어온다.
	//2. 경로를 완성한다.
	//3. 리소스 객체를 완성한다.
	RES_TYPE eResType = GetType<T>();

	CRes* pResource = FindRes<T>(_strRelativePath).Get();

	if (nullptr != pResource)
		return (T*)pResource;

	pResource = new T;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	if (FAILED(pResource->Load(strFilePath)))
	{
		MessageBox(nullptr, strFilePath.c_str(), L"리소스 로딩 실패", MB_OK);
		delete pResource;
		return nullptr;
	}
	pResource->SetRelativePath(_strRelativePath);
	AddRes<T>(_strRelativePath, (T*)pResource);
	return (T*)pResource;
}

template<typename T>
inline Ptr<T> CResMgr::Load(const wstring& _strKey, const wstring& _strRelativePath)
{
	//1. 타입을 읽어온다.
	//2. 경로를 완성한다.
	//3. 리소스 객체를 완성한다.
	RES_TYPE eResType = GetType<T>();

	CRes* pResource = FindRes<T>(_strKey).Get();

	if (nullptr != pResource)
		return (T*)pResource;

	pResource = new T;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	if (FAILED(pResource->Load(strFilePath)))
	{
		MessageBox(nullptr, strFilePath.c_str(), L"리소스 로딩 실패", MB_OK);
		delete pResource;
		return nullptr;
	}
	pResource->SetRelativePath(_strRelativePath);
	AddRes<T>(_strKey, (T*)pResource);

	return (T*)pResource;
}

template<typename T>
wstring CResMgr::GetNewResName()
{
	RES_TYPE type = GetType<T>();

	wstring strKey = L"New ";

	switch (type)
	{
	case RES_TYPE::PREFAB:
		strKey += L"Prefab";
		break;
	case RES_TYPE::MESHDATA:
		strKey += L"MeshData";
		break;
	case RES_TYPE::MATERIAL:
		strKey += L"Material";
		break;
	case RES_TYPE::TEXTURE:
		strKey += L"Texture";
		break;
	}

	wstring strTemp{};

	UINT iNum = 1;

	while (true)
	{
		strTemp = strKey;
		strTemp += std::to_wstring(iNum);

		if (nullptr != CResMgr::FindRes<CMaterial>(strTemp))
		{
			++iNum;
		}
		else
		{
			break;
		}
	}
	return strTemp;
}
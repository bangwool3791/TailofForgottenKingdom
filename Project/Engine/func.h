#pragma once

extern int g_ScrollUp;
extern int g_ScrollDown;
extern int g_ScrollClick;

template<typename T>
void Safe_Delete(T& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

template<typename T1, typename T2>
void Clear_VecOfMap(map<const T1, vector<T2*>>& _map)
{
	typename map<T1, vector<T2*>>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		iter->second.clear();
	}
	_map.clear();
}

template<typename T1, typename T2>
void Clear_VecOfUnMap(unordered_map<const T1, vector<T2*>>& _map)
{
	typename map<T1, vector<T2*>>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		iter->second.clear();
	}
	_map.clear();
}


template<typename T1, typename T2>
void Clear_VecOfUnoderedMap(unordered_map<const T1, vector<T2*>>& _map)
{
	typename unordered_map<T1, vector<T2*>>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		iter->second.clear();
	}
	_map.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2*>& _map)
{
	typename map<T1, T2*>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		if (nullptr != iter->second)
			delete iter->second;
	}
	_map.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	typename map<T1, T2>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		iter->second = nullptr;
	}
	_map.clear();
}


template<typename T, int SIZE>
void Safe_Del_Array(T* (&_arr)[SIZE])
{
	for (int i = 0; i < SIZE; ++i)
	{
		if (nullptr != _arr[i])
			delete _arr[i];
	}
}

template<typename T>
void Safe_Del_Array(T& arr)
{
	for (auto Iter{ arr.begin() }; Iter != arr.end(); ++Iter)
	{
		delete* Iter;
		(*Iter) = nullptr;
	}
}

template<typename T>
void Safe_Del_Vec(vector<T*>& _vec)
{
	//typename vector<T*>::iterator iter = _vec.begin();
	for (size_t i{}; i < _vec.size(); ++i)
	{
		if (nullptr != (_vec[i]))
			delete (_vec[i]);
	}
	_vec.clear();
}


string WStringToString(const wstring& _str);
wstring StringToWString(const string& _str);

template<typename T>
void tick_function(T& container)
{
	for (auto iter{ container.begin() }; iter != container.end(); ++iter)
	{
		iter->tick();
	}
}

template<typename T>
void finaltick_function(T& container)
{
	for (auto iter{ container.begin() }; iter != container.end(); ++iter)
	{
		iter->finaltick();
	}
}

template<typename T>
void render_function(T& container)
{
	for (auto iter{ container.begin() }; iter != container.end(); ++iter)
	{
		iter->render();
	}
}

template<typename T>
void clear_function(T& container)
{
	for (auto iter{ container.begin() }; iter != container.end(); ++iter)
	{
		iter->clear();
	}
}

class CEntity;
inline bool IsValid(CEntity* _pCom)
{
	if (nullptr == _pCom)
		return false;
	else
		return true;
}

class CGameObject;
void Instantiate(CGameObject* _pNewObj, int _iLayerIdx = 0);
void Instantiate(CGameObject* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx = 0);
void Instantiate(CGameObject* _pNewObj, CGameObject* _pOwner, int _iLayerIdx = 0);

void SaveStringToFile(const string& _str, FILE* _pFile);
void LoadStringFromFile(string& _str, FILE* _pFile);

void SaveWStringToFile(const wstring& _str, FILE* _pFile);
void LoadWStringFromFile(wstring& _str, FILE* _pFile);

// Relative Path 가져오기
wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath);

#include "CResMgr.h"
#include "Ptr.h"

class CRes;
void AddRes(CRes* _Res, RES_TYPE _Type);
void DeleteRes(CRes* _Res, RES_TYPE _Type);

template<typename T>
void SaveResourceRef(Ptr<T> _res, FILE* _pFile)
{
	int iExist = !!_res.Get();

	fwrite(&iExist, sizeof(int), 1, _pFile);

	if (iExist)
	{
		SaveWStringToFile(_res->GetKey(), _pFile);
		SaveWStringToFile(_res->GetRelativePath(), _pFile);
	}
}

template<typename T>
void LoadResourceRef(Ptr<T>& _Res, FILE* _pFile)
{
	int iExist = 0;

	fread(&iExist, sizeof(int), 1, _pFile);

	if (iExist)
	{
		wstring strKey, strRelativePath;
		LoadWStringFromFile(strKey, _pFile);
		LoadWStringFromFile(strRelativePath, _pFile);
		_Res = CResMgr::GetInst()->Load<T>(strKey, strRelativePath);
	}
}



const char* ToString(COMPONENT_TYPE);
const wchar_t* ToWString(COMPONENT_TYPE);

const char* ToString(OBJECT_TYPE);
const wchar_t* ToWString(OBJECT_TYPE);

const char* ToString(RES_TYPE);
const wchar_t* ToWString(RES_TYPE);


#ifdef _DEBUG
void DebugDrawRect(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawCircle(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration = 0.f);
void DebugDrawCube(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawSphere(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration = 0.f);
void DebugDrawCone(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawFrustum(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fDuration = 0.f);
#endif

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat);

bool closeEnough(const float& a, const float& b
	, const float& epsilon = std::numeric_limits<float>::epsilon());
Vec3 DecomposeRotMat(const Matrix& _matRot);
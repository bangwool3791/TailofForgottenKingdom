#include "pch.h"
#include "func.h"

#include "CRes.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CEventMgr.h"
#include "CRes.h"
#include "CRenderMgr.h"
#include "CKeyMgr.h"
#include "CDevice.h"

int g_ScrollUp;
int g_ScrollDown;
int g_ScrollClick;

void AddRes(CRes* _Res, RES_TYPE _Type)
{
	_Res->AddRef();

	tEvent evn = {};
	evn.eType = EVENT_TYPE::ADD_RES;
	evn.wParam = (DWORD_PTR)_Type;
	evn.lParam = (DWORD_PTR)_Res;
	CEventMgr::GetInst()->AddEvent(evn);
}

void DeleteRes(CRes* _Res, RES_TYPE _Type)
{
	_Res->AddRef();

	tEvent evn = {};
	evn.eType = EVENT_TYPE::DELETE_RES;
	evn.wParam = (DWORD_PTR)_Type;
	evn.lParam = (DWORD_PTR)_Res;
	CEventMgr::GetInst()->AddEvent(evn);
}

wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath)
{
	wstring strRelativePath;
	if (-1 == _strPath.find(_strBase))
	{
		return strRelativePath;
	}

	strRelativePath = _strPath.substr(_strBase.length(), _strPath.length());
	return strRelativePath;
}

void Instantiate(CGameObject* _pNewObj, int _iLayerIdx)
{
	tEvent evn{};

	evn.eType = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_pNewObj;
	evn.lParam = (DWORD_PTR)_iLayerIdx;

	CEventMgr::GetInst()->AddEvent(evn);
}

void Instantiate(CGameObject* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx)
{
	_pNewObj->Transform()->SetRelativePos(_vWorldPos);

	tEvent evn{};

	evn.eType = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_pNewObj;
	evn.lParam = (DWORD_PTR)_iLayerIdx;

	CEventMgr::GetInst()->AddEvent(evn);
}

void Instantiate(CGameObject* _pNewObj, CGameObject* _pOwner, int _iLayerIdx)
{
	tEvent evn{};

	evn.eType = EVENT_TYPE::ADD_CHILD;
	evn.wParam = (DWORD_PTR)_pNewObj;
	evn.lParam = (DWORD_PTR)_pOwner;

	CEventMgr::GetInst()->AddEvent(evn);
}

void DebugDrawRect(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::RECT, _vColor, _vPosition, _vScale, _vRotation, 0.f, _fDuration);
}

void DebugDrawCircle(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CIRCLE, _vColor, _vPosition, Vec3::Zero, Vec3::Zero, _fRadius, _fDuration);
}

void DebugDrawCube(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CUBE, _vColor, _vPosition, _vScale, _vRotation, 0.f, _fDuration);
}

void DebugDrawSphere(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::SPHERE, _vColor, _vPosition, Vec3::Zero, Vec3::Zero, _fRadius, _fDuration);
}

void DebugDrawCone(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CONE, _vColor, _vPosition, _vScale, _vRotation, 0.f, _fDuration);
}

void DebugDrawFrustum(Vec4 _vColor, Vec3 _vPosition,Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::FRUSTUM, _vColor, _vPosition, Vec3{1.f, 1.f, 1.f}, _vRotation, 0.f, _fDuration);
}


const char* ToString(COMPONENT_TYPE _type)
{
	return COMPONENT_TYPE_CHAR[(UINT)_type];
}

const wchar_t* ToWString(COMPONENT_TYPE _type)
{
	return COMPONENT_TYPE_WCHAR[(UINT)_type];
}

const char* ToString(OBJECT_TYPE _type)
{
	return OBJECT_TYPE_CHAR[(UINT)_type];
}

const wchar_t* ToWString(OBJECT_TYPE _type)
{
	return OBJECT_TYPE_WCHAR[(UINT)_type];
}

const char* ToString(RES_TYPE _type)
{
	return RES_TYPE_CHAR[(UINT)_type];
}

const wchar_t* ToWString(RES_TYPE _type)
{
	return RES_TYPE_WCHAR[(UINT)_type];
}

void SaveStringToFile(const string& _str, FILE* _pFile)
{
	size_t iLen = _str.length();
	fwrite(&iLen, sizeof(size_t), 1, _pFile);
	fwrite(_str.c_str(), sizeof(char), iLen, _pFile);
}

void LoadStringFromFile(string& _str, FILE* _pFile)
{
	size_t iLen = _str.length();
	fread(&iLen, sizeof(size_t), 1, _pFile);
	char szbuffer[256] = {};
	fread(szbuffer, sizeof(char), iLen, _pFile);
	_str = szbuffer;
}

void SaveWStringToFile(const wstring& _str, FILE* _pFile)
{
	size_t iLen = _str.length();
	fwrite(&iLen, sizeof(size_t), 1, _pFile);
	fwrite(_str.c_str(), sizeof(wchar_t), iLen, _pFile);
}

void LoadWStringFromFile(wstring& _str, FILE* _pFile)
{
	size_t iLen = _str.length();
	fread(&iLen, sizeof(size_t), 1, _pFile);
	wchar_t szbuffer[256] = {};
	fread(szbuffer, sizeof(wchar_t), iLen, _pFile);
	_str = szbuffer;
}

string WStringToString(const wstring& _str)
{
	return string(_str.begin(), _str.end());
}

wstring StringToWString(const string& _str)
{
	return wstring(_str.begin(), _str.end());
}


bool closeEnough(const float& a, const float& b
	, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}

Vec3 DecomposeRotMat(const Matrix& _matRot)
{
	// _mat 을 분해 후 다시 행렬 만들기	
	Vec4 vMat[4];

	vMat[0] = Vec4(_matRot._11, _matRot._12, _matRot._13, _matRot._14);
	vMat[1] = Vec4(_matRot._21, _matRot._22, _matRot._23, _matRot._24);
	vMat[2] = Vec4(_matRot._31, _matRot._32, _matRot._33, _matRot._34);
	vMat[3] = Vec4(_matRot._41, _matRot._42, _matRot._43, _matRot._44);

	/*XMStoreFloat4(&vMat[0], _matRot._11);
	XMStoreFloat4(&vMat[1], _matRot.r[1]);
	XMStoreFloat4(&vMat[2], _matRot.r[2]);
	XMStoreFloat4(&vMat[3], _matRot.r[3]);*/

	Vec3 vNewRot;
	if (closeEnough(vMat[0].z, -1.0f)) {
		float x = 0; //gimbal lock, value of x doesn't matter
		float y = XM_PI / 2;
		float z = x + atan2f(vMat[1].x, vMat[2].x);
		vNewRot = Vec3{ x, y, z };
	}
	else if (closeEnough(vMat[0].z, 1.0f)) {
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2f(-vMat[1].x, -vMat[2].x);
		vNewRot = Vec3{ x, y, z };
	}
	else { //two solutions exist
		float y1 = -asinf(vMat[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(vMat[1].z / cosf(y1), vMat[2].z / cosf(y1));
		float x2 = atan2f(vMat[1].z / cosf(y2), vMat[2].z / cosf(y2));

		float z1 = atan2f(vMat[0].y / cosf(y1), vMat[0].x / cosf(y1));
		float z2 = atan2f(vMat[0].y / cosf(y2), vMat[0].x / cosf(y2));

		//choose one solution to return
		//for example the "shortest" rotation
		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			vNewRot = Vec3{ x1, y1, z1 };
		}
		else {
			vNewRot = Vec3{ x2, y2, z2 };
		}
	}
	return vNewRot;
}

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = (float)_mat.Get(i, j);
		}
	}
	return mat;
}
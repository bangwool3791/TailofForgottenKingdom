
#pragma once

class CCamera;
class CLight2D;
class CLight3D;
class CStructuredBuffer;

class CMRT;

#include "CLight2D.h"
#include "CLight3D.h"
#include "CTexture.h"

class CRenderMgr
	: public CSingleton<CRenderMgr>
{
private:
	CRenderMgr();
	virtual ~CRenderMgr();
	friend class CSingleton<CRenderMgr>;
private:
	CMRT* m_arrMRT[(UINT)MRT_TYPE::END];
	CCamera* m_EditorCam;
	CCamera* m_EnvCam;

	vector<CCamera*>			m_vecCam;			// 현재 레벨에 있는 모든 카메라

	vector<CLight2D*>			m_vecLight2D;		// 현재 레벨에 있는 모든 2D 광원
	vector<CLight3D*>			m_vecLight3D;		// 현재 레벨에 있는 모든 2D 광원

	CStructuredBuffer* m_pLight2DBuffer;
	CStructuredBuffer* m_pLight3DBuffer;
	vector<tDebugShapeInfo>		m_DebugDrawInfo;	// 현재 레벨에 있는 모든 카메라

	Ptr<CTexture>				m_RTCopyTex;
	Ptr<CTexture>				m_CopyColorTex;
	Ptr<CTexture>				m_CopyPositionTex;
public:
	void RegisterCamera(auto _pCam) { m_vecCam.push_back(_pCam); }
	void RegisterLight2D(auto _pLight) { m_vecLight2D.push_back(_pLight); }
	int RegisterLight3D(auto _pLight)
	{
		m_vecLight3D.push_back(_pLight);
		return m_vecLight3D.size() - 1;
	}
	void RegisterEditCam(auto _pCam) { m_EditorCam = _pCam; }
	// 렌더타겟을 카피텍스쳐로 복사
	void CopyRenderTarget();
	void CopyColorTarget();
	void CopyPositionTarget();
	void SetEnvCamera(CCamera* pCam) { m_EnvCam = pCam; }
	CCamera* GetMainCam();
	CMRT* GetMRT(MRT_TYPE _type) { return m_arrMRT[(UINT)_type]; }
	const vector<CLight3D*>& GetLight3D() { return m_vecLight3D; }
	void DebugDraw(DEBUG_SHAPE _eShape, Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fRadius, float _fDuration)
	{
#ifdef _DEBUG
		m_DebugDrawInfo.push_back(tDebugShapeInfo{ _eShape, _vColor, _vPosition, _vScale, _vRotation, _fRadius, _fDuration, 0.f });
#endif
	}

	vector<tDebugShapeInfo>& GetDebugDrawInfo() { return m_DebugDrawInfo; }

private:
	void UpdateNoiseTexture();
	void UpdateLight2D();
	void UpdateLight3D();
public:
	void init();
	void tick();
	void render();
	void render(const vector<CGameObject*>& obj);
	void render_env(const vector<CGameObject*>& obj, const wstring& texName, wstring path);
	void clear();
	void render_game();
	void render_editor();
	void render_editor(const vector<CGameObject*>& obj);
	void render_dynamic_shadowdepth();
	void render_dynamic_shadowdepth(const vector<CGameObject*>& obj);

	void render_static_shadowdepth();
	void render_static_shadowdepth(const vector<CGameObject*>& obj);

	void CreateMRT();
	void CreateMRT(const wstring& wstr, MRT_TYPE eType);
	void ClearMRT();
	void ClearMRT(MRT_TYPE _eType);
};
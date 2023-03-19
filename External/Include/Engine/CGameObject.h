#pragma once
#include "CEntity.h"
#include "smallobjallocator.h"
class CComponent;
class CCamera;
class CAnimator2D;
class CTransform;
class CLight2D;
class CLight3D;
class CTerrain;
class CTileMap;
class CParticleSystem;
class CRenderComponent;
class CMeshRender;
class CScript;
class CCollider2D;
class CCollider3D;
class CDecal;
class CSkyBox;
class CLandScape;
class CWaveRenderer;

#define GET_COMPONENT(classname, CLASSNAME ) C##classname* classname() { return ( C##classname*)GetComponent(COMPONENT_TYPE::CLASSNAME); }


static char CGameObjectKey[100] = "CGameObject";
class CGameObject
	:public CEntity
	//template type -> map 분기 처리
	, public SmallObjAllocator<CGameObject, OBJECTPOOL_SIZE, CGameObjectKey>
{
private:
	array<CComponent*, (UINT)COMPONENT_TYPE::END> m_arrCom;
	vector<CScript*>							  m_vecScripts;
	CRenderComponent* m_pRenderComponent{};
protected:
	//부모 오브젝트
	CGameObject* m_pParent;
	vector<CGameObject*>						  m_vecChild;
	int											  m_iLayerIdx;
	bool										  m_bDead;
public:
	CGameObject* Get_Parent() { return m_pParent; }
	const vector<CGameObject*>& GetChilds() const { return m_vecChild; }
	const vector<CScript*>& GetScripts() const { return m_vecScripts; }
	int GetLayerIndex() { return m_iLayerIdx; }
	void SetLayerIndex(int _iLayerIndex) { m_iLayerIdx = _iLayerIndex; }

	CGameObject* GetChild(const wstring& _key);
	bool IsDead() { return m_bDead; }
	void Destroy();
public:
	virtual void begin();
	virtual void tick();
	virtual void finaltick();
	virtual void render();
public:
	CRenderComponent* GetRenderComponent() { return m_pRenderComponent; }

public:
	friend class CLayer;
	friend class CEventMgr;
public:
	void AddComponent(CComponent* _pComponent);
	void DestroyComponent(COMPONENT_TYPE _eComType);
	void AddChild(CGameObject* _pGameObejct);

	bool DeleteComponent(const string& _strName);
	bool DeleteScript(const string& _strName);
	CComponent* GetComponent(COMPONENT_TYPE _eComType);

	GET_COMPONENT(Transform, TRANSFORM);
	GET_COMPONENT(MeshRender, MESHRENDER);
	GET_COMPONENT(Camera, CAMERA);
	GET_COMPONENT(Collider2D, COLLIDER2D);
	GET_COMPONENT(Collider3D, COLLIDER3D);
	GET_COMPONENT(Animator2D, ANIMATOR2D);
	GET_COMPONENT(Light2D, LIGHT2D);
	GET_COMPONENT(Light3D, LIGHT3D);
	GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);
	GET_COMPONENT(TileMap, TILEMAP);
	GET_COMPONENT(Terrain, TERRAIN);
	GET_COMPONENT(Decal, DECAL);
	GET_COMPONENT(SkyBox, SKYBOX);
	GET_COMPONENT(LandScape, LANDSCAPE);
	GET_COMPONENT(WaveRenderer, WAVERENDERER);

	template<typename T>
	T* GetScript();
	template<typename T>
	T* GetScript(const wstring& _name);
	void DisconnectFromParent();

	CLONE(CGameObject);
public:
	CGameObject();
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();
};

template<typename T>
T* CGameObject::GetScript()
{
	for (size_t i{ 0 }; i < m_vecScripts.size(); ++i)
	{
		T* pScript = dynamic_cast<T*>(m_vecScripts[i]);
		if (nullptr != pScript)
			return pScript;
	}
	return nullptr;
}

template<typename T>
T* CGameObject::GetScript(const wstring& _name)
{
	for (size_t i{ 0 }; i < m_vecScripts.size(); ++i)
	{
		T* pScript = dynamic_cast<T*>(m_vecScripts[i]);

		if (!pScript)
			return nullptr;

		if (!lstrcmp(_name.c_str(), pScript->GetName().c_str()))
		{
			return pScript;
		}
	}
	return nullptr;
}
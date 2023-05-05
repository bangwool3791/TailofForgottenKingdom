#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

/*
* 매쉬
* 재질
*/
struct tMtrlSet
{
	Ptr<CMaterial>  pCurMtrl;       // 현재 사용 할 메테리얼
	Ptr<CMaterial>  pSharedMtrl;    // 공유 메테리얼
	Ptr<CMaterial>  pDynamicMtrl;   // 공유 메테리얼의 복사본    
};

class CRenderComponent
	:public CComponent
{
public:
	CRenderComponent(COMPONENT_TYPE _eComponentType);
	CRenderComponent(const CRenderComponent& rhs);
	virtual ~CRenderComponent();
private:
	bool					m_bDynamicShadow;
	bool                    m_bUseFrustumCulling;
	Ptr<CMesh>				m_pMesh;
	vector<tMtrlSet>        m_vecMtrls;     // 재질    
	ShadowType              m_eShadowType;
public:
	void SetMesh(Ptr<CMesh> _pMesh);
	Ptr<CMesh> GetMesh() { return m_pMesh; }

	UINT GetMtrlCount() { return (UINT)m_vecMtrls.size(); }

	void ClearMaterials();

	void SetSharedMaterial(Ptr<CMaterial> _pMtrl, UINT _iIdx);

	const vector<tMtrlSet>& GetMaterials() { return m_vecMtrls; }
	Ptr<CMaterial> GetCurMaterial(UINT _iIdx);
	Ptr<CMaterial> GetSharedMaterial(UINT _idx);
	Ptr<CMaterial> GetDynamicMaterial(UINT _iIdx);
	ULONG64 GetInstID(UINT _iMtrlIdx);
	void SetFrustumCulling(bool _Use) { m_bUseFrustumCulling = _Use; }
	bool IsUseFrustumCulling() { return m_bUseFrustumCulling; }
public:
	virtual CRenderComponent* Clone() = 0;
protected:
	INSTANCING_TYPE m_eInsType;
public:
	void SetInstancingType(INSTANCING_TYPE _eType) { m_eInsType = _eType; }
	INSTANCING_TYPE GetInstancingType() { return m_eInsType; }
	void SetShadowType(ShadowType eType) { m_eShadowType = eType; }
	ShadowType GetShadowType() { return m_eShadowType; }
public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
public:
	virtual void render() = 0;
	virtual void render_depthmap();
	virtual void render(UINT _iSubset) = 0;
};

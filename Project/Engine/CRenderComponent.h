#pragma once
#include "CComponent.h"

#include "CMesh.h"
#include "CMaterial.h"

/*
* 매쉬
* 재질
*/
class CRenderComponent
	:public CComponent
{
public:
	CRenderComponent(COMPONENT_TYPE _eComponentType);
	CRenderComponent(const CRenderComponent& rhs);
	virtual ~CRenderComponent()
	{
		m_pDynamicMtrl = nullptr;
	}
private:
	Ptr<CMesh>				m_pMesh;
	Ptr<CMaterial>			m_pSharedMtrl;
	Ptr<CMaterial>			m_pDynamicMtrl;
	Ptr<CMaterial>			m_pCurMtrl;

public:
	void SetMesh(Ptr<CMesh> _pMesh) { m_pMesh = _pMesh; }
	Ptr<CMesh> GetMesh() { return m_pMesh; }

	void SetSharedMaterial(Ptr<CMaterial> _pMtrl) 
	{ 
		m_pSharedMtrl = _pMtrl; 
		m_pCurMtrl = _pMtrl; 
	}
	Ptr<CMaterial> GetSharedMaterial();

	Ptr<CMaterial> GetCurMaterial() 
	{
 		return m_pCurMtrl; 
	}
	Ptr<CMaterial> GetDynamicMaterial();
public:
	virtual CRenderComponent* Clone() = 0;
protected:
	INSTANCING_TYPE m_eInsType;
public:
	void SetInstancingType(INSTANCING_TYPE _eType) { m_eInsType = _eType; }
	INSTANCING_TYPE GetInstancingType() { return m_eInsType; }
public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
public:
	virtual void render() = 0;
	virtual void render_Instancing() = 0;
};

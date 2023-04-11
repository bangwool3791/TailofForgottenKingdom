#pragma once
#include "CRenderComponent.h"
#include "CMesh.h"
#include "CMaterial.h"

static char CMeshRender_Key[100] = "CMeshRender";
class CMeshRender
	:public CRenderComponent
	, public SmallObjAllocator<CMeshRender, OBJECTPOOL_SIZE, CMeshRender_Key>
{
public:
	virtual void tick() override;
	virtual void finaltick() override;
	virtual void render();
	virtual void render(UINT _iSubset) override;
public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CMeshRender);
public :
	CMeshRender();
	CMeshRender(INSTANCING_TYPE _eInsType);
	CMeshRender(const CMeshRender& rhs);
	virtual ~CMeshRender();
};

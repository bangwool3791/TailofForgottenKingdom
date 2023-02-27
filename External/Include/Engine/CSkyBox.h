#pragma once
#include "CRenderComponent.h"

enum class SKYBOX_TYPE
{
	SPHERE,
	CUBE,
};

class CSkyBox :
	public CRenderComponent
{
private:
	SKYBOX_TYPE         m_Type;
	Ptr<CTexture>       m_SkyBoxTex;
public:
    void SetType(SKYBOX_TYPE _type);
    void SetSkyBoxTex(Ptr<CTexture> _tex);

public:
    virtual void finaltick() override;
    virtual void render() override;
	virtual void render_Instancing() {};
public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

    CLONE(CSkyBox);
public:
    CSkyBox();
    ~CSkyBox();
};
#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"
#include "CTexture.h"

class CStructuredBuffer;
class CGameObject;

class CTerrain :
    public CRenderComponent
{
public:
    enum class TILE_MODE { INGAME, EDIT };
private:
    vector<Ptr<CTexture>>   m_AtlasTex;     // Ÿ�ϸ��� ����ϴ� ��Ʋ�� �̹���   
    Vec2                    m_vTileCount;   // Ÿ�ϸ� ���� ���� ����
    CStructuredBuffer*      m_TileBuffer;   // �� Ÿ���� ��Ʋ�� �������� ����ü

    CGameObject*            m_pCamera;
    Vec4                    m_vCameraPos;
public:
    void SetCamera(CGameObject* _pCamera) { m_pCamera = _pCamera; }
    void SetTileAtlas(Ptr<CTexture> _AtlasTex) { m_AtlasTex.push_back(_AtlasTex); }
    void SetTextureID(Ray _ray, UINT i);
public:
    virtual void begin() override;
    virtual void finaltick() override;
    virtual void render() override;
    virtual void render_Instancing() override {}

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    CLONE(CTerrain);
public:
    CTerrain();
    ~CTerrain();
};


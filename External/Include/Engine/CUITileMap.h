#pragma onceCUitileMap
#include "CRenderComponent.h"

#include "Ptr.h"
#include "CTexture.h"

class CStructuredBuffer;
class CGameObject;

class CUitileMap :
    public CRenderComponent
{
private:
    Ptr<CTexture>           m_AtlasTex;    
    Vec2                    m_vTileCount;  
    CStructuredBuffer*      m_TileBuffer;  

    CGameObject*            m_pCamera;
    Vec4                    m_vCameraPos;
public:
    void SetCamera(CGameObject* _pCamera) { m_pCamera = _pCamera; }
public:
    virtual void begin() override;
    virtual void finaltick() override;
    virtual void render() override;
    virtual void render_Instancing() override {}

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    CLONE(CUitileMap);
public:
    CUitileMap();
    ~CUitileMap();
};


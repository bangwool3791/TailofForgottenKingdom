#pragma once
#include "UI.h"

class CGameObject;
class ComponentUI;
class CTerrain;
class CTileMap;

class TileMapUI :
    public UI
{
private:
    CGameObject*  m_pEditTerrainObject;
    CTerrain*     m_pEditTerrain;
    
private:
    Vec2         m_vTileSize;
private:
    virtual void begin() override;
    virtual void update() override;
    virtual void render_update() override;

public :
    void    Initialize(void* pAddr);
public:
    TileMapUI();
    ~TileMapUI();
};


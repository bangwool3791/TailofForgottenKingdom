#pragma once
#include "UI.h"
#include "imgui_filedialog.h"

class CGameObject;
class ComponentUI;
class CLandScape;
class CTileMap;

class TileMapUI :
    public UI
{
private:
    CGameObject*    m_pLandObj;
    CLandScape*     m_pLandScape;
    
private :
    bool                  m_bDialogLoad{};
    bool                  m_bDialogSave{};
    ImFileDialogInfo      m_fileDialogLoad;
    ImFileDialogInfo      m_fileDialogSave;
    pair<UINT, UINT>      m_tFaceid;

    int                  m_iXFaceCount;
    int                  m_iZFaceCount;
private:
    void LoadTextureFromEdit(const wstring& _path);
    void InitializeHeightMap();
public:
    virtual void begin() override;
    virtual void update() override;
    virtual void render_update() override;
public:
    TileMapUI();
    ~TileMapUI();
};


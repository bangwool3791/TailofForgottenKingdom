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
    ImTextureID           m_HeightImage;
    ImTextureID           m_BrushImage;
    CGameObject*          m_pLandObj;
    CGameObject*          m_pBrushObj;
    CLandScape*           m_pLandScape;
    
private :
    bool                  m_bDialogLoad{};
    bool                  m_bDialogSave{};
    bool                  m_bDialogBrushLoad{};
    ImFileDialogInfo      m_fileDialogBrushLoad;
    ImFileDialogInfo      m_fileDialogLoad;
    ImFileDialogInfo      m_fileDialogSave;
    pair<UINT, UINT>      m_tFaceid;

    int                  m_iXFaceCount;
    int                  m_iZFaceCount;

private:
    void FindTexture(const wstring& _path, Ptr<CTexture>& pTex);
    void LoadHeightMap(const wstring& _path, ImTextureID& _image);
    void LoadBrushMap(const wstring& _path, ImTextureID& _image);
    void InitializeHeightMap();
public:
    virtual void begin() override;
    virtual void update() override;
    virtual void render_update() override;
public:
    TileMapUI();
    ~TileMapUI();
};


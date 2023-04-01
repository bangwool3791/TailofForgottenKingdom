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
    ImTextureID                 m_HeightImage;
    ImTextureID                 m_BrushImage;
    CGameObject*                m_pLandObj;
    CGameObject*                m_pBrushObj;
    CLandScape*                 m_pLandScape;
    void SetColorTexture(DWORD_PTR _strColorKey);
    void SetDirTexture(DWORD_PTR _strMeshKey);
private :
    bool                        m_bDialogLoad{};
    bool                        m_bDialogSave{};

    bool                        m_bDialogTextureArrSave{};
    bool                        m_bDialogTextureArrLoad{};
    
    bool                        m_bDialogBrushLoad{};
    bool                        m_bDialogCubemapSave{};

    ImFileDialogInfo            m_fileDialogBrushLoad;
    ImFileDialogInfo            m_fileDialogLoad;
    ImFileDialogInfo            m_fileDialogSave;
    ImFileDialogInfo            m_fileDialogTextureArrSave;
    ImFileDialogInfo            m_fileDialogTextureArrLoad;
    ImFileDialogInfo            m_fileDialogCubemapSave;

    ImFileDialogInfo            m_fileDialogLandscapeSave;
    ImFileDialogInfo            m_fileDialogLandscapeLoad;
    bool                        m_bDialogLandscapeSave;
    bool                        m_bDialogLandscapeLoad;

    pair<UINT, UINT>            m_tFaceid;

    vector<pair<Ptr<CTexture>, ImTextureID>>      m_vecColorTexture;
    int                        m_iColorTexIdx = -1;
    vector<pair<Ptr<CTexture>, ImTextureID>>      m_vecDirTexture;
    int                        m_iDirTexIdx = -1;

    char                       m_szTexArrName[100];

    Ptr<CTexture>              m_TextureArray;
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


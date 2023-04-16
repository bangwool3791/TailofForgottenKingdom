#pragma once

#include "UI.h"
#include "imgui_filedialog.h"

#include <Engine\CAnimator3D.h>

class CGameObjectEx;
class ComponentUI;

class AnimationUI
	: public UI
{
private:
    CGameObjectEx*      m_pParentObj;
    CGameObjectEx*      m_pCurrentObj;
    //Animation Key
private:
    bool                m_bPuase = false;
    float               m_fTimeScale;
    char                m_szAnimation[128];
    tAnim3DFrm          m_tFrame;
    UINT                m_iMaxFarme;
    vector<string>      m_vecAniKey;
    UINT                m_iAniCurKey;
    UINT                m_iCurChild;
    string              m_strAniCurKey;
    string              m_strInputKey;
private:
    void Update_FrameListUI();
    void Update_Engine_Frame();
    void Update_GUI_Frame();
    void Render_Text(ImVec4 _vColor, Vec2 _vSize, string _str);
public:
    virtual void begin() {}
    virtual void update() override;
    virtual void render_update() override;

private:
    void Play(const wstring& str);
    void PlayAll(const wstring& str);
    void Pause();
    void Release();
    void Delete(const wstring& str);
    void DeleteAll(const wstring& str);
    void SetTimeScale();
public:
    void SetTargetObj(CGameObjectEx* _pObj);
public:
    AnimationUI();
    ~AnimationUI();
};

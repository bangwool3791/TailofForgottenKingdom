#pragma once
#include "ResUI.h"
class MeshDataUI :
    public ResUI
{
private:
    TEX_PARAM			m_eSelectTexParam;
    string				m_strRes;
    UINT                m_iCurMtrl = 0;
private:
    virtual void update() override;
    virtual void render_update() override;

private:
    void SetTexture(DWORD_PTR _strTexKey);
    void SetShader(DWORD_PTR _strShaderKey);
public:
    void ConvertGameObjectPath(const wstring& originPath, const wstring& _NewPath);

public:
    MeshDataUI();
    ~MeshDataUI();
};


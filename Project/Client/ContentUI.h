#pragma once
#include "UI.h"

class TreeUI;
class CGameObject;

class ContentUI :
    public UI
{
private:
    CGameObjectEx*     m_pLandScape;

    CPrefab*         m_pTargetPrefab;
    TreeUI*          m_Tree;
    bool             m_bDragEvent;
    vector<wstring>  m_vecContentName;
public:
    //���� ���ҽ� ����� Ʈ���� ����
    void ResetContent();
    // Content ���� �ؿ� ���ϵ��� �޸𸮿� �ҷ���
    void ReloadContent();

public:
    virtual void begin() override;
    virtual void update() override;
    virtual void render_update() override;
private:
    void SetResourceToInspector(DWORD_PTR _res);
    void SetDragObject(DWORD_PTR _res);
    void FindContentFileName(const wstring& _strFolderPath);
    RES_TYPE GetResTypeByExt(const wstring& _filename);
public:
    ContentUI();
    ~ContentUI();
};


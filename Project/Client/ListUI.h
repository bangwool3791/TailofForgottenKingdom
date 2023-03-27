#pragma once
#include "UI.h"

class ListUI :
    public UI
{
private:
    vector<string>  m_ItemList;
    int             m_iSelectedIdx;

    UI*             m_DBCInst;
    FUNC_1          m_DBCFunc;

    FUNC_1          m_DBCRightFunc;

public:
    void SetItemList(const vector<string>& _inList);
    void SetItemList(const vector<wstring>& _inList);

    void AddDynamicDBClicked(UI* _Inst, FUNC_1 _Func)
    {
        m_DBCInst = _Inst;
        m_DBCFunc = _Func;
    }

    void AddDynamicDBRightClicked(UI* _Inst, FUNC_1 _Func)
    {
        m_DBCInst = _Inst;
        m_DBCRightFunc = _Func;
    }

public:
    virtual void render_update() override;
    virtual void Close() override;

public:
    ListUI();
    ~ListUI();
};


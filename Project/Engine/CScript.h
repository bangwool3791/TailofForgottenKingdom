#pragma once
#include "CComponent.h"

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CResMgr.h"
#include "GlobalComponent.h"

enum class SCRIPT_PARAM
{
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    TEXTURE,
    MATERIAL,
    PREFAB,
    SOUND,
};

struct tScriptParam
{
    string strParamName;
    SCRIPT_PARAM Type;
    void* pParam;

};
class CScript :
    public CComponent
{
private:
    const int               m_iScriptType;
    vector<tScriptParam>    m_vecParam;
public:
    int GetScriptType() { return m_iScriptType; }
protected:
public:
    void AddScriptParam(SCRIPT_PARAM _eParam, const string& _name, void* _pParam)
    {
        m_vecParam.push_back(tScriptParam{ _name, _eParam ,_pParam });
    }
    const vector<tScriptParam>& GetScriptParam() { return m_vecParam; }

    void Activate() { m_bActive = true; }
    void Deactivate() { m_bActive = false; }
    bool IsActive() { return m_bActive; }
public:
    virtual void begin();
    virtual void tick();
    virtual void finaltick();

    virtual CScript* Clone() = 0;
public:
    CScript(int _iScript);
    virtual ~CScript();

public:
    virtual void BeginOverlap(CCollider* _pOther) = 0;
    virtual void Overlap(CCollider* _pOther) = 0;
    virtual void EndOverlap(CCollider* _pOther) = 0;
};


#pragma once
#include "Ptr.h"
#include "CEntity.h"

class CRes :
    public CEntity
{
private:
    wstring         m_strKey;
    const RES_TYPE  m_eResType;
    int             m_iRefCount;
    bool            m_bEngineRes;
protected:
    wstring         m_strRelativePath;
public:
    void AddRef()
    {
        ++m_iRefCount;
    }

    void Release()
    {
        --m_iRefCount;

        if ((m_iRefCount < 0))
        {
            int a = 0;
        }

        if (0 == m_iRefCount)
        {
            delete this;
        }
    }
public:
    int GetRefCount() const { return m_iRefCount; }
    //파일로 저장
    virtual void Save(FILE* pFile) {}
    // 파일로 부터 로딩
    virtual int Load(FILE* pFile) { return 0; }
    //파일로 저장
    virtual void Save(const wstring& _strRelativePath) = 0;
    // 파일로 부터 로딩
    virtual int Load(const wstring& _strFilePath) = 0;

protected:
    void SaveKeyPath(FILE* _pFile);
    void LoadKeyPath(FILE* _pFile);

public:
    const wstring& GetKey() const { return m_strKey; }
    const wstring& GetRelativePath() const { return m_strRelativePath; }
    RES_TYPE GetResType() const { return m_eResType; }
    bool IsEngineRes() const { return m_bEngineRes; }
public:
    void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
    void SetRelativePath(const wstring& _strRelativePath) { m_strRelativePath = _strRelativePath; }
    bool CheckRelativePath(const wstring& _strRelativePath);
    template<typename T>
    friend class Ptr;
    friend class CResMgr;
    /*
    * 추상 함수는 자식만 상속 받으면 됨.
    */
public:
    virtual CRes* Clone() = 0;
public:
    CRes(RES_TYPE _eResType, bool _bEngineRes = false);
    CRes(const CRes& _other);
    virtual ~CRes();
};


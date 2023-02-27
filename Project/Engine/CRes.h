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
    //���Ϸ� ����
    virtual void Save(FILE* pFile) {}
    // ���Ϸ� ���� �ε�
    virtual int Load(FILE* pFile) { return 0; }
    //���Ϸ� ����
    virtual void Save(const wstring& _strRelativePath) = 0;
    // ���Ϸ� ���� �ε�
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
    * �߻� �Լ��� �ڽĸ� ��� ������ ��.
    */
public:
    virtual CRes* Clone() = 0;
public:
    CRes(RES_TYPE _eResType, bool _bEngineRes = false);
    CRes(const CRes& _other);
    virtual ~CRes();
};


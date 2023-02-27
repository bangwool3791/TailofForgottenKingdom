#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"
#include "CTexture.h"

class CStructuredBuffer;
class CGameObject;

class CTileMap :
    public CRenderComponent
{
private:
    int                         m_iAlive;
private:
    tMtrlConst				    m_tConst;
    Ptr<CTexture>               m_AtlasTex;     // 타일맵이 사용하는 아틀라스 이미지   
    CStructuredBuffer*          m_TileBuffer;   // 각 타일의 아틀라스 참조정보 구조체
    vector<tTile>               m_vecInfo;
private:
    bool Picking(Vec3 vPos, UINT& iIndex);
public:
    virtual void begin() override;
    virtual void finaltick() override;
    virtual void render() override;
    virtual void render_Instancing() override {}

    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    tTile    GetInfo(int _iIndex);
    tTile    GetInfo(Vec3 _vPos);
    void     SetInfo(int _iIndex, UINT _iInfo);
    void     SetInfo(Vec3 _vPos, UINT _iInfo);
    int      Find(Vec3 _vPos, int left, int right);
    void     On();
    void     Off();
    CLONE(CTileMap);
public:
    CTileMap();
    ~CTileMap();
};


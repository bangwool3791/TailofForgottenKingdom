#pragma once
#include "CComputeShader.h"


class CStructuredBuffer;

class CWeightMapShader :
    public CComputeShader
{
private:
    CStructuredBuffer* m_pWeightMap;
    CStructuredBuffer* m_pRaycastInfo;

    Ptr<CTexture>       m_pBrushArrTex;
    Vec2                m_vBrushScale;
    int                 m_iBrushIdx;
    int                 m_iWeightIdx; // ����ġ ������ ��ġ
    float               m_iWeightClear;
    int                 m_iWidth;
    int                 m_iHeight;

public:
    void SetWeightMap(CStructuredBuffer* _pWeightMap, UINT _iWidth, UINT _iHeight)
    {
        m_pWeightMap = _pWeightMap;
        m_iWidth = _iWidth;
        m_iHeight = _iHeight;
    }

    void SetInputBuffer(CStructuredBuffer* _pRaycastData)
    {
        m_pRaycastInfo = _pRaycastData;
    }

    void SetBrushArrTex(Ptr<CTexture> _pBrushTex)
    {
        m_pBrushArrTex = _pBrushTex;
    }

    void SetBrushScale(Vec2 _vScale)
    {
        m_vBrushScale = _vScale;
    }

    void SetBrushIndex(int _iIdx)
    {
        m_iBrushIdx = _iIdx;
    }

    void SetWeightIdx(int _iIdx)
    {
        m_iWeightIdx = _iIdx;
    }

    void SetWeightClear(float _iIdx)
    {
        m_iWeightClear = _iIdx;
    }

    int GetWidth() { return m_iWidth; }
    int GetHeight() { return m_iHeight; }
    float GetWeightClear() { return m_iWeightClear; }

public:
    virtual void UpdateData();
    virtual void Clear();

public:
    CWeightMapShader();
    ~CWeightMapShader();
};
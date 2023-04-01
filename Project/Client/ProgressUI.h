#pragma once
#include "UI.h"

class ProgressUI :
    public UI
{
private:
    float m_progress = 0.0f;
    float m_progress_dir = 1.0f;
    bool  m_animate = true;
    float m_fFactor = 0.f;
public:
    virtual void render_update() override;
    virtual void Close() override;
    void SetFactor(float _fValue) { m_fFactor = _fValue; }
public:
    ProgressUI();
    ProgressUI(float _fFactor);
    ~ProgressUI();
};


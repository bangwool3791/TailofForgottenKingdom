#pragma once
#include "ResUI.h"

class TextureUI :
    public ResUI
{

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    TextureUI();
    ~TextureUI();
};


#pragma once
#include "ResUI.h"
class MeshUI :
    public ResUI
{

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    MeshUI();
    ~MeshUI();
};


#pragma once
#include <Engine/CCamera.h>

class CEditorCam :
	public CCamera
{
public:
	virtual void finaltick() override;
};
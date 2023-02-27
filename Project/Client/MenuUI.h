#pragma once
#include "UI.h"

class MenuUI :
	public UI
{
private :
	EDIT_MODE m_eEditMode;
public :
	virtual void render() override;
	virtual void render_update() override;
	MenuUI();
	~MenuUI();
};
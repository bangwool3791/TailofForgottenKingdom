#pragma once

#include "ComponentUI.h"

class CScript;

class ScriptUI :
	public ComponentUI
{
private:
	CScript* m_pTargetScript;

public:
	void SetTargetScript(CScript* _pScript)
	{
		m_pTargetScript = _pScript;
	}

	CScript* GetTargetScript()
	{
		return m_pTargetScript;
	}

public:
	virtual void update() override;
	virtual void render_update() override;
private :
	void render_scriptparam();
public:
	ScriptUI();
	~ScriptUI();
};
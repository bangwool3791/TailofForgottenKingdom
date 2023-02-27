#include "pch.h"
#include "CScript.h"

#include "CEngine.h"
#include "CDevice.h"
#include "CInterfaceMgr.h"

CScript::CScript(int _iScriptType)
	:CComponent(COMPONENT_TYPE::SCRIPT)
	,m_iScriptType(_iScriptType)
{
}

CScript::~CScript()
{

}

void CScript::begin()
{

}

void CScript::tick()
{
}

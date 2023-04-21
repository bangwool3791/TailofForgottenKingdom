#pragma once
#include <Engine\singleton.h>
#include "Sample.h"

class CGameObjectEx;

class NavMeshMgr
	: public CSingleton<NavMeshMgr>
{
private:

public:
	NavMeshMgr();
	~NavMeshMgr();
	friend class CSingleton<NavMeshMgr>;
};

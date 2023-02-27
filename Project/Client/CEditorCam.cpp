#include "pch.h"
#include "CEditorCam.h"

#include <Engine\CResMgr.h>

void CEditorCam::finaltick()
{
	CalcViewMat();

	CalcProjMat();
}

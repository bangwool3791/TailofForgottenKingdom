#include "pch.h"
#include "InspectorUI.h"

#include <Engine/CRes.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CGameObject.h>

#include "CAnimator2DUI.h"
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Collider2DUI.h"
#include "ShadowUI.h"
#include "Light2DUI.h"
#include "MeshUI.h"
#include "TextureUI.h"
#include "MaterialUI.h"
#include "MeshDataUI.h"
#include "ScriptUI.h"

/*
* Resource
*/
#include "ResUI.h"

InspectorUI::InspectorUI()
	: UI("Inspector")
	, m_TargetObj(nullptr)
	, m_arrComUI{}
	, m_arrResUI{}
{
	/*
	* 자동 조절
	*/
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = new TransformUI;	
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]->SetSize(ImVec2(0.f, 150.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]);

	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = new MeshRenderUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->SetSize(ImVec2(0.f, 150.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]);

	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = new Collider2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D]->SetSize(ImVec2(0.f, 150.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D]);

	//m_arrComUI[(UINT)COMPONENT_TYPE::SCRIPT] = new ShadowUI;
	//AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::SCRIPT]);

	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new CAnimator2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]->SetSize(ImVec2(0.f, 150.f));
	AddChild(m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]);
	//m_arrObjUI[(UINT)OBJECT_TYPE::LIGHT2D] = new Light2DUI;
	//m_arrObjUI[(UINT)OBJECT_TYPE::LIGHT2D]->SetSize(ImVec2(0.f, 150.f));
	//AddChild(m_arrObjUI[(UINT)OBJECT_TYPE::LIGHT2D]);

	// ResourceUI
	m_arrResUI[(UINT)RES_TYPE::MESH] = new MeshUI;
	m_arrResUI[(UINT)RES_TYPE::MESH]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::MESH]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::MESH]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::MESH]);

	m_arrResUI[(UINT)RES_TYPE::TEXTURE] = new TextureUI;
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::TEXTURE]);

	m_arrResUI[(UINT)RES_TYPE::MATERIAL] = new MaterialUI;
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::MATERIAL]);

	m_arrResUI[(UINT)RES_TYPE::MESHDATA] = new MeshDataUI;
	m_arrResUI[(UINT)RES_TYPE::MESHDATA]->SetSize(ImVec2(0.f, 0.f));
	m_arrResUI[(UINT)RES_TYPE::MESHDATA]->ShowSeperator(false);
	m_arrResUI[(UINT)RES_TYPE::MESHDATA]->Close();
	AddChild(m_arrResUI[(UINT)RES_TYPE::MESHDATA]);

	ScriptUI* pScript = new ScriptUI;
	pScript->Close();
	AddChild(pScript);
	m_vecScriptUI.push_back(pScript);
	
}

InspectorUI::~InspectorUI()
{	

}

void InspectorUI::update()
{
	if (!IsValid(m_TargetObj))
	{
		SetTargetObject(nullptr);
	}

	UI::update();
}

void InspectorUI::render_update()
{

}

void InspectorUI::SetTargetObject(CGameObject* _Target)
{
	// 리소스가 타겟인 상태였다면
	if (nullptr != _Target && nullptr != m_TargetRes)
	{
		SetTargetResource(nullptr);
	}

	if (nullptr == dynamic_cast<CGameObject*>(_Target))
		return;

	m_TargetObj = _Target;

	/*
	* Com객체 목룍 표시
	*/
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrComUI[i])
		{
			/*
			* 타겟 존재, 컴포넌트 미소유
			*/
			if (nullptr != m_TargetObj && nullptr == m_TargetObj->GetComponent((COMPONENT_TYPE)i))
			{
				m_arrComUI[i]->SetTarget(nullptr);
				m_arrComUI[i]->Close();
			}		
			/*
			* 컴포넌트 소유
			* 타겟 존재
			* 타겟 미 존재
			*/
			else
			{
				m_arrComUI[i]->SetTarget(_Target);

				if (nullptr != _Target)
				{
					m_arrComUI[i]->Open();
				}
				else
				{
					m_arrComUI[i]->Close();
				}
			}
		}		
	}	

	if (nullptr == m_TargetObj)
	{
		for (UINT i{}; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->Close();
		}
	}
	else
	{
		for (UINT i{}; i < m_vecScriptUI.size(); ++i)
			m_vecScriptUI[i]->Close();


		const vector<CScript*>& scripts = m_TargetObj->GetScripts();

		if (m_vecScriptUI.size() < scripts.size())
		{
			for (UINT i{}; i < scripts.size() - m_vecScriptUI.size(); ++i)
			{
				ScriptUI* pScript = new ScriptUI;
				pScript->Close();
				AddChild(pScript);
				m_vecScriptUI.push_back(pScript);
			}
		}

		for (UINT i{}; i < scripts.size(); ++i)
		{
			m_vecScriptUI[i]->Open();
			m_vecScriptUI[i]->SetTargetScript(scripts[i]);
		}
	}
	
	//auto pShadow = m_TargetObj->GetChild(L"Shadow");
	//auto pShadowScript = m_TargetObj->GetChild(L"Shadow")->GetScript<CShadowScript>(L"ShadowScript");

	//if (pShadowScript)
	//{
	//	m_arrComUI[(UINT)COMPONENT_TYPE::SCRIPT]->SetTarget(pShadow);
	//	m_arrComUI[(UINT)COMPONENT_TYPE::SCRIPT]->Open();
	//}
	//else
	//{
	//	m_arrComUI[(UINT)COMPONENT_TYPE::SCRIPT]->Close();
	//	m_arrComUI[(UINT)COMPONENT_TYPE::SCRIPT]->SetTarget(nullptr);
	//}
}

void InspectorUI::InitializeScriptUI()
{
	if (nullptr == m_TargetObj)
	{
		for (UINT i{}; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->Close();
		}
	}
	else
	{
		for (UINT i{}; i < m_vecScriptUI.size(); ++i)
			m_vecScriptUI[i]->Close();


		const vector<CScript*>& scripts = m_TargetObj->GetScripts();

		if (m_vecScriptUI.size() < scripts.size())
		{
			for (UINT i{}; i < scripts.size() - m_vecScriptUI.size(); ++i)
			{
				ScriptUI* pScript = new ScriptUI;
				pScript->Close();
				AddChild(pScript);
				m_vecScriptUI.push_back(pScript);
			}
		}

		for (UINT i{}; i < scripts.size(); ++i)
		{
			m_vecScriptUI[i]->Open();
			m_vecScriptUI[i]->SetTargetScript(scripts[i]);
		}
	}
}
void InspectorUI::SetTargetResource(CRes* _Resource)
{
	if (nullptr != m_TargetObj)
	{
		SetTargetObject(nullptr);
	}

	if (nullptr != _Resource)
	{
		if (nullptr != m_TargetRes && nullptr != m_arrResUI[UINT(m_TargetRes->GetResType())])
		{
			m_arrResUI[UINT(m_TargetRes->GetResType())]->Close();
		}
		//새로 지정된 리소스를 담당하는 UI를 활성화
		m_TargetRes = _Resource;
		RES_TYPE eType = m_TargetRes->GetResType();

		if ((int)eType < 0)
			return;
		if (nullptr != m_arrResUI[(UINT)eType])
		{
			m_arrResUI[(UINT)eType]->SetTarget(m_TargetRes);
			m_arrResUI[(UINT)eType]->Open();
		}
	}
	else
	{
		for (UINT i{}; i < (UINT)RES_TYPE::END; ++i)
		{
			if (nullptr != m_arrResUI[i])
			{
				m_arrResUI[i]->SetTarget(nullptr);
				m_arrResUI[i]->Close();
			}
		}
	}
}



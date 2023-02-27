#include "pch.h"
#include "CComponent.h"

CComponent::CComponent(COMPONENT_TYPE _eType)
	:m_eType{_eType}
	, m_pOwnerObject{ nullptr }
	, m_bActive{true}
{

}

CComponent::CComponent(const CComponent& rhs)
	:CEntity(rhs)
	,m_eType{rhs.m_eType}
	,m_pOwnerObject{nullptr}
	, m_bActive{ rhs.m_bActive }
{

}

CComponent::~CComponent()
{

}
#include "pch.h"
#include "TrailComUI.h"
#include <Engine\CTrailComponent.h>

TrailComUI::TrailComUI()
	: ComponentUI("TrailComponent", COMPONENT_TYPE::TRAIL)
{
}

TrailComUI::~TrailComUI()
{
}


void TrailComUI::update()
{
	ComponentUI::update();
}

void TrailComUI::render_update()
{
	ComponentUI::render_update();

	if (GetTarget() && GetTarget()->TrailComponent())
	{
		ImGui::Text("OffsetPos"); ImGui::SameLine(); 
		
		static Vec3 vOffset{};
		vOffset = GetTarget()->TrailComponent()->GetOffsetPos();

		if (ImGui::InputFloat3("##OffsetPos", vOffset))
		{
			GetTarget()->TrailComponent()->SetOffsetPos(vOffset);
		}
	}
}
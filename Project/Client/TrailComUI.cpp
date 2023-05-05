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
		ImGui::Text("TopOffsetPos"); ImGui::SameLine(); 
		
		static Vec3 vTopOffset{};
		vTopOffset = GetTarget()->TrailComponent()->GetTopOffsetPos();

		if (ImGui::InputFloat3("##OffsetPos1", vTopOffset))
		{
			GetTarget()->TrailComponent()->SetTopOffsetPos(vTopOffset);
		}

		ImGui::Text("BottomOffsetPos"); ImGui::SameLine();

		static Vec3 vBottomOffset{};
		vBottomOffset = GetTarget()->TrailComponent()->GetBottomOffsetPos();

		if (ImGui::InputFloat3("##OffsetPos2", vBottomOffset))
		{
			GetTarget()->TrailComponent()->SetBottomOffsetPos(vBottomOffset);
		}
	}
}
#include "pch.h"
#include "imgui.h"

void Vector3::ToRadian()
{
	x = (x / 180.f) * XM_PI;
	y = (y / 180.f) * XM_PI;
	z = (z / 180.f) * XM_PI;
}

void Vector3::ToDegree()
{
	x = (x / XM_PI) * 180.f;
	y = (y / XM_PI) * 180.f;
	z = (z / XM_PI) * 180.f;
}

Vec2::operator ImVec2() const { return ImVec2(x, y); }
Vector4& Vector4::operator=(const ImVec4& _v)
{
	x = _v.x;
	y = _v.y;
	z = _v.z;
	w = _v.w;
	return *this;
}


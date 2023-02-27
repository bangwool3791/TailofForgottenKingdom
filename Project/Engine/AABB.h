#pragma once

static int GAABBInstanceID = 0;

struct AABB
{
public:
	int X0, X1;
	int Y0, Y1;
	int InstanceID;

public:
	AABB() {}
	AABB(int x0, int x1, int y0, int y1)
		:X0{ x0 }
		, X1{ x1 }
		, Y0{ y0 }
		, Y1{ y1 }
		, InstanceID{ GAABBInstanceID++ }
	{
	}

	int Width() { return X1 - X0; }
	int Height() { return Y1 - Y0; }

	bool IsIncluded(AABB aabb)
	{
		return (aabb.X0 >= X0 && aabb.X1 <= X1 && aabb.Y0 >= Y0 && aabb.Y1 <= Y1);
	}

	bool IsIntersect(AABB aabb)
	{
		return (aabb.X0 >= X0 && aabb.X1 <= X1 && aabb.Y0 >= Y0 && aabb.Y1 <= Y1);
	}
};